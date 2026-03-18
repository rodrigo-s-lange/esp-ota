#pragma once

#include <stdbool.h>

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize OTA component.
 *
 * @param log_enabled Enable internal component logs.
 * @param at_enabled  Register AT+OTA commands.
 */
esp_err_t esp_ota_init(bool log_enabled, bool at_enabled);

/**
 * @brief Deinitialize OTA component.
 *
 * Returns ESP_ERR_INVALID_STATE if an OTA session is still running.
 */
esp_err_t esp_ota_deinit(void);

/**
 * @brief Returns whether the component is initialized.
 */
bool esp_ota_is_initialized(void);

/**
 * @brief Returns whether an OTA session is currently running.
 */
bool esp_ota_is_running(void);

/**
 * @brief Start OTA update from URL.
 *
 * Runs in a dedicated background task and returns immediately.
 */
esp_err_t esp_ota_start(const char *url);

#ifdef __cplusplus
}
#endif
