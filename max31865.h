
#ifndef MAX31865_H
#define MAX31865_H

#include <stdint.h>
#include "MDR32F9Q2I.h"
#include "MDR32FxQI_ssp.h"
#include "MDR32FxQI_port.h"

// --- CS: Chip Select ---
#define T1_CS_PORT        MDR_PORTB
#define T1_CS_PIN         PORT_Pin_4


// --- DRDY: Data Ready ---
//#define T1_DRDY_PORT      MDR_PORTB
//#define T1_DRDY_PIN       PORT_Pin_8


// Макросы управления CS
#define T1_CS_LOW()       PORT_ResetBits(T1_CS_PORT, T1_CS_PIN)
#define T1_CS_HIGH()      PORT_SetBits(T1_CS_PORT, T1_CS_PIN)


// Макросы чтения DRDY
#define T1_DRDY_READY()   (!(T1_DRDY_PORT->RXTX & T1_DRDY_PIN))


// SPI
#define SPI_PORT          MDR_PORTD
#define SPI_PIN_MISO      PORT_Pin_2
#define SPI_PIN_CLK       PORT_Pin_5
#define SPI_PIN_MOSI      PORT_Pin_6
#define SPI_INSTANCE      MDR_SSP2

// Регистры MAX31865
#define MAX31865_REG_RTD_MSB    0x01
#define MAX31865_REG_RTD_LSB    0x02
#define MAX31865_REG_CONFIG     0x00
#define MAX31865_REG_FAULT      0x07

// --- Режим проводов ---
// Раскомментируйте ОДИН
#define WIRE_MODE_2   // 2-проводное подключение
// #define WIRE_MODE_3   // 3-проводное подключение
//#define WIRE_MODE_4   // 4-проводное подключение

#if defined(WIRE_MODE_3)
#define CONFIG_3WIRE_BIT 1
#else
#define CONFIG_3WIRE_BIT 0  // Для 2- или 4-проводного
#endif

// Конфигурация: bias on, auto convert, 50Hz filter
#define MAX31865_CONFIG_DEFAULT \
    (1 << 7) |        /* Bias = 1 */ \
    (1 << 6) |        /* MODE = 1 (auto convert) */ \
    (CONFIG_3WIRE_BIT << 4) | \
    (0 << 2)          /* FILTER = 0 → 50 Hz */

// Калибровка температуры (в градусах)
#define TEMP_CALIBRATION_T1   (+0.0f)  // Пример: +0.5f, -1.2f


// Включить усреднение температуры
#define USE_TEMPERATURE_FILTERING

#ifdef USE_TEMPERATURE_FILTERING
#define FILTER_SAMPLES 5  // Количество точек для усреднения
#endif

// Признак ошибки (вместо NAN)
#define TEMP_ERROR_VALUE (-999.0f)

// Прототипы
void MAX31865_Init(void);
float MAX31865_ReadTemperature(uint8_t sensor_id);
uint16_t MAX31865_ReadRTD(uint8_t sensor_id);
void MAX31865_CheckFault(uint8_t sensor_id);

#endif // MAX31865_H