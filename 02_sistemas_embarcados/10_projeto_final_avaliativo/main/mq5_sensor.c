#include <stdint.h>
#include "mq5_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"

#define MQ5_CHANNEL     ADC_CHANNEL_6
#define READ_INTERVAL_MS 1000

static const char *TAG = "MQ5_SENSOR";

static QueueHandle_t queue_handle_buzzer;
static QueueHandle_t queue_handle_mqtt;

static adc_oneshot_unit_handle_t adc_handle;

static void sensor_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Tarefa do sensor iniciada (Aquecimento 15s)...");
    vTaskDelay(pdMS_TO_TICKS(15000));

    int current_alert_state = -1;
    int new_alert_state = 0;
    int val_raw = 0;

    while (1) {
        if (adc_oneshot_read(adc_handle, MQ5_CHANNEL, &val_raw) != ESP_OK) {
            ESP_LOGE(TAG, "Erro leitura ADC");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        if (val_raw > GAS_THRESHOLD) {
            new_alert_state = 1;
        } else {
            new_alert_state = 0;
        }

        if (new_alert_state != current_alert_state) {
            current_alert_state = new_alert_state;
            // Envia para o buzzer apenas se mudou (0 ou 1)
            xQueueSend(queue_handle_buzzer, &current_alert_state, 0);
            ESP_LOGI(TAG, "Buzzer state alterado: %d (ADC: %d)", current_alert_state, val_raw);
        }

        // 2. Lógica do MQTT (Envio Contínuo para Média)
        if (xQueueSend(queue_handle_mqtt, &val_raw, 0) != pdTRUE) {
            ESP_LOGD(TAG, "Fila MQTT cheia, descartando amostra.");
        }

        vTaskDelay(pdMS_TO_TICKS(READ_INTERVAL_MS));
    }
}

void mq5_sensor_init(QueueHandle_t q_buzz, QueueHandle_t q_mqtt) {
    queue_handle_buzzer = q_buzz;
    queue_handle_mqtt = q_mqtt;

    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = ADC_UNIT_1,
    };
    
    adc_oneshot_new_unit(&init_cfg, &adc_handle);

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_oneshot_config_channel(adc_handle, MQ5_CHANNEL, &chan_cfg);

    xTaskCreate(sensor_task, "sensor_task", 4096, NULL, 5, NULL);
}