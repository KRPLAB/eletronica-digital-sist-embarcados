#ifndef MQ5_SENSOR_H
#define MQ5_SENSOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

/**
 * @brief Inicializa o sensor MQ-5 e cria a tarefa de leitura.
 * * @param queue O handle (função que se encarrega de processar o conteúdo) da fila para onde os alertas de gás devem ser enviados.
 */
void mq5_sensor_init(QueueHandle_t queue_buzzer, QueueHandle_t queue_mqtt);

#endif // MQ5_SENSOR_H