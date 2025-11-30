idf_component_register(SRCS "10_projeto_final_avaliativo.c"
                           "mq5_sensor.c"
                           "buzzer_control.c"
                           "wifi_manager.c"
                           "mqtt_manager.c"
                   INCLUDE_DIRS "."
                   REQUIRES "mqtt" "esp_adc" "esp_netif" "esp_wifi" "driver" "nvs_flash" "cjson")