#include "max31865.h"
#include "system.h"
#include "uart.h"
#include "usb.h"
#include <math.h>

// === НАСТРОЙКИ ===
#define MAX31865_WIRES        2             // 2, 3 или 4 провода
#define MAX31865_RREF         430.0f        // Опорный резистор: 390 или 430 Ом  (погрешность резистора VREF поэтому)
#define TEMP_CALIBRATION      (-5.0f)       // Калибровка температуры

// Фильтрация (опционально)
// #define USE_IIR_FILTER
#ifdef USE_IIR_FILTER
#define FILTER_ALPHA          0.2f
static float filtered_temp = NAN;
#endif

// Макросы CS
#define T1_CS_LOW()     PORT_ResetBits(T1_CS_PORT, T1_CS_PIN)
#define T1_CS_HIGH()    PORT_SetBits(T1_CS_PORT, T1_CS_PIN)

// Регистры MAX31865
#define MAX31865_REG_CONFIG     0x00
#define MAX31865_REG_RTD_MSB    0x01
#define MAX31865_REG_FAULT      0x07

// Биты конфигурации
#define MAX31865_BIAS_ON        (1 << 7)
#define MAX31865_MODE_AUTO      (1 << 6)
#define MAX31865_ONE_SHOT       (1 << 5)
#define MAX31865_3WIRE_MODE     (1 << 4)
#define MAX31865_FILTER_50HZ    (1 << 1)
#define MAX31865_FAULT_STATUS   (1 << 2)

// --- SPI Transfer ---
static uint8_t SSP2_Transfer(uint8_t data) {
    while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_TFE) == RESET);
    SSP_SendData(MDR_SSP2, data);
    while (SSP_GetFlagStatus(MDR_SSP2, SSP_FLAG_RNE) == RESET);
    return SSP_ReceiveData(MDR_SSP2);
}

// --- Инициализация MAX31865 ---
void MAX31865_Init(void) {
    //send_all("MAX31865: Initializing...\r\n");
    //usb_flush();
    // === 1. Тактирование портов и SSP2 ===
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTD | RST_CLK_PCLK_PORTB | RST_CLK_PCLK_SSP2, ENABLE);

    // Принудительно включаем тактирование SSP2 (бит 13)
    MDR_RST_CLK->PER_CLOCK |= (1 << 13);

    // if (MDR_RST_CLK->PER_CLOCK & (1 << 13)) {
    //     //send_all("OK: SSP2 clock enabled.\r\n");
    //     //usb_flush();
    // } else {
    //     //send_all("ERROR: Failed to enable SSP2 clock!\r\n");
    //    // usb_flush();
    // }

    PORT_InitTypeDef port;
    SSP_InitTypeDef ssp;

    // === 2. Настройка PORTD: SCK (PD5), MOSI (PD6), MISO (PD2) ===
    PORT_StructInit(&port);
    port.PORT_MODE   = PORT_MODE_DIGITAL;
    port.PORT_SPEED  = PORT_SPEED_FAST;
    port.PORT_GFEN   = PORT_GFEN_OFF;

    // SCK, MOSI — выход, альтернативная функция
    port.PORT_Pin    = PORT_Pin_5 | PORT_Pin_6;
    port.PORT_OE     = PORT_OE_OUT;
    port.PORT_FUNC   = PORT_FUNC_ALTER;
    PORT_Init(MDR_PORTD, &port);

    // MISO — вход, альтернативная функция
    port.PORT_Pin    = PORT_Pin_2;
    port.PORT_OE     = PORT_OE_IN;
    port.PORT_FUNC   = PORT_FUNC_ALTER;
    PORT_Init(MDR_PORTD, &port);

    // === 3. CS (PB4) ===
    port.PORT_Pin    = PORT_Pin_4;
    port.PORT_OE     = PORT_OE_OUT;
    port.PORT_FUNC   = PORT_FUNC_PORT;
    port.PORT_MODE   = PORT_MODE_DIGITAL;
    port.PORT_SPEED  = PORT_SPEED_FAST;
    PORT_Init(MDR_PORTB, &port);

    // Проверка CS
   // send_all("Testing CS (PB4)...\r\n");
    T1_CS_LOW();
    delay_ms(10);
   // send_all("CS LOW? ");
   // send_all((MDR_PORTB->RXTX & PORT_Pin_4) ? "NO" : "YES");
   // send_all("\r\n");
   // usb_flush();

    T1_CS_HIGH();
    delay_ms(10);
   // send_all("CS HIGH? ");
   // send_all((MDR_PORTB->RXTX & PORT_Pin_4) ? "YES" : "NO");
   // send_all("\r\n");

   // usb_flush();

    // === 4. Инициализация SSP2 ===
    SSP_DeInit(MDR_SSP2);
    SSP_BRGInit(MDR_SSP2, SSP_HCLKdiv1);

    SSP_StructInit(&ssp);
    ssp.SSP_Mode         = SSP_ModeMaster;
    ssp.SSP_SPO          = SSP_SPO_Low;      // Clock Idle = Low
    ssp.SSP_SPH          = SSP_SPH_2Edge;    // Data on 2nd edge → SPI Mode 1
    ssp.SSP_FRF          = SSP_FRF_SPI_Motorola;
    ssp.SSP_WordLength   = SSP_WordLength8b;
    ssp.SSP_CPSDVSR      = 16;               // ~500 kHz
    SSP_Init(MDR_SSP2, &ssp);
    SSP_Cmd(MDR_SSP2, ENABLE);

    delay_ms(10);

    // === 5. Инициализация MAX31865: Auto-Convert Mode ===
    T1_CS_HIGH();
    delay_ms(10);

    // Auto-Convert + Bias ON + 50Hz filter + 2-wire
    uint8_t config_value = 0xC1;  // 0b11000001

   // send_all("Writing CONFIG: 0x");
    //send_hex(config_value);
   // send_all("\r\n");
   // usb_flush();

    T1_CS_LOW();
    SSP2_Transfer(MAX31865_REG_CONFIG | 0x80);  // Write
    SSP2_Transfer(config_value);
    T1_CS_HIGH();

    delay_ms(10);

    // Чтение обратно
    T1_CS_LOW();
    SSP2_Transfer(MAX31865_REG_CONFIG & 0x7F);  // Read
    uint8_t read_config = SSP2_Transfer(0);
    T1_CS_HIGH();

    //send_all("Read CONFIG back: 0x");
    //send_hex(read_config);
    //send_all("\r\n");
    //usb_flush();

    // if (read_config != config_value) {
    //     send_all("WARNING: CONFIG mismatch! Check wiring.\r\n");
    //     usb_flush();
    // } else {
    //     send_all("SUCCESS: CONFIG written correctly.\r\n");
    //     usb_flush();
    // }

    // Сброс флагов ошибок
    T1_CS_LOW();
    SSP2_Transfer(MAX31865_REG_CONFIG & 0x7F);
    uint8_t cfg = SSP2_Transfer(0);
    T1_CS_HIGH();

    cfg |= MAX31865_FAULT_STATUS;
    T1_CS_LOW();
    SSP2_Transfer(MAX31865_REG_CONFIG | 0x80);
    SSP2_Transfer(cfg);
    T1_CS_HIGH();

    // usb_flush();
    // send_all("Fault flags cleared.\r\n");
    // usb_flush();
}

// --- Чтение RTD ---
uint16_t MAX31865_ReadRTD(uint8_t sensor_id) {
    if (sensor_id != 1) return 0;

    // Сброс флагов ошибок
    T1_CS_LOW();
    SSP2_Transfer(MAX31865_REG_CONFIG & 0x7F);
    uint8_t cfg = SSP2_Transfer(0);
    T1_CS_HIGH();

    cfg |= MAX31865_FAULT_STATUS;
    T1_CS_LOW();
    SSP2_Transfer(MAX31865_REG_CONFIG | 0x80);
    SSP2_Transfer(cfg);
    T1_CS_HIGH();

    // Включить BIAS (уже включён в Auto-Mode, но перестрахуемся)
    cfg |= MAX31865_BIAS_ON;
    T1_CS_LOW();
    SSP2_Transfer(MAX31865_REG_CONFIG | 0x80);
    SSP2_Transfer(cfg);
    T1_CS_HIGH();

    delay_ms(10);  // Для стабилизации

    // === Чтение RTD (в Auto-Mode не нужен запуск) ===
    T1_CS_LOW();
    SSP2_Transfer(MAX31865_REG_RTD_MSB & 0x7F);  // Read command
    uint8_t msb = SSP2_Transfer(0);
    uint8_t lsb = SSP2_Transfer(0);
    T1_CS_HIGH();

    uint16_t rtd = ((msb << 8) | lsb) >> 1;

    // send_all("MAX31865: RTD = ");
    // usb_flush();
    // send_decimal(rtd);
    // usb_flush();
    // send_all(" (0x");
    // send_hex(msb);
    // send_hex(lsb);
    // send_all(")\r\n");
    // usb_flush();

    return rtd;
}

// --- Чтение температуры ---
float MAX31865_ReadTemperature(uint8_t sensor_id) {
#ifdef MAX31865_TEST_MODE
    return 23.5f + TEMP_CALIBRATION;
#else
    uint16_t rtd = MAX31865_ReadRTD(sensor_id);

    // Чтение FAULT
    T1_CS_LOW();
    SSP2_Transfer(MAX31865_REG_FAULT & 0x7F);  // Read command
    uint8_t fault = SSP2_Transfer(0);
    T1_CS_HIGH();

    send_all("RAW FAULT = 0x");
    send_hex(fault);
    send_usb_hex(fault);
    send_all("\r\n");
    usb_flush();

    if (fault != 0) {
        if (fault & 0x01) send_all(" -> RTDIN- > 0.85×VBIAS (open)\r\n");
        if (fault & 0x02) send_all(" -> RTDIN- < 0.85×VBIAS (short)\r\n");
        if (fault & 0x04) send_all(" -> REFIN- > 0.85×VBIAS\r\n");
        if (fault & 0x08) send_all(" -> Over/undervoltage\r\n");
        usb_flush();
        return TEMP_ERROR_VALUE;
    }

    if (rtd == 0 || rtd == 0xFFFF) {
        send_all("ERROR: RTD = 0 or 0xFFFF → check RREF and wiring!\r\n");
        usb_flush();
        return TEMP_ERROR_VALUE;
    }

    float resistance = (rtd * MAX31865_RREF) / 32768.0f;
    float temp = (resistance - 100.0f) / 0.385f;
    temp += TEMP_CALIBRATION;

#ifdef USE_IIR_FILTER
    if (isnan(filtered_temp)) filtered_temp = temp;
    filtered_temp = filtered_temp * (1.0f - FILTER_ALPHA) + temp * FILTER_ALPHA;
    temp = filtered_temp;
#endif

    return temp;
#endif
}