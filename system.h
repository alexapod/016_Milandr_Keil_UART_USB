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


#include "MDR32FxQI_config.h"           // <-- ДО usb.h

// === USB библиотека ===
#include "MDR32FxQI_usb.h"
#include "MDR32FxQI_usb_device.h"
#include "MDR32FxQI_usb_CDC.h"

// === Добавьте это перед #include "MDR32FxQI_usb_handlers.h" ===
USB_Result USB_CDC_DummyDataReceive(uint8_t* Buffer, uint32_t Length);
// === Обработчики USB ===
#include "MDR32FxQI_usb_handlers.h"     // <-- Последний!

// === Функции и переменные ===
extern volatile uint32_t ms_ticks;
extern volatile uint8_t usb_data_received;

void init_CPU(void);
void init_USB(void);
void delay_ms(uint32_t ms);
void send_char(char c);
void send_string(const char* str);
void send_decimal(uint32_t value);
void send_hex(uint8_t value);

void send_usb(const char* str);

// === Обработчик USB CDC ===
// === Обработчик USB CDC (обязательно!) ===
USB_Result USB_CDC_RecieveData(uint8_t* Buffer, uint32_t Length);
USB_Result USB_CDC_GetLineCoding(uint16_t wINDEX, USB_CDC_LineCoding_TypeDef* DATA);
USB_Result USB_CDC_SetLineCoding(uint16_t wINDEX, const USB_CDC_LineCoding_TypeDef* DATA);


#endif // SYSTEM_H