# esp_ota

OTA firmware update over HTTP/HTTPS for the ESP32 family.

## Features

- OTA triggered through public API or optional AT command
- HTTP and HTTPS support via `esp_https_ota`
- Non-blocking background task
- Guard against concurrent OTA attempts
- Auto-restart on success
- Optional internal logs

## Init policy

`esp_ota_init(log_enabled, at_enabled)` controls optional behavior.

- `log_enabled`
  - enables OTA logs
- `at_enabled`
  - registers `AT+OTA` and `AT+OTA?`

## Network prerequisite

WiFi must already be connected before starting OTA.

Recommended companion:
- `rodrigo-s-lange/esp_network`

## Usage

```c
#include "esp_at.h"
#include "esp_ota.h"

nvs_flash_init();
ESP_ERROR_CHECK(esp_at_init(false));
ESP_ERROR_CHECK(esp_network_init());
ESP_ERROR_CHECK(esp_network_start());
ESP_ERROR_CHECK(esp_ota_init(false, true));
```

## Public API

- `esp_ota_init(log_enabled, at_enabled)`
- `esp_ota_deinit()`
- `esp_ota_is_initialized()`
- `esp_ota_is_running()`
- `esp_ota_start(url)`

## AT commands

When `at_enabled=true`:
- `AT+OTA?`
- `AT+OTA`
- `AT+OTA="http://host/firmware.bin"`

## Notes

- Only one OTA session may run at a time.
- `esp_ota_deinit()` refuses to run while OTA is active.
- OTA task stack is `8192` bytes with priority `5`.

## Repository

https://github.com/rodrigo-s-lange/esp-ota
