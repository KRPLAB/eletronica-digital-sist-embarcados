#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"

/**
 * @brief Event group for Wi-Fi events
 */
#define WIFI_STA_CONNECTED_BIT      BIT0
#define WIFI_STA_IPV4_OBTAINED_BIT  BIT1
#define WIFI_STA_IPV6_OBTAINED_BIT  BIT2

/**
 * @brief Initialize the Wi-Fi manager in station mode.
 * 
 * Set up the Wi-Fi interface and connect to a Wi-Fi network. Is possible uses
 * event groups to wait for a connection and IP address assignment.
 * 
 * Important: This function must be called after calling esp_netif_init() and
 * esp_event_loop_create_default().
 * 
 * @param[in] event_group Event group handle to WiFi and IP events. Can be NULL.
 * 
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err wifi_sta_init(EventGroupHandle_t event_group);

/**
 * @brief Disable WiFi
 * @return
 *      - ESP_OK: WiFi disabled successfully
 *      - ESP_ERR_WIFI_NOT_INIT: WiFi driver was not initialized
 */
esp_err wifi_sta_stop(void);

/**
 * @brief Attempt to reconnect to the Wi-Fi network.
 * 
 * @return 
 * - ESP_OK: Reconnection initiated successfully
 * - Other error codes from esp_wifi_connect()
 */
esp_err wifi_sta_reconnect(void);



void wifi_manager_init(void);
#endif // WIFI_MANAGER_H