#include "esp_ota.h"
#include "esp_at.h"
#include "esp_https_ota.h"
#include "esp_app_desc.h"
#include "esp_ota_ops.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h>
#include <stdlib.h>

//static const char *TAG = "esp_ota";

static bool           s_initialized = false;
static volatile bool  s_ota_running = false;

/* ======================================================================== */
/* Helpers                                                                   */
/* ======================================================================== */

static void _print_info(void)
{
    const esp_app_desc_t  *desc    = esp_app_get_description();
    const esp_partition_t *running = esp_ota_get_running_partition();

    AT(G "Firmware : " W "%s", desc->version);
    AT(G "IDF      : " W "%s", desc->idf_ver);
    AT(G "Partition: " W "%s", running->label);
}

/* ======================================================================== */
/* Task OTA                                                                  */
/* ======================================================================== */

static void _ota_task(void *arg)
{
    char *url = (char *)arg;

    AT_I(Y "OTA: iniciando — " W "%s", url);

    esp_http_client_config_t http_cfg = {
        .url                        = url,
        .skip_cert_common_name_check = true,
        .keep_alive_enable           = true,
    };

    esp_https_ota_config_t ota_cfg = {
        .http_config = &http_cfg,
    };

    esp_err_t err = esp_https_ota(&ota_cfg);

    free(url);
    s_ota_running = false;

    if (err == ESP_OK) {
        AT_I(G "OTA: concluído — reiniciando...");
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
    } else {
        AT_E("OTA: falhou — %s", esp_err_to_name(err));
    }

    vTaskDelete(NULL);
}

/* ======================================================================== */
/* Handlers AT                                                               */
/* ======================================================================== */

static void _handle_ota_query(const char *param)
{
    _print_info();
}

static void _handle_ota(const char *param)
{
    if (param == NULL || param[0] == '\0') {
        _print_info();
        return;
    }

    if (s_ota_running) {
        AT_W("OTA: atualização já em andamento");
        return;
    }

    char *url = strdup(param);
    if (!url) {
        AT_E("OTA: sem memória");
        return;
    }

    s_ota_running = true;

    if (xTaskCreate(_ota_task, "ota_task", 8192, url, 5, NULL) != pdPASS) {
        AT_E("OTA: falha ao criar task");
        free(url);
        s_ota_running = false;
    }
}

/* ======================================================================== */
/* API pública                                                               */
/* ======================================================================== */

esp_err_t esp_ota_init(void)
{
    if (s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    esp_at_register_cmd("AT+OTA?", _handle_ota_query);
    esp_at_register_cmd("AT+OTA",  _handle_ota);

    s_initialized = true;
    AT_I(G "esp_ota iniciado");
    return ESP_OK;
}
