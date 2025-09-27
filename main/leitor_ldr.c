#include "drivers/gpio.h"
#include "drivers/adc.h"
#include <stdio.h>

void app_main(void){
	//--- 1. CONFIGURACAO DO ADC ---
	// Configura a resolucao do ADC1 para 12 bits (0-4095)
	adc1_config_width(ADC_WIDTH_BIT_12);

	// Configura a atenuacao do canal do ADC1
	// GPIO34 corresponde ao ADC1_CHANNEL_6
	ADC
	adc1_config_channel_atten(ADC1_CHANNEL_6, ADC1_ATTEN_DB_0);

	//--- 2. CONFIGURACAO DO GPIO (LED) ---
	// Configura o GPIO 23 como saída
	gpio_set_direction(GPIO_NUM_23, GPIO_MODE_OUTPUT);

	//--- LOOP PRINCIPAL ---
	while(1){
		// 1. ler o valor bruto do ADC do canal 6 (GPIO34)
		int valod adc = adc1_get_raw(ADC_CHANNEL_6);

		// 2. Imprimir o valor no console (monitor seial)
		printf("Valor do LDR: %d\n", valor_adc);

		// 3. Aguardar 1 segundo antes da proxima leitura
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
