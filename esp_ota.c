#include <stdlib.h>
#include <string.h>

#include "esp_app_desc.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_ota.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_at.h"

static const char *TAG = "esp_ota";

static bool s_initialized = false;
static volatile bool s_ota_running = false;
static bool s_log_enabled = false;
static bool s_at_enabled = false;
static bool s_at_registered = false;

#define OTA_LOGI(...) do { if (s_log_enabled) ESP_LOGI(TAG, __VA_ARGS__); } while (0)
#define OTA_LOGW(...) do { if (s_log_enabled) ESP_LOGW(TAG, __VA_ARGS__); } while (0)
#define OTA_LOGE(...) do { if (s_log_enabled) ESP_LOGE(TAG, __VA_ARGS__); } while (0)

static void print_info(void)
{
    const esp_app_desc_t *desc = esp_app_get_description();
    const esp_partition_t *running = esp_ota_get_running_partition();

    AT(G "Firmware : " W "%s", desc->version);
    AT(G "IDF      : " W "%s", desc->idf_ver);
    AT(G "Partition: " W "%s", running->label);
}

static void ota_task(void *arg)
{
    char *url = (char *)arg;

    AT_I(Y "OTA: iniciando - " W "%s", url);
    OTA_LOGI("starting OTA from %s", url);

    esp_http_client_config_t http_cfg = {
        .url = url,
        .skip_cert_common_name_check = true,
        .keep_alive_enable = true,
    };

    esp_https_ota_config_t ota_cfg = {
        .http_config = &http_cfg,
    };

    esp_err_t err = esp_https_ota(&ota_cfg);

    free(url);
    s_ota_running = false;

    if (err == ESP_OK) {
        AT_I(G "OTA: concluido - reiniciando...");
        OTA_LOGI("OTA completed successfully");
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
    } else {
        AT_E("OTA: falhou - %s", esp_err_to_name(err));
        OTA_LOGE("OTA failed: %s", esp_err_to_name(err));
    }

    vTaskDelete(NULL);
}

static void handle_ota_query(const char *param)
{
    (void)param;
    print_info();
}

static void handle_ota(const char *param)
{
    if (param == NULL || param[0] == '\0') {
        print_info();
        return;
    }

    esp_err_t err = esp_ota_start(param);
    if (err != ESP_OK) {
        AT_E("OTA: falha - %s", esp_err_to_name(err));
    }
}

static esp_err_t register_at_commands(void)
{
    if (s_at_registered) return ESP_OK;

    esp_err_t err = esp_at_register_cmd_example("AT+OTA?", handle_ota_query, "AT+OTA?");
    if (err != ESP_OK) return err;

    err = esp_at_register_cmd_example("AT+OTA", handle_ota, "AT+OTA=\"http://192.168.1.10/firmware.bin\"");
    if (err != ESP_OK) return err;
    err = esp_at_set_help_visible("AT+OTA", false);
    if (err != ESP_OK) return err;

    s_at_registered = true;
    return ESP_OK;
}

static void unregister_at_commands(void)
{
    if (!s_at_registered) return;

    (void)esp_at_unregister_cmd("AT+OTA?");
    (void)esp_at_unregister_cmd("AT+OTA");
    s_at_registered = false;
}

esp_err_t esp_ota_init(bool log_enabled, bool at_enabled)
{
    if (s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    s_log_enabled = log_enabled;
    s_at_enabled = at_enabled;

    if (s_at_enabled) {
        esp_err_t err = register_at_commands();
        if (err != ESP_OK) {
            s_log_enabled = false;
            s_at_enabled = false;
            return err;
        }
    }

    s_initialized = true;
    OTA_LOGI("initialized (AT=%s)", s_at_enabled ? "on" : "off");
    return ESP_OK;
}

esp_err_t esp_ota_deinit(void)
{
    if (!s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    if (s_ota_running) {
        return ESP_ERR_INVALID_STATE;
    }

    OTA_LOGI("deinitialized");
    unregister_at_commands();
    s_initialized = false;
    s_log_enabled = false;
    s_at_enabled = false;
    return ESP_OK;
}

bool esp_ota_is_initialized(void)
{
    return s_initialized;
}

bool esp_ota_is_running(void)
{
    return s_ota_running;
}

esp_err_t esp_ota_start(const char *url)
{
    if (!s_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    if (url == NULL || url[0] == '\0') {
        return ESP_ERR_INVALID_ARG;
    }
    if (s_ota_running) {
        OTA_LOGW("OTA already running");
        return ESP_ERR_INVALID_STATE;
    }

    char *url_copy = strdup(url);
    if (url_copy == NULL) {
        return ESP_ERR_NO_MEM;
    }

    s_ota_running = true;
    if (xTaskCreate(ota_task, "ota_task", 8192, url_copy, 5, NULL) != pdPASS) {
        free(url_copy);
        s_ota_running = false;
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}
