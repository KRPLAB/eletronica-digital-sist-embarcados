#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "esp_err.h"

/**
 * @brief Inicializa o cliente MQTT.
 * Configura a URI do broker e registra os callbacks de eventos
 */
void mqtt_manager_init(void);

/**
 * @brief Publica uma mensagem em um tópico específico.
 * * @param topic O tópico onde a mensagem será publicada (ex: "casa/cozinha/gas")
 * @param message A string de dados a ser enviada.
 * @return int ID da mensagem ou -1 em caso de falha.
 */
int mqtt_manager_publish(const char *topic, const char *message);

#endif // MQTT_MANAGER_H