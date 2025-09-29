#include <stdio.h>
#include "driver/adc.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

// Funcao para salvar um valor inteiro no NVS
esp_err_t salvar_valor_nvs(const char* chave, int32_t valor) {
	nvs_handle_t my_handle;
	esp_err_t err;

	// 1. Abre o NVS no namespace "storage"
	err = nvs_open("storage", NVS_READWRITE, &my_handle);
	if (err != ESP_OK) {
		printf("Erro (%s) ao abrir o NVS!\n", esp_err_to_name(err));
		return err;
	}

	// 2. Escreve o valor na chave especificada
	err = nvs_set_i32(my_handle, chave, valor);
	if (err != ESP_OK) {
		printf("Erro (%s) ao escrever no NVS!\n", esp_err_to_name(err));
		nvs_close(my_handle);
		return err;
	}

	// 3. Aplica as mudanças (salva permanentemente)
	err = nvs_commit(my_handle);
	if (err != ESP_OK) {
		printf("Erro (%s) ao comitar no NVS!\n", esp_err_to_name(err));
		nvs_close(my_handle);
		return err;
	}

	// 4. Fecha o NVS
	nvs_close(my_handle);
	return ESP_OK;
}

// Funcao para ler um valor inteiro do NVS
esp_err_t ler_valor_nvs(const char* chave, int32_t* valor_lido) {
	nvs_handle_t my_handle;
	esp_err_t err;

	err = nvs_open("storage", NVS_READONLY, &my_handle);
	if (err != ESP_OK) {
		printf("Erro (%s) ao abrir NVS para leitura!\n", esp_err_to_name(err));
		return err;
	}

	// Tenta ler o valor da chave.
	// Se a chave nao existir, a funcao retornara ESP_ERR_NVS_NOT_FOUND.
	err = nvs_get_i32(my_handle, chave, valor_lido);

	nvs_close(my_handle);
	return err;
}

void app_main(void) {
    //--- 0. INICIALIZACAO DO NVS ---
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // Se o NVS estiver corrompido ou em uma versao antiga, apaga e reinicia
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    // Verifica se houve algum erro fatal na inicializacao.
    ESP_ERROR_CHECK(ret);

    printf("NVS inicializado com sucesso!\n");

    //--- 1. CONFIGURACAO DO ADC ---
    // Configura a resolucao do ADC1 para 12 bits (0-4095)
    adc1_config_width(ADC_WIDTH_BIT_12);

    // Configura a atenuacao do canal do ADC1
    // GPIO35 corresponde ao ADC1_CHANNEL_7
    adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_0);

    //--- 2. CONFIGURACAO DO GPIO (LED) ---
    // Configura o GPIO 23 como saída
    gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT);

    //--- LOOP PRINCIPAL ---
    while (1) {
        // 1. ler o valor bruto do ADC do canal 7 (GPIO35)
        int valor_adc = adc1_get_raw(ADC1_CHANNEL_7);

        // 2. Imprimir o valor no console (monitor seial)
        printf("Valor do LDR: %d\n", valor_adc);

        // 3. Aguardar 1 segundo antes da proxima leitura
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        // 4. Controlar o LED com base no valor lido

        // Se for maior que o limiar (mais luz, menos resistencia), desliga o
        // LED O valor 1500 eh um limiar inicial que ajustarei em brve

        if (valor_adc > 1500)
            gpio_set_level(GPIO_NUM_19, 0);
        else
            // Se for menor (menos luz, mais resistencia), liga o LED
            gpio_set_level(GPIO_NUM_19, 1);
    }
}
