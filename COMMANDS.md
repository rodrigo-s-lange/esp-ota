# esp_ota - Commands

When `esp_ota_init(log_enabled, true)` is used, the component registers these AT commands.

## Info

`AT+OTA?`
- prints firmware version, IDF version and active partition

`AT+OTA`
- same as `AT+OTA?`

## Start update

`AT+OTA="http://host/firmware.bin"`
- starts OTA from URL
- runs in background
- restarts automatically on success

## Example

```text
AT+OTA?
AT+OTA="http://192.168.1.10/firmware.bin"
```

## Validation

```bash
cmd /c "call C:\esp\release-v6.0\esp-idf\export.bat && idf.py build"
```
