#include <stdint.h>
// Includes do FreeRTOS (Sistema Operacional)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include <time.h>

// Includes dos seus Módulos
#include "mq5_sensor.h"
#include "buzzer_control.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

// Includes de utilitários do ESP-IDF
#include "nvs_flash.h" // Necessário para o Wi-Fi
#include "esp_netif.h" 
#include "esp_log.h"
#include "cJSON.h"
#include "esp_wifi.h" 
#include "esp_mac.h" 
#include "esp_sntp.h"

static const char *TAG = "APP_MAIN";

/**
 * @brief Fila para comunicação entre o sensor de gás e o controle do buzzer.
 * A fila transporta inteiros simples (0 para OK, 1 para ALERTA).
 * Essa fila é criada em app_main() e passada para os módulos que precisam dela.
 */
QueueHandle_t queue_buzzer;
QueueHandle_t queue_mqtt;
EventGroupHandle_t wifi_event_group;

// Constantes
#define SAMPLE_INTERVAL_MS 1000 // Leitura a cada 1 segundo
#define REPORT_INTERVAL_MS (5 * 60 * 1000) // 5 minutos
#define SAMPLES_COUNT (REPORT_INTERVAL_MS / SAMPLE_INTERVAL_MS)

// Obter MAC String
void get_mac_str(char *mac_str) {
    uint8_t mac[6];
    esp_wifi_get_mac(WIFI_IF_STA, mac);
    sprintf(mac_str, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

// Lógica de Registro (UUID)
void ensure_device_registered() {
    nvs_handle_t my_nvs_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_nvs_handle);
    size_t required_size = sizeof(device_uuid);

    // 1. Tenta ler UUID da NVS
    err = nvs_get_str(my_nvs_handle, "device_uuid", device_uuid, &required_size);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "UUID carregado da NVS: %s", device_uuid);
    } else {
        ESP_LOGW(TAG, "UUID não encontrado. Iniciando registro...");
        
        char mac_str[13];
        get_mac_str(mac_str);

        // 2. Inscreve no tópico de resposta: dispositivos/registro/<MAC>
        char sub_topic[64];
        sprintf(sub_topic, "dispositivos/registro/%s", mac_str);
        mqtt_subscribe(sub_topic);

        // 3. Publica solicitação: dispositivos/registro (Payload: MAC)
        // Usando JSON para ficar padronizado
        cJSON *root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "mac_address", mac_str);
        char *json_str = cJSON_PrintUnformatted(root);
        
        mqtt_manager_publish("dispositivos/registro", json_str);
        
        cJSON_Delete(root);
        free(json_str);

        ESP_LOGI(TAG, "Solicitação enviada. Aguardando resposta do backend...");

        // 4. Bloqueia até receber a resposta no mqtt_manager.c
        if (xSemaphoreTake(sem_uuid_received, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "UUID Recebido! Salvando na NVS...");
            
            // 5. Salva na NVS
            nvs_set_str(my_nvs_handle, "device_uuid", device_uuid);
            nvs_commit(my_nvs_handle);
        }
    }
    nvs_close(my_nvs_handle);
}

void sensor_logic_task(void *param) {
    int adc_value = 0;
    long long sum_values = 0;
    int samples_count = 0;
    
    // Controles de tempo
    // 5 minutos * 60 segs * 1000 ms | 1000 ms amostra = 300 amostras
    const int SAMPLES_REQUIRED = 300;

    char topic_pub[128];

    while (1) {
        if (xQueueReceive(queue_mqtt, &adc_value, portMAX_DELAY)) {
            // --- ALERTA IMEDIATO ---
            if (adc_value > GAS_THRESHOLD) {
                const char *nivel;
                if (adc_value < GAS_THRESHOLD * 1.2) {
                    nivel = "baixo";
                } else if (adc_value < GAS_THRESHOLD * 1.5) {
                    nivel = "medio";
                } else {
                    nivel = "alto";
                }


                sprintf(topic_pub, "%s/sensores/alertas/mq5", device_uuid);

                // ALERTA
                cJSON *json_alert = cJSON_CreateObject();
                cJSON_AddStringToObject(json_alert, "nivel", nivel);
                cJSON_AddStringToObject(json_alert, "mensagem", "GAS_DETECTADO");
                cJSON_AddNumberToObject(json_alert, "valor", adc_value);

                // Adiciona timestamp ISO 8601
                char timestamp[32];
                time_t now = time(NULL);
                struct tm tm_now;
                gmtime_r(&now, &tm_now);
                strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S.000Z", &tm_now);
                cJSON_AddStringToObject(json_alert, "timestamp", timestamp);
                cJSON_AddNumberToObject(json_alert, "sensor", 1); 

                char *str_alert = cJSON_PrintUnformatted(json_alert);
                mqtt_manager_publish(topic_pub, str_alert);

                cJSON_Delete(json_alert);
                free(str_alert);

                vTaskDelay(pdMS_TO_TICKS(1000)); 
            }

            // --- ACUMULA PARA MÉDIA ---
            sum_values += adc_value;
            samples_count++;

            // --- RELATÓRIO A CADA 5 MINUTOS ---
            if (samples_count >= SAMPLES_REQUIRED) {
                double media = (double)sum_values / samples_count;
                sprintf(topic_pub, "%s/sensores/leituras/1", device_uuid);

                // LEITURA
                cJSON *json_read = cJSON_CreateObject();
                cJSON_AddNumberToObject(json_read, "valor", (int)media);

                char timestamp[32];
                time_t now = time(NULL);
                struct tm tm_now;
                gmtime_r(&now, &tm_now);
                strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S.000Z", &tm_now);
                cJSON_AddStringToObject(json_read, "timestamp", timestamp);
                cJSON_AddNumberToObject(json_read, "sensor", 1);

                char *str_read = cJSON_PrintUnformatted(json_read);
                ESP_LOGI(TAG, "Publicando média (5 min): %.2f", media);
                mqtt_manager_publish(topic_pub, str_read);

                cJSON_Delete(json_read);
                free(str_read);

                sum_values = 0;
                samples_count = 0;
            }
        }
    }
}

void sync_time() {
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 10;
    while (timeinfo.tm_year < (2023 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Aguardando sincronização NTP...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Iniciando Sistema...");

    // 1. Inicializa NVS e Netif
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());

    // 2. Filas e Eventos
    queue_buzzer = xQueueCreate(10, sizeof(int));
    queue_mqtt   = xQueueCreate(20, sizeof(int)); 
    wifi_event_group = xEventGroupCreate();

    // 3. Hardware Init
    mq5_sensor_init(queue_buzzer, queue_mqtt);
    buzzer_control_init(queue_buzzer);

    // 4. Wi-Fi Init
    wifi_sta_init(wifi_event_group);

    // 5. Bloqueio com Confirmação: Espera Wi-Fi Conectar e IP
    ESP_LOGI(TAG, "Aguardando conexão Wi-Fi e obtenção de IP...");

    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, 
                                           WIFI_STA_IPV4_OBTAINED_BIT, 
                                           pdFALSE, 
                                           pdTRUE, 
                                           portMAX_DELAY);

    // 6. Verifica se realmente conectou
    if (bits & WIFI_STA_IPV4_OBTAINED_BIT) {
        ESP_LOGI(TAG, "Wi-Fi e IP prontos. Iniciando MQTT...");
        
        sync_time();
        
        mqtt_manager_init();
        
        // Pequeno delay para garantir handshake TCP do MQTT antes de tentar publicar/inscrever
        vTaskDelay(pdMS_TO_TICKS(2000)); 

        // 7. Lógica de Registro (Bloqueante)
        // O código só passa daqui se conseguir o UUID (seja lendo da NVS ou pedindo ao backend)
        ensure_device_registered();

        ESP_LOGI(TAG, "Dispositivo Registrado e Pronto. UUID: %s", device_uuid);
        
        char topic_status[128];
        sprintf(topic_status, "%s/status", device_uuid);
        mqtt_manager_publish(topic_status, "{\"status\": \"online\", \"version\": \"1.0\"}");

        // 8. Inicia a Task Principal (Média e Alertas)
        // Só cria a task depois de confirmar que tem Wi-Fi, MQTT e UUID
        xTaskCreate(sensor_logic_task, "sensor_logic", 4096, NULL, 5, NULL);
        
    } else {
        ESP_LOGE(TAG, "Falha crítica: Não foi possível obter IP. O sistema não iniciará a comunicação.");
    }

    ESP_LOGI(TAG, "Inicialização (Main) completa.");
}