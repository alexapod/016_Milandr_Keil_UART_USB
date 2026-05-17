// usb.h
#ifndef USB_H
#define USB_H

#include "MDR32FxQI_config.h"
#include "MDR32FxQI_usb.h"
#include "MDR32FxQI_usb_CDC.h"
#include "MDR32FxQI_usb_device.h"
#include "MDR32FxQI_usb_handlers.h"  // ← Должен быть включён (содержит extern)
#include "system.h"

#define USB_TX_BUFFER_SIZE 64

extern volatile uint8_t usb_data_received;

// === Функции отправки ===
void send_usb_char(char c);
void send_usb_string(const char* str);
void send_usb_decimal(uint32_t value);
void send_usb_hex(uint8_t value);
void usb_flush(void);

void init_USB(void);

// === Ваши обработчики CDC ===
USB_Result USB_CDC_RecieveData(uint8_t* Buffer, uint32_t Length);
USB_Result USB_CDC_GetLineCoding(uint16_t wINDEX, USB_CDC_LineCoding_TypeDef* DATA);
USB_Result USB_CDC_SetLineCoding(uint16_t wINDEX, const USB_CDC_LineCoding_TypeDef* DATA);

#endif // USB_H