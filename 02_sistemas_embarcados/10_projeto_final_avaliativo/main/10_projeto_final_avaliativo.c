#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <driver/ledc.h>
#include "esp_log.h"

#define BUZZ_PIN 19
#define MQ5_CHANNEL ADC1_CHANNEL_6
#define GAS_THRESHOLD 2000

static const char *TAG = "GAS_SENSOR";

void beep_pwm(void) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 512);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    vTaskDelay(pdMS_TO_TICKS(500));
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 0);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

void app_main(void) {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(MQ5_CHANNEL, ADC_ATTEN_DB_11);

    // PWM
    ledc_timer_config_t timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .duty_resolution = LEDC_TIMER_10_BIT,
        .freq_hz = 1000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timer);

    ledc_channel_config_t channel = {
        .gpio_num = BUZZ_PIN,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0
    };
    ledc_channel_config(&channel);

    ESP_LOGI(TAG, "Sistema iniciado.");

    while (1) {
        int val = adc1_get_raw(MQ5_CHANNEL);
        ESP_LOGI(TAG, "Leitura: %d", val);

        if (val > GAS_THRESHOLD)
            beep_pwm();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
