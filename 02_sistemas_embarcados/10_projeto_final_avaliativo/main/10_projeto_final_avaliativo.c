#include <stdint.h>
// Includes do FreeRTOS (Sistema Operacional)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

// Includes dos seus Módulos
#include "mq5_sensor.h"
#include "buzzer_control.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

// Includes de utilitários do ESP-IDF
#include "nvs_flash.h" // Necessário para o Wi-Fi
#include "esp_netif.h" 
#include "esp_log.h"

static const char *TAG = "APP_MAIN";

/**
 * @brief Fila para comunicação entre o sensor de gás e o controle do buzzer.
 * A fila transporta inteiros simples (0 para OK, 1 para ALERTA).
 * Essa fila é criada em app_main() e passada para os módulos que precisam dela.
 */
QueueHandle_t queue_buzzer;
QueueHandle_t queue_mqtt;

EventGroupHandle_t wifi_event_group;

void gas_mqtt_task(void *param) {
    int status;

    QueueHandle_t local_q = (QueueHandle_t)param;

    while (1) {
        // Espera algo chegar na fila EXCLUSIVA do MQTT
        if (xQueueReceive(local_q, &status, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Task MQTT processando estado: %d", status);
            if (status == 1) {
                mqtt_manager_publish("sensor/gas/alerta", "PERIGO");
            } else {
                mqtt_manager_publish("sensor/gas/alerta", "NORMAL");
            }
        }
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Iniciando o Sistema Detector de Gás...");

    // 1. Inicializar o NVS (Non-Volatile Storage) - essencial para o Wi-Fi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Inicializar o TCP/IP stack
    ESP_ERROR_CHECK(esp_netif_init());

    // 3. Criar a Fila de Alertas e Event Group
    queue_buzzer = xQueueCreate(5, sizeof(int));
    queue_mqtt   = xQueueCreate(5, sizeof(int)); 

    wifi_event_group = xEventGroupCreate();

    if (queue_buzzer == NULL || queue_mqtt == NULL || wifi_event_group == NULL) {
        ESP_LOGE(TAG, "Erro de memória na criação das filas/eventos!");
        return;
    }
    
    ESP_LOGI(TAG, "Fila de alertas e grupo de eventos criados com sucesso.");

    // 4. Inicializar os Módulos de Hardware
    mq5_sensor_init(queue_buzzer, queue_mqtt);
    buzzer_control_init(queue_buzzer);

    // 5. Inicializar o Gerenciador de Wi-Fi (ele cria o event loop internamente)
    ESP_ERROR_CHECK(wifi_sta_init(wifi_event_group));

    // 6. Esperar até o Wi-Fi conectar e obter um IP
    ESP_LOGI(TAG, "Aguardando conexão Wi-Fi e obtenção de IP...");
    EventBits_t bits = xEventGroupWaitBits(wifi_event_group, WIFI_STA_IPV4_OBTAINED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);

    if (bits & WIFI_STA_IPV4_OBTAINED_BIT) {
        ESP_LOGI(TAG, "Wi-Fi e IP prontos. Iniciando MQTT...");
        mqtt_manager_init();
        
        // Pequeno delay para garantir conexão MQTT antes de publicar
        vTaskDelay(pdMS_TO_TICKS(2000));
        mqtt_manager_publish ("sensor/gas/status", "Sistema Online") ;

        xTaskCreate(gas_mqtt_task, "gas_mqtt_task", 4096, (void*)queue_mqtt, 5, NULL);
    } else {
        ESP_LOGE(TAG, "Falha ao conectar o Wi-Fi ou obter IP. MQTT não será iniciado.");
    }

    ESP_LOGI(TAG, "Inicialização completa. Tarefas em execução.");
}