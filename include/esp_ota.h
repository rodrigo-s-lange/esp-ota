/**
 * @file    esp_ota.h
 * @brief   Atualização OTA via HTTP/HTTPS com controle por comandos AT.
 *
 * Este componente realiza OTA (Over-The-Air) a partir de uma URL HTTP/HTTPS,
 * executando o download e a gravação em background (task dedicada) sem bloquear
 * a at_task nem o event loop.
 *
 * @par Dependência obrigatória
 *   Requer o componente **rodrigo-s-lange/esp_at**.
 *   esp_at_init() DEVE ser chamado antes de esp_ota_init().
 *   Repositório: https://github.com/rodrigo-s-lange/esp-at
 *
 * @par Pré-requisito de rede
 *   A interface WiFi deve estar conectada antes de chamar AT+OTA="url".
 *   Use rodrigo-s-lange/esp_network ou equivalente.
 *
 * Fluxo de uso:
 * @code
 *   nvs_flash_init();
 *   esp_at_init();
 *   esp_network_init();
 *   esp_network_start();
 *   esp_ota_init();
 * @endcode
 *
 * Comandos AT registrados por esp_ota_init():
 * @code
 *   AT+OTA?              → versão do firmware, IDF e partição ativa
 *   AT+OTA               → mesmo que AT+OTA?
 *   AT+OTA="http://..."  → inicia OTA pela URL; reinicia ao concluir
 * @endcode
 *
 * @note A task OTA usa 8 KB de stack e prioridade 5.
 *       Apenas uma atualização simultânea é permitida.
 *
 * @par Registry
 *   rodrigo-s-lange/esp_ota
 */

#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Inicializa o componente OTA.
 *
 * Registra os comandos AT+OTA? e AT+OTA via esp_at_register_cmd().
 * Deve ser chamada após esp_at_init().
 *
 * @return ESP_OK                Inicialização bem-sucedida.
 * @return ESP_ERR_INVALID_STATE Já inicializado.
 */
esp_err_t esp_ota_init(void);

#ifdef __cplusplus
}
#endif
