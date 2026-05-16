#ifndef USB_H
#define USB_H

#define _ISOC99_SOURCE

#include "MDR32FxQI_config.h"

#include "MDR32FxQI_usb.h"              // Drivers:USB
#include "MDR32FxQI_usb_CDC.h"          // Drivers:USB Library
#include "MDR32FxQI_usb_device.h"       // Drivers:USB Library

#include "usb.h"

#include <ctype.h>
#include <stdio.h>
#include <math.h>

#include "gpio.h"
#include "max31865.h"
#include "system.h"


// === Обработчики USB ===
USB_Result USB_CDC_DummyDataReceive(uint8_t* Buffer, uint32_t Length);
#include "MDR32FxQI_usb_handlers.h"

extern volatile uint8_t usb_data_received;

void send_usb(const char* str);


void init_USB(void);

// === Обработчики USB CDC ===
USB_Result USB_CDC_RecieveData(uint8_t* Buffer, uint32_t Length);
USB_Result USB_CDC_GetLineCoding(uint16_t wINDEX, USB_CDC_LineCoding_TypeDef* DATA);
USB_Result USB_CDC_SetLineCoding(uint16_t wINDEX, const USB_CDC_LineCoding_TypeDef* DATA);
#endif // USB_H