// system.c
#include "system.h"
#include "uart.h"
#include "gpio.h"
#include "usb.h"

volatile uint32_t ms_ticks = 0;

void SysTick_Handler(void) {
    ms_ticks++;
}

void init_CPU(void) {
    RST_CLK_DeInit();
    RST_CLK_HSEconfig(RST_CLK_HSE_ON);
    while (RST_CLK_HSEstatus() != SUCCESS);
    RST_CLK_CPUclkSelectionC1(RST_CLK_CPU_C1srcHSEdiv1);
    RST_CLK_CPUclkPrescaler(RST_CLK_CPUclkDIV1);
    RST_CLK_CPUclkSelection(RST_CLK_CPUclkCPU_C3);
    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000);
}

void delay_ms(uint32_t ms) {
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms) {}
}

// === Один символ → в UART и в USB ===
void send_char(char c) {
    while (MDR_UART2->FR & UART_FLAG_TXFF) {}
    MDR_UART2->DR = (uint32_t)c;
    send_usb_char(c);  // Теперь через буфер
}

// === Строка ===
void send_string(const char* str) {
    while (*str) {
        send_char(*str++);
    }
}

// === Десятичное число ===
void send_decimal(uint32_t value) {
    if (value == 0) {
        send_char('0');
        return;
    }
    char buf[12];
    int i = 0;
    uint32_t val = value;
    while (val > 0) {
        buf[i++] = (val % 10) + '0';
        val /= 10;
    }
    while (i > 0) {
        send_char(buf[--i]);
    }
}

// === Hex ===
void send_hex(uint8_t value) {
    const char hex[] = "0123456789ABCDEF";
    send_char(hex[(value >> 4) & 0xF]);
    send_char(hex[value & 0xF]);
}

// === Отправка в оба интерфейса (просто строка) ===
void send_all(const char* str) {
    send_string(str);
    //usb_flush();  // <-- Принудительно отправляем пакет
}