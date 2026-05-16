// system.h
#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include "MDR32F9Q2I.h"

// === Сначала HAL-драйверы ===
#include "MDR32FxQI_rst_clk.h"
#include "MDR32FxQI_port.h"
#include "MDR32FxQI_uart.h"
#include "MDR32FxQI_ssp.h"

// === ЗАТЕМ конфигурация и USB ===
#include "MDR32FxQI_config.h"

// === USB библиотека ===
#include "MDR32FxQI_usb.h"
#include "MDR32FxQI_usb_device.h"
#include "MDR32FxQI_usb_CDC.h"



// === Функции и переменные ===
extern volatile uint32_t ms_ticks;


void init_CPU(void);

void delay_ms(uint32_t ms);
void send_char(char c);
void send_string(const char* str);
void send_decimal(uint32_t value);
void send_hex(uint8_t value);

void send_all(const char* str);  // <-- НОВОЕ: отправка в оба интерфейса



#endif // SYSTEM_H