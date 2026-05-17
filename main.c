// <h> JTAG pins protection for MCU MDR32F9Q2I, K1986VE9xI and MDR32FG16S1QI
// <i> Prevents RXTX and OE bits setting for JTAG when doing GPIO
// read-modify-write <i> Uncomment the definition(s) below to define used JTAG
// port(s). <i> Leave all commented/unchecked if there is no GPIO pins combined
// with JTAG.

// <c> JTAG_A pins protection
// #define USE_JTAG_A
// </c>

// <c> JTAG_B pins protection
// #define USE_JTAG_B
// </c>
// </h>

// <o> HSE clock value [Hz]
// <i> Default: 8000000 (8MHz)
// #define HSE_Value       ((uint32_t)8000000)
// <o> HSE2 clock value [Hz] for MDR32F1QI, K1986VE1xI

// Тестовый режим (раскомментировать для вывода без датчиков)
// #define MAX31865_TEST_MODE
// в max31865.h

// #define HSE_Value ((uint32_t)16000000)
#define _ISOC99_SOURCE
#include <ctype.h>
#include <math.h>
#include <stdio.h>

#include "gpio.h"
#include "max31865.h"
#include "system.h"
#include "uart.h"
#include "usb.h"


volatile uint8_t system_ready = 0;  // Было uart_ready
volatile uint8_t auto_temp_enabled = 0;
uint32_t last_auto_read = 0;

extern void show_menu(void);
extern void process_immediate_command(uint8_t cmd);

int main(void) {
  init_CPU();
  delay_ms(200);
  init_USB();
  delay_ms(500);

  init_GPIO();
  init_UART();
  delay_ms(200);
  MAX31865_Init();
  SysTick_Config(SystemCoreClock / 1000);
  startup_animation();
 
  send_all("System started.\r\n");
  usb_flush();

  send_all("Waiting for 'H' on UART or USB...\r\n");
  usb_flush();

  uint32_t start_time = ms_ticks;
  uint32_t timeout_duration = 120000;  // 120 секунд
  uint32_t last_update = ms_ticks;

while (!system_ready && (ms_ticks - start_time) < timeout_duration) {
    if ((ms_ticks - last_update) >= 5000) {
        int32_t time_left = (timeout_duration - (ms_ticks - start_time)) / 1000;
        send_all("Time remaining: ");
        send_decimal(time_left);
        send_all(" seconds. Press 'H' to connect.\r\n");
        usb_flush();
        last_update = ms_ticks;
    }

    // === Приём по UART ===
    if (!(MDR_UART2->FR & UART_FLAG_RXFE)) {
        uint8_t data = (uint8_t)(MDR_UART2->DR & 0xFF);
        if (data == 'H' || data == 'h') {
            system_ready = 1;
            send_all("\r\nTerminal connected via UART!\r\n");
            usb_flush();
            connect_animation();

            send_all("INITIAL TEMP READ:\r\n");
            usb_flush();
            float t1 = MAX31865_ReadTemperature(1);
            char msg[128];
            if (t1 < -990.0f)
                sprintf(msg, "  T1 = ERROR (no sensor)\r\n");
            else
                sprintf(msg, "  T1 = %.1f C\r\n", t1);
            send_all(msg);
            usb_flush();
            show_menu();
            usb_flush();
        }
    }

    // === Приём по USB ===
    if (usb_data_received) {
        uint8_t cmd = usb_data_received;
        usb_data_received = 0;

        // char dbg[64];
        // sprintf(dbg, "DEBUG: USB received byte = 0x%02X ('%c')\r\n", cmd, isprint(cmd) ? cmd : '?');
        // send_all(dbg);
        // usb_flush();
            system_ready = 1;
        if (cmd == 'H' || cmd == 'h') {
            system_ready = 1;

            // Даём время на стабилизацию
            delay_ms(100);

            send_all("\r\nTerminal connected via USB!\r\n");
            usb_flush();
            connect_animation();

            send_all("INITIAL TEMP READ:\r\n");
            usb_flush();
            float t1 = MAX31865_ReadTemperature(1);
            char msg[128];
            if (t1 < -990.0f)
                sprintf(msg, "  T1 = ERROR (no sensor)\r\n");
            else
                sprintf(msg, "  T1 = %.1f C\r\n", t1);
            send_all(msg);
            usb_flush();
            show_menu(); 
            //usb_flush();           
        }
    }

    // Убрали delay_ms(10) → теперь цикл работает без блокировки
}

  if (!system_ready) {
    send_string("\r\n[TIMEOUT] No 'H' received. Session closed.\r\n");
    usb_flush();
    while (1) {
      LED_TOGGLE(LED_1);
      delay_ms(500);
    }
  }

  while (1) {
    // === UART ===
    if (!(MDR_UART2->FR & UART_FLAG_RXFE)) {
      uint8_t data = (uint8_t)(MDR_UART2->DR & 0xFF);
      process_immediate_command(data);
    }

    // === USB ===
    if (usb_data_received) {
      uint8_t cmd = usb_data_received;
      usb_data_received = 0;
      process_immediate_command(cmd);
    }

    // === Авточтение температуры ===
    if (auto_temp_enabled && (ms_ticks - last_auto_read) >= 3000) {
      float t1 = MAX31865_ReadTemperature(1);
      char msg[128];
      if (t1 < -990.0f)
        sprintf(msg, "AUTO: T1=ERROR\r\n");
      else
        sprintf(msg, "AUTO: T1=%.1f C\r\n", t1);
      send_all(msg);
      usb_flush();
      last_auto_read = ms_ticks;
    }

    delay_ms(10);
  }
}