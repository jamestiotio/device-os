#pragma once

#include "hal_platform_nrf52840_config.h"
#include "platforms.h"

#define HAL_PLATFORM_NCP (1)
#define HAL_PLATFORM_NCP_AT (1)
#define HAL_PLATFORM_NCP_UPDATABLE (1)
#define HAL_PLATFORM_WIFI (1)
#define HAL_PLATFORM_WIFI_SERIAL (HAL_USART_SERIAL2)
#define HAL_PLATFORM_SPI_NUM (2)
#define HAL_PLATFORM_I2C_NUM (2)
#define HAL_PLATFORM_USART_NUM (2)
#define HAL_PLATFORM_WIFI_COMPAT (1)
#define HAL_PLATFORM_NCP_COUNT (1)
#define HAL_PLATFORM_MAX_CLOUD_CONNECT_TIME (1*60*1000)
#define HAL_PLATFORM_ESP32_NCP (1)

#define PRODUCT_SERIES "argon"

#if PLATFORM_ID == PLATFORM_ARGON

#define HAL_PLATFORM_RADIO_ANTENNA_INTERNAL (1)
#define HAL_PLATFORM_RADIO_ANTENNA_EXTERNAL (1)

#if HAL_PLATFORM_ETHERNET
#define HAL_PLATFORM_ETHERNET_WIZNETIF_CS_PIN_DEFAULT    (D5)
#define HAL_PLATFORM_ETHERNET_WIZNETIF_RESET_PIN_DEFAULT (D3)
#define HAL_PLATFORM_ETHERNET_WIZNETIF_INT_PIN_DEFAULT   (D4)
#endif // HAL_PLATFORM_ETHERNET

#else // PLATFORM_ID != PLATFORM_ARGON (i.e. PLATFORM_ASOM)

#define HAL_PLATFORM_PMIC_BQ24195 (1)
#define HAL_PLATFORM_PMIC_BQ24195_I2C (HAL_I2C_INTERFACE1)
#define HAL_PLATFORM_FUELGAUGE_MAX17043 (1)
#define HAL_PLATFORM_FUELGAUGE_MAX17043_I2C (HAL_I2C_INTERFACE1)
#define HAL_PLATFORM_POWER_MANAGEMENT (1)
#define HAL_PLATFORM_POWER_MANAGEMENT_OPTIONAL (1)
#define HAL_PLATFORM_PMIC_BQ24195_FAULT_COUNT_THRESHOLD (4)
#define HAL_PLATFORM_RADIO_ANTENNA_EXTERNAL (1)

#if HAL_PLATFORM_ETHERNET
#define HAL_PLATFORM_ETHERNET_WIZNETIF_CS_PIN_DEFAULT    (D8)
#define HAL_PLATFORM_ETHERNET_WIZNETIF_RESET_PIN_DEFAULT (A7)
#define HAL_PLATFORM_ETHERNET_WIZNETIF_INT_PIN_DEFAULT   (D22)
#endif // HAL_PLATFORM_ETHERNET

#endif // PLATFORM_ID == PLATFORM_ARGON
