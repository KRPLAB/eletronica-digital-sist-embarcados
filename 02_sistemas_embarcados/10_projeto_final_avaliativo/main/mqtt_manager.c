#include "mqtt_manager.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "mqtt_client.h"

#include "credentials.h"

static const char *TAG = "MQTT_MANAGER";

// Handle global para o cliente MQTT
esp_mqtt_client_handle_t client = NULL;

// Buffer global para armazenar o UUID recebido e Semáforo
char device_uuid[64] = {0};
SemaphoreHandle_t sem_uuid_received = NULL;

void mqtt_subscribe(const char *topic) {
    if (client) {
        esp_mqtt_client_subscribe(client, topic, 1);
        ESP_LOGI(TAG, "Inscrito no tópico: %s", topic);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    // Log de debug mostrando a base e o ID do evento; PRIi32 garante formatação correta para int32_t
    ESP_LOGD(TAG, "Evento despachado do loop base=%s, event_id=%" PRIi32, base, event_id);

    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
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
        ESP_LOGI(TAG, "Dados recebidos.");
        if (strstr(event->topic, "dispositivos/registro/")) {
            // Copia o payload (UUID) para o buffer global
            int copy_len = event->data_len;
            if (copy_len >= sizeof(device_uuid)) copy_len = sizeof(device_uuid) - 1;
            
            memcpy(device_uuid, event->data, copy_len);
            device_uuid[copy_len] = '\0';
            
            ESP_LOGI(TAG, "UUID Recebido do Backend: %s", device_uuid);
            
            // Libera o semáforo para a main prosseguir
            if (sem_uuid_received) {
                xSemaphoreGive(sem_uuid_received);
            }
        }
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
    sem_uuid_received = xSemaphoreCreateBinary();
    if (sem_uuid_received == NULL) {
        ESP_LOGE(TAG, "Falha ao criar semáforo para UUID!");
        return;
    }
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