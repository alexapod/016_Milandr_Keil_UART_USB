// usb.c
// Управление USB-CDC интерфейсом для MDR32F9Q2I
// Поддержка буферизованной отправки, защита от зависаний

#include "system.h"
#include "gpio.h"
#include "usb.h"
#include <string.h>

// === Глобальные переменные ===
volatile uint8_t usb_data_received = 0;

// === Буфер отправки USB (64 байта — стандартный размер пакета) ===

static char usb_tx_buffer[64];
static uint8_t usb_tx_count = 0;

// === Принудительная отправка содержимого буфера ===
void usb_flush(void) {
    if (usb_tx_count == 0) return;
    while (USB_CDC_SendData((uint8_t*)usb_tx_buffer, usb_tx_count) == USB_ERR_BUSY) {
        delay_ms(1);  // Ждём, пока хост будет готов
    }
    usb_tx_count = 0;  // Очищаем буфер
}

// === Инициализация USB устройства ===
void init_USB(void) {
    //delay_ms(100);

    USB_Clock_TypeDef USB_CLK = {0};
    USB_DeviceBUSParam_TypeDef USB_BUS = {0};

    // Настройка тактирования USB
    USB_CLK.USB_USBC1_Source = USB_C1HSEdiv2;
    USB_CLK.USB_PLLUSBMUL    = USB_PLLUSBMUL12;

    // Настройка параметров шины
    USB_BUS.MODE  = USB_SC_SCFSP_Full;       // Полный режим
    USB_BUS.SPEED = USB_SC_SCFSR_12Mb;       // Скорость 12 Мбит/с
    USB_BUS.PULL  = USB_HSCR_DP_PULLUP_Set;  // Внутренний подтягивающий резистор

    //delay_ms(100);

    RST_CLK_PCLKcmd(RST_CLK_PCLK_USB, ENABLE);  // Тактирование USB
     //delay_ms(100);

    USB_BRGInit(&USB_CLK);
     //delay_ms(100);
    USB_DeviceInit(&USB_CLK, &USB_BUS);
     //delay_ms(100);
    USB_SetSIM(USB_SIS_Msk);
     //delay_ms(100);
    USB_DevicePowerOn();
     //delay_ms(100);
    USB_DEVICE_HANDLE_RESET;
    // delay_ms(100);
    NVIC_EnableIRQ(USB_IRQn);

    //delay_ms(100);

//    // Инициализация CDC
//    static uint8_t usb_rx_buffer[64];
    // delay_ms(100);

   
    // delay_ms(100);

    // Анимация: мигание светодиодом
    for (int i = 0; i < 3; i++) {
        LED_TOGGLE(LED_1);
        delay_ms(150);
    }
    delay_ms(100);
}

// === Обработчик получения данных по CDC ===
USB_Result USB_CDC_RecieveData(uint8_t* Buffer, uint32_t Length) {
    if (Length > 0) {
        usb_data_received = Buffer[0];  // Сохраняем первый байт
    }
    return USB_SUCCESS;
}

// === Обработчик запроса Line Coding (терминал запрашивает параметры COM-порта) ===
USB_Result USB_CDC_GetLineCoding(uint16_t wINDEX, USB_CDC_LineCoding_TypeDef* DATA) {
    DATA->dwDTERate   = 115200;     // Скорость
    DATA->bCharFormat = 0;          // 1 стоп-бит
    DATA->bParityType = 0;          // Без чётности
    DATA->bDataBits   = 8;          // 8 бит данных
    return USB_SUCCESS;
}

// === Обработчик установки Line Coding ===
USB_Result USB_CDC_SetLineCoding(uint16_t wINDEX, const USB_CDC_LineCoding_TypeDef* DATA) {
    return USB_SUCCESS;
}

// === Отправка одного символа (с буферизацией) ===
void send_usb_char(char c) {
    usb_tx_buffer[usb_tx_count++] = c;

    // Если буфер заполнен — отправляем
    if (usb_tx_count >= 64) {
        usb_flush();
    }
}

// === Отправка строки ===
void send_usb_string(const char* str) {
    if (!str) return;
    while (*str) {
        send_usb_char(*str++);
    }
}

// === Отправка десятичного числа ===
void send_usb_decimal(uint32_t value) {
    if (value == 0) {
        send_usb_char('0');
        return;
    }

    char buf[12];
    int i = 0;
    while (value > 0) {
        buf[i++] = (value % 10) + '0';
        value /= 10;
    }
    while (i > 0) {
        send_usb_char(buf[--i]);
    }
}

// === Отправка байта в HEX ===
void send_usb_hex(uint8_t value) {
    const char hex[] = "0123456789ABCDEF";
    send_usb_char(hex[(value >> 4) & 0xF]);
    send_usb_char(hex[value & 0xF]);
}