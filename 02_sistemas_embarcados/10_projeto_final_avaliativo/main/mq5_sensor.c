#include <stdint.h>
#include "mq5_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_continuous.h"

// --- Configurações do Sensor ---
#define MQ5_CHANNEL     ADC_CHANNEL_6  // GPIO 34
#define GAS_THRESHOLD   2000            // Limiar de perigo (a ser ajustado conforme necessário melhor pesquisado a respeito do sensor MQ-5)
#define READ_INTERVAL_MS 1000

static const char *TAG = "MQ5_SENSOR";

static QueueHandle_t queue_handle_buzzer;
static QueueHandle_t queue_handle_mqtt;

static adc_oneshot_unit_handle_t adc_handle;
static adc_oneshot_chan_cfg_t chan_cfg;

/**
 * @brief Tarefa (Task) que lê o sensor em loop.
 * * Esta é a Task A. Ela passa a maior parte do tempo em "vTaskDelay" (dormindo).
 */
static void sensor_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Tarefa do sensor iniciada.");
    
    // Deixar o sensor aquecer por um tempo antes de começar as leituras
    vTaskDelay(pdMS_TO_TICKS(15000));

    int current_alert_state = -1;
    int new_alert_state = 0;
    int val_raw = 0;

    while (1) {
        if (adc_oneshot_read(adc_handle, MQ5_CHANNEL, &val_raw) != ESP_OK) {
            ESP_LOGE(TAG, "Falha ao ler ADC!");
            continue;
        }

        if (val_raw > GAS_THRESHOLD) {
            new_alert_state = 1;
            ESP_LOGW(TAG, "Gás acima do limite! Valor ADC: %d", val_raw);
        } else {
            new_alert_state = 0;
            ESP_LOGI(TAG, "Nível de gás normal. Valor ADC: %d", val_raw);
        }

        // Envia para a fila sempre que o estado mudar
        if (new_alert_state != current_alert_state) {
            current_alert_state = new_alert_state;
            ESP_LOGI(TAG, "Mudança de estado: %d. Distribuindo mensagens...", current_alert_state);

            if (xQueueSend(queue_handle_buzzer, &current_alert_state, pdMS_TO_TICKS(100)) != pdTRUE) {
                ESP_LOGE(TAG, "Falha ao enviar para a fila!");
            }

            if (xQueueSend(queue_handle_mqtt, &current_alert_state, pdMS_TO_TICKS(100)) != pdTRUE) {
                ESP_LOGE(TAG, "Falha ao enviar para a fila!");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(READ_INTERVAL_MS));
    }
}

/**
 * @brief Função de inicialização do módulo.
 */
void mq5_sensor_init(QueueHandle_t q_buzz, QueueHandle_t q_mqtt) {
    queue_handle_buzzer = q_buzz;
    queue_handle_mqtt = q_mqtt;

    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    
    adc_oneshot_new_unit(&init_cfg, &adc_handle);

    chan_cfg.atten = ADC_ATTEN_DB_12;
    chan_cfg.bitwidth = ADC_BITWIDTH_12;

    adc_oneshot_config_channel(adc_handle, MQ5_CHANNEL, &chan_cfg);

    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
}