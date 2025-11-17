#include <stdint.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_wifi_netif.h"
#include "wifi_manager.h"

// Tag for debugging messages
static const char *TAG = "WIFI_MANAGER";

// Static global variables that may be used in future implementations
static esp_netif_t *s_wifi_netif = NULL;
static EventGroupHandle_t s_wifi_event_group = NULL;
static wifi_netif_driver_t s_wifi_driver = NULL;

/************************************************************************
 * Private functions prototypes
 ************************************************************************/

static void on_wifi_event(void *arg, 
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void *event_data);

static void on_ip_event(void *arg,
                        esp_event_base_t event_base,
                        int32_t event_id,
                        void *event_data);

static void wifi_start(void *esp_netif,
                       esp_event_base_t event_base,
                       int32_t event_id,
                       void *event_data);

/************************************************************************
 * Private functions definitions
 ************************************************************************/

static void on_wifi_event(void *arg, 
                          esp_event_base_t event_base,
                          int32_t event_id,
                          void *event_data) {
    switch (event_id){
        case WIFI_EVENT_STA_START:
            if (s_wifi_netif != NULL) {
                wifi_start(s_wifi_netif, event_base, event_id, event_data);
            }
            break;

        case WIFI_EVENT_STA_STOP:
            if (s_wifi_netif != NULL) {
                esp_netif_action_stop(s_wifi_netif, event_base, event_id, event_data);
            }
            break;
        
        case WIFI_EVENT_STA_CONNECTED:
            // Make suere we have a valid interface handle
            if (s_wifi_netif == NULL) {
                ESP_LOGE(TAG, "WiFi not initialized. Interface handle is NULL.");
                return;
            }

            // Print AP information
            wifi_event_sta_connected_t *event = (wifi_event_sta_connected_t *)event_data;
            ESP_LOGI(TAG, "Connected to AP:");
            ESP_LOGI(TAG, " SSID: %.*s", (char*)event->ssid_len, event->ssid);
            ESP_LOGI(TAG, " Channel: %d", event->channel);
            ESP_LOGI(TAG, " Authmode: %d", event->authmode);
            ESP_LOGI(TAG, " AID: %d", event->aid);

            // (s2.4) Register interface received callback
            wifi_netif_driver_t driver = esp_netif_get_io_driver(s_wifi_netif);
            if (!esp_wifi_is_if_ready_when_started(driver)) {
                esp_err_t esp_ret = esp_wifi_register_if_rxcb(driver, esp_netif_receive, s_wifi_netif);

                if (esp_ret != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to register interface RX callback: %s", esp_err_to_name(esp_ret));
                    return;
                }
            }

            // (s4.2) Set up the WiFi interface and start DHCP process
            esp_netif_action_connected(s_wifi_netif, event_base, event_id, event_data);
            

            // Set WiFi connected bit
            xEventGroupSetBits(s_wifi_event_group, WIFI_STA_CONNECTED_BIT);
#if CONFIG_WIFI_STA_CONNECTED_IPV6 || CONFIG_WIFI_STA_CONNECTED_UNSPECIFIED
            // Request IPv6 link-local address for the interface
            esp_ret = esp_netif_create_ip6_linklocal(s_wifi_netif);
            if (esp_ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to create IPv6 link-local address: %s", esp_err_to_name(esp_ret));
            }
#endif
            break;
        case WIFI_EVENT_STA_DISCONNECTED:
            if (s_wifi_netif != NULL) {
                esp_netif_action_disconnected(s_wifi_netif, event_base, event_id, event_data);
            }
            // Clear WiFi connected bit
            xEventGroupClearBits(s_wifi_event_group, WIFI_STA_CONNECTED_BIT);
#if CONFIG_WIFI_STA_AUTO_RECONNECT
            ESP_LOGI(TAG, "Reconnecting to the AP...");
            wifi_sta_reconnect();
#endif
            break;

    }
}

static void on_ip_event(void *arg,
                        esp_event_base_t event_base,
                        int32_t event_id,
                        void *event_data) {
    esp_err_t esp_ret;

    switch (event_id) {
#if CONFIG_WIFI_STA_CONNECTED_IPV4 || CONFIG_WIFI_STA_CONNECTED_UNSPECIFIED
        case IP_EVENT_STA_GOT_IP:
            // Make suere we have a valid interface handle
            if (s_wifi_netif == NULL) {
                ESP_LOGE(TAG, "On obtain IPV4 addr: Interface handle is NULL.");
                return;       
            }
            
            // Notify the interface about the obtained IP address
            esp_ret = esp_wifi_internal_set_sta_ip(s_wifi_netif, event_base, event_id, event_data);
            if (esp_ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to set STA IP: %s", esp_err_to_name(esp_ret));
            }

            // Set connected bit
            xEventGroupSetBits(s_wifi_event_group, WIFI_STA_IPV4_OBTAINED_BIT)

            // Log the obtained IP address
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
            esp_netif_ip_info_t ip_info = event->ip_info;
            ESP_LOGI(TAG, "Wifi IPv4 address obtained:");
            ESP_LOGI(TAG, " IP: " IPSTR, IP2STR(&ip_info.ip));
            ESP_LOGI(TAG, " Netmask: " IPSTR, IP2STR(&ip_info.netmask));
            ESP_LOGI(TAG, " Gateway: " IPSTR, IP2STR(&ip_info.gw));
            break;
#endif
    }
}

void wifi_manager_init(void) {
    ESP_LOGI(TAG, "Inicializado");
    // ... Implementação futura do gerenciamento de Wi-Fi ...
}