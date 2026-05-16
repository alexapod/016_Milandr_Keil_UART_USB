// system.c
#include "system.h"
#include "uart.h"
#include "gpio.h"

volatile uint32_t ms_ticks = 0;
volatile uint8_t usb_data_received = 0;


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

void init_USB(void)
{
    send_string("USB: Init started...\r\n");

    USB_Clock_TypeDef USB_CLK = {0};
    USB_DeviceBUSParam_TypeDef USB_BUS = {0};

    // === Тактирование USB ===
    USB_CLK.USB_USBC1_Source = USB_C1HSEdiv2;   // 8 МГц / 2 = 4 МГц
    USB_CLK.USB_PLLUSBMUL    = USB_PLLUSBMUL12; // 4 МГц ×12 = 48 МГц

    // === Параметры шины ===
    USB_BUS.MODE  = USB_SC_SCFSP_Full;           // Full-speed mode
    USB_BUS.SPEED = USB_SC_SCFSR_12Mb;           // 12 Mbps
    USB_BUS.PULL  = USB_HSCR_DP_PULLUP_Set;      // Включаем pull-up на D+

    // === Включаем тактирование USB ===
    RST_CLK_PCLKcmd(RST_CLK_PCLK_USB, ENABLE);

    // === Инициализация ===
    USB_BRGInit(&USB_CLK);
    send_string("USB: BRGInit done.\r\n");

    USB_DeviceInit(&USB_CLK, &USB_BUS);
    send_string("USB: DeviceInit done.\r\n");

    // === Включаем прерывания ===
    USB_SetSIM(USB_SIS_Msk);

    USB_DevicePowerOn();
    send_string("USB: PowerOn done.\r\n");

    // === КРИТИЧНО: сброс устройства ===
    USB_DEVICE_HANDLE_RESET;

#ifdef USB_INT_HANDLE_REQUIRED
    NVIC_EnableIRQ(USB_IRQn);
    send_string("USB: IRQ enabled.\r\n");
#endif

    // === Инициализация CDC ===
    static uint8_t usb_rx_buffer[64];
    USB_CDC_Init(usb_rx_buffer, 1, SET);

    for (int i = 0; i < 3; i++) {
        LED_TOGGLE(LED_1);
        delay_ms(150);
    }
    send_string("USB: Ready.\r\n");
}


// --- Обработчик приёма данных по USB CDC ---
USB_Result USB_CDC_RecieveData(uint8_t* Buffer, uint32_t Length)
{
    if (Length > 0)
    {
        // Берём первый байт как команду (остальные игнорируем)
        usb_data_received = Buffer[0];
    }
    return USB_SUCCESS;
}

// --- Обязательные обработчики CDC ---
// --- Обработчик Line Coding (правильная структура) ---
USB_Result USB_CDC_GetLineCoding(uint16_t wINDEX, USB_CDC_LineCoding_TypeDef* DATA)
{
    // Заполняем поля по спецификации CDC
    DATA->dwDTERate   = 115200;      // Скорость: 115200 бод
    DATA->bCharFormat = 0;           // 0 = 1 stop bit, 1 = 1.5, 2 = 2 stop bits
    DATA->bParityType = 0;           // 0 = нет чётности, 1 = нечётная, 2 = чётная
    DATA->bDataBits   = 8;           // 8 бит данных

    return USB_SUCCESS;
}

USB_Result USB_CDC_SetLineCoding(uint16_t wINDEX, const USB_CDC_LineCoding_TypeDef* DATA)
{
    // Можно игнорировать или сохранить значения
    // Например: скорость можно использовать для чего-то, но в нашем случае — просто принимаем
    return USB_SUCCESS;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = ms_ticks;
    while ((ms_ticks - start) < ms) {
        // Ждём
    }
}

void send_char(char c)
{
    while (MDR_UART2->FR & UART_FLAG_TXFF) {
        // Ждём
    }
    MDR_UART2->DR = (uint32_t)c;
}

void send_string(const char* str)
{
    while (*str) {
        send_char(*str++);
    }
}

void send_decimal(uint32_t value)
{
    char buf[12];
    int i = 0;

    if (value == 0) {
        send_char('0');
        return;
    }

    while (value > 0) {
        buf[i++] = (value % 10) + '0';
        value /= 10;
    }

    while (i > 0) {
        send_char(buf[--i]);
    }
}

void send_hex(uint8_t value) {
    char hex[] = "0123456789ABCDEF";
    send_char(hex[(value >> 4) & 0xF]);
    send_char(hex[value & 0xF]);
}


// Где-то в system.c или отдельно
void send_usb(const char* str) {
    uint8_t len = 0;
    const char* p = str;
    while (*p++ && len < 64) len++;  // Ограничение длины

    if (len > 0) {
        while (USB_CDC_SendData((uint8_t*)str, len) == USB_ERR_BUSY) {
            delay_ms(1);  // Ждём освобождения буфера
        }
    }
}