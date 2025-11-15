#include <stdint.h>
#include "mq5_sensor.h"
#include "driver/adc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// --- Configurações do Sensor ---
#define MQ5_CHANNEL     ADC1_CHANNEL_6  // GPIO 34
#define GAS_THRESHOLD   2000            // Limiar de perigo (a ser ajustado conforme necessário melhor pesquisado a respeito do sensor MQ-5)
#define READ_INTERVAL_MS 1000

static const char *TAG = "MQ5_SENSOR";

static QueueHandle_t local_alert_queue;

/**
 * @brief Tarefa (Task) que lê o sensor em loop.
 * * Esta é a Task A. Ela passa a maior parte do tempo em "vTaskDelay" (dormindo).
 */
static void sensor_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Tarefa do sensor iniciada.");
    
    // Deixar o sensor aquecer por um tempo antes de começar as leituras
    vTaskDelay(pdMS_TO_TICKS(15000));

    int current_alert_state = 0;
    int new_alert_state = 0;

    while (1) {
        int val_raw = adc1_get_raw(MQ5_CHANNEL);

        if (val_raw > GAS_THRESHOLD) {
            new_alert_state = 1;
            ESP_LOGW(TAG, "Quantidade de gás acima do limite detectado! Valor ADC: %d", val_raw);
            ESP_LOGW(TAG, "Ativando buzzer e enviando mensagem MQTT.");
        } else {
            new_alert_state = 0;
        }

        if (new_alert_state != current_alert_state) {
            current_alert_state = new_alert_state;
            ESP_LOGI(TAG, "Estado mudou para: %d. Enviando para fila.", current_alert_state);
            
            // xQueueSend(handle_da_fila, ponteiro_para_o_dado, tempo_de_espera)
            // Tenta enviar o novo estado para a fila.
            if (xQueueSend(local_alert_queue, &current_alert_state, pdMS_TO_TICKS(100)) != pdTRUE) {
                ESP_LOGE(TAG, "Falha ao enviar para a fila!");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(READ_INTERVAL_MS));
    }
}

/**
 * @brief Função de inicialização do módulo.
 */
void mq5_sensor_init(QueueHandle_t queue)
{
    local_alert_queue = queue;

    // Configura o ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(MQ5_CHANNEL, ADC_ATTEN_DB_11);

    // Cria a Tarefa A
    // xTaskCreate(função_da_tarefa, "nome_tarefa", stack_size, parametros, prioridade, handle_tarefa)
    xTaskCreate(sensor_task, "sensor_task", 2048, NULL, 5, NULL);
}