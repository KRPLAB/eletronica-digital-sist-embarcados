#ifndef BUZZER_CONTROL_H
#define BUZZER_CONTROL_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * @brief Inicializa o controle do buzzer (PWM) e cria a tarefa de escuta.
 * * @param queue O handle da fila de onde os alertas de gás devem ser lidos.
 */
void buzzer_control_init(QueueHandle_t queue);

#endif // BUZZER_CONTROL_H