#include "mqtt_manager.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "credentials.h"

static const char *TAG = "MQTT_MANAGER";

// Handle global para o cliente MQTT, para que possamos publicar de qualquer lugar
esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // Log de debug mostrando a base e o ID do evento; PRIi32 garante formatação correta para int32_t
    ESP_LOGD(TAG, "Evento despachado do loop base=%s, event_id=%" PRIi32, base, event_id);

    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // Exemplo: Ao conectar, podemos nos inscrever em um topico de comando
        // esp_mqtt_client_subscribe(client, "sistema/comandos", 0);
        break;
        
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGE(TAG, "Erro de transporte TCP/IP: %s", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
        
    default:
        ESP_LOGI(TAG, "Outro evento id:%d", event->event_id);
        break;
    }
}

void mqtt_manager_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URI,
        //.credentials.username = "",
        //.credentials.authentication.password = "",
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, client);
    
    esp_mqtt_client_start(client);
}

int mqtt_manager_publish(const char *topic, const char *message)
{
    if (client == NULL) {
        ESP_LOGE(TAG, "Cliente MQTT nao inicializado!");
        return -1;
    }
    
    // Publica mensagem. 
    // len=0 (calcula strlen auto), qos=1 (garante entrega pelo menos uma vez), retain=0
    int msg_id = esp_mqtt_client_publish(client, topic, message, 0, 1, 0);
    ESP_LOGI(TAG, "Mensagem enviada para %s, ID: %d", topic, msg_id);
    return msg_id;
}