# esp_ota

OTA (Over-The-Air) firmware update over HTTP/HTTPS for the ESP32 family, triggered by AT commands.

## Dependencies

> **Requires [rodrigo-s-lange/esp_at](https://github.com/rodrigo-s-lange/esp-at)**
> `esp_at_init()` must be called before `esp_ota_init()`.

> **Network pre-requisite:** WiFi must be connected before issuing `AT+OTA="url"`.
> Recommended: [rodrigo-s-lange/esp_network](https://github.com/rodrigo-s-lange/esp-network)

## Features

- OTA triggered by a single AT command
- HTTP and HTTPS support (via `esp_https_ota`)
- Non-blocking: runs in a dedicated background task
- Progress and error feedback via `AT_I` / `AT_E`
- Guard against concurrent OTA attempts
- Auto-restart on success

## Targets

`esp32` · `esp32-c3` · `esp32-c6` · `esp32-s3`

## Usage

```c
#include "esp_at.h"
#include "esp_ota.h"

nvs_flash_init();
esp_at_init();
esp_network_init();   // or any WiFi init
esp_network_start();
esp_ota_init();
```

## AT Commands

| Command | Effect |
|---|---|
| `AT+OTA?` | Show firmware version, IDF version, active partition |
| `AT+OTA` | Same as `AT+OTA?` |
| `AT+OTA="http://192.168.1.10/firmware.bin"` | Start OTA from URL; reboots on success |

## Partition table

OTA requires a partition table with at least two OTA partitions.
Add to your project's `partitions.csv` or use the IDF default OTA table:

```
# sdkconfig
CONFIG_PARTITION_TABLE_TWO_OTA=y
```

## Install

```bash
idf.py add-dependency "rodrigo-s-lange/esp_ota>=0.1.0"
idf.py add-dependency "rodrigo-s-lange/esp_at>=0.1.0"
```

Or as git submodules:

```bash
git submodule add https://github.com/rodrigo-s-lange/esp-at.git  components/esp_at
git submodule add https://github.com/rodrigo-s-lange/esp-ota.git components/esp_ota
```

## License

MIT — © 2026 Rodrigo S. Lange
