#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

// --- Bits do "Quadro de Luzes" (EventGroup) ---
// Necessário definir estes bits para que app_main e wifi_manager estejam com a mesma definição
#define WIFI_STA_CONNECTED_BIT      BIT0    // O Wi-Fi conectou (tem sinal)
#define WIFI_STA_IPV4_OBTAINED_BIT  BIT1    // O Wi-Fi obteve um IP (está na rede)
#define WIFI_STA_IPV6_OBTAINED_BIT  BIT2    // (Para redes IPv6)

/**
 * @brief Inicializa e inicia o Wi-Fi em modo Estação (STA).
 *
 * @param event_group Handle do EventGroup (criado no app_main) que será usado
 * para sinalizar o status da conexão (ex: IP obtido).
 * @return esp_err_t ESP_OK em sucesso, ou um código de erro.
 */
esp_err_t wifi_sta_init(EventGroupHandle_t event_group);

/**
 * @brief Para e desliga o Wi-Fi em modo Estação.
 *
 * @return esp_err_t ESP_OK em sucesso, ou um código de erro.
 */
esp_err_t wifi_sta_stop(void);

/**
 * @brief Tenta se reconectar ao AP parando e reiniciando o Wi-Fi.
 *
 * @return esp_err_t ESP_OK em sucesso, ou um código de erro.
 */
esp_err_t wifi_sta_reconnect(void);

#endif // WIFI_MANAGER_H