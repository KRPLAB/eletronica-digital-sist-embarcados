#include "drivers/gpio.h"
#include "drivers/adc.h"
#include <stdio.h>

void app_main(void)
{
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


	while(1);
}
