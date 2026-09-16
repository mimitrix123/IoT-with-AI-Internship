idf_component_register(SRCS "main.c" INCLUDE_DIRS "." REQUIRES esp_wifi esp_event nvs_flash mqtt driver esp_http_client app_update esp_https_ota)
