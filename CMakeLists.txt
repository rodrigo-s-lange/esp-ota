idf_component_register(SRCS "esp_ota.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_at esp_https_ota app_update esp_http_client
                             esp_system freertos)
