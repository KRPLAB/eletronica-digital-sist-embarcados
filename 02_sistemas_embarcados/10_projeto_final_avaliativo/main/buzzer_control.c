#include <stdint.h>
#include "buzzer_control.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// --- Configurações do Buzzer ---
#define BUZZ_PIN 19
#define BUZZ_LEDC_TIMER LEDC_TIMER_0
#define BUZZ_LEDC_CHANNEL LEDC_CHANNEL_0
#define BUZZ_LEDC_MODE LEDC_LOW_SPEED_MODE
#define BUZZ_FREQ_HZ 1000               
#define BUZZ_DUTY_RES LEDC_TIMER_10_BIT
#define BUZZ_DUTY_50_PERCENT 512

static const char *TAG = "BUZZER_CONTROL";

// Handle da fila (será recebido pelo init)
static QueueHandle_t local_alert_queue;

/**
 * @brief Função auxiliar para ligar/desligar o PWM do buzzer
 */
static void set_buzzer_state(bool on)
{
    if (on) {
        ledc_set_duty(BUZZ_LEDC_MODE, BUZZ_LEDC_CHANNEL, BUZZ_DUTY_50_PERCENT);
        ledc_update_duty(BUZZ_LEDC_MODE, BUZZ_LEDC_CHANNEL);
    }
    else {
        // Desliga o buzzer
        ledc_set_duty(BUZZ_LEDC_MODE, BUZZ_LEDC_CHANNEL, 0);
        ledc_update_duty(BUZZ_LEDC_MODE, BUZZ_LEDC_CHANNEL);
    }
}

/**
 * @brief Tarefa (Task) que espera por alertas na fila.
 *
 * Esta é a Task B. Ela passa 99.9% do tempo "bloqueada" (dormindo),
 * esperando por uma mensagem na fila.
 */
static void buzzer_task(void *pvParameters)
{
    ESP_LOGI(TAG, "Tarefa do buzzer iniciada. Aguardando alertas...");

    int alert_state;

    while (1) {
        // 1. Esperar por uma mensagem
        // xQueueReceive(handle_da_fila, ponteiro_para_onde_salvar, tempo_de_espera)
        // portMAX_DELAY significa: "durma para sempre até uma mensagem chegar"
        if (xQueueReceive(local_alert_queue, &alert_state, portMAX_DELAY)) {
            ESP_LOGI(TAG, "Alerta recebido da fila: %d", alert_state);

            if (alert_state == 1) {
                ESP_LOGW(TAG, "Ativando buzzer.");
                set_buzzer_state(true);
            } else {
                ESP_LOGI(TAG, "Situação segura. Desativando buzzer.");
                set_buzzer_state(false);
            }
        }
    }
}

/**
 * @brief Função de inicialização do módulo.
 */
void buzzer_control_init(QueueHandle_t queue)
{
    local_alert_queue = queue;

    // Configura o Timer do LEDC
    ledc_timer_config_t timer = {
        .speed_mode = BUZZ_LEDC_MODE,
        .timer_num = BUZZ_LEDC_TIMER,
        .duty_resolution = BUZZ_DUTY_RES,
        .freq_hz = BUZZ_FREQ_HZ,
        .clk_cfg = LEDC_AUTO_CLK};
    ledc_timer_config(&timer);

    // Configura o Canal do LEDC
    ledc_channel_config_t channel = {
        .gpio_num = BUZZ_PIN,
        .speed_mode = BUZZ_LEDC_MODE,
        .channel = BUZZ_LEDC_CHANNEL,
        .timer_sel = BUZZ_LEDC_TIMER,
        .duty = 0};
    ledc_channel_config(&channel);

    // Cria a Tarefa B
    xTaskCreate(buzzer_task, "buzzer_task", 2048, NULL, 5, NULL);
}