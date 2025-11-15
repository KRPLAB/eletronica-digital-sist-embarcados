#include <stdint.h>
// Includes do FreeRTOS (Sistema Operacional)
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Includes dos seus Módulos
#include "mq5_sensor.h"
#include "buzzer_control.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

// Includes de utilitários do ESP-IDF
#include "nvs_flash.h" // Necessário para o Wi-Fi
#include "esp_log.h"

static const char *TAG = "APP_MAIN";

/**
 * @brief Fila para comunicação entre o sensor de gás e o controle do buzzer.
 * A fila transporta inteiros simples (0 para OK, 1 para ALERTA).
 * Essa fila é criada em app_main() e passada para os módulos que precisam dela.
 */
QueueHandle_t gas_alert_queue;

void app_main(void)
{
    ESP_LOGI(TAG, "Iniciando o Sistema Detector de Gás...");

    // 1. Inicializar o NVS (Non-Volatile Storage) - essencial para o Wi-Fi
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Criar a Fila de Alertas
    // xQueueCreate(tamanho_da_fila, tamanho_de_cada_item)
    gas_alert_queue = xQueueCreate(10, sizeof(int));
    if (gas_alert_queue == NULL)
        ESP_LOGE(TAG, "Falha ao criar a fila!");
    else
        ESP_LOGI(TAG, "Fila de alertas criada com sucesso.");

    // 3. Inicializar os Módulos
    // (Ainda em desenvolvimento para focar no sensor e buzzer primeiro)
    wifi_manager_init();
    mqtt_manager_init();

    mq5_sensor_init(gas_alert_queue);
    buzzer_control_init(gas_alert_queue);

    ESP_LOGI(TAG, "Inicialização completa. Tarefas em execução.");
}