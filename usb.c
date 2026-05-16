#include "system.h"
#include "gpio.h"
#include "usb.h"

volatile uint8_t usb_data_received = 0;

void init_USB(void) {
    delay_ms(100);

    USB_Clock_TypeDef USB_CLK = {0};
    USB_DeviceBUSParam_TypeDef USB_BUS = {0};

    USB_CLK.USB_USBC1_Source = USB_C1HSEdiv2;
    USB_CLK.USB_PLLUSBMUL    = USB_PLLUSBMUL12;

    USB_BUS.MODE  = USB_SC_SCFSP_Full;
    USB_BUS.SPEED = USB_SC_SCFSR_12Mb;
    USB_BUS.PULL  = USB_HSCR_DP_PULLUP_Set;

    RST_CLK_PCLKcmd(RST_CLK_PCLK_USB, ENABLE);

    USB_BRGInit(&USB_CLK);
    USB_DeviceInit(&USB_CLK, &USB_BUS);
    USB_SetSIM(USB_SIS_Msk);
    USB_DevicePowerOn();
    USB_DEVICE_HANDLE_RESET;
    NVIC_EnableIRQ(USB_IRQn);

    delay_ms(100);

    static uint8_t usb_rx_buffer[64];
    USB_CDC_Init(usb_rx_buffer, 1, SET);

    for (int i = 0; i < 3; i++) {
        LED_TOGGLE(LED_1);
        delay_ms(150);
    }
    delay_ms(100);
}

USB_Result USB_CDC_RecieveData(uint8_t* Buffer, uint32_t Length) {
    if (Length > 0) {
        usb_data_received = Buffer[0];
    }
    return USB_SUCCESS;
}

USB_Result USB_CDC_GetLineCoding(uint16_t wINDEX, USB_CDC_LineCoding_TypeDef* DATA) {
    DATA->dwDTERate   = 115200;
    DATA->bCharFormat = 0;
    DATA->bParityType = 0;
    DATA->bDataBits   = 8;
    return USB_SUCCESS;
}

USB_Result USB_CDC_SetLineCoding(uint16_t wINDEX, const USB_CDC_LineCoding_TypeDef* DATA) {
    return USB_SUCCESS;
}

void send_usb(const char* str) {
    uint8_t len = 0;
    const char* p = str;
    while (*p++ && len < 64) len++;
    if (len > 0) {
        while (USB_CDC_SendData((uint8_t*)str, len) == USB_ERR_BUSY) {
            delay_ms(1);
        }
    }
}
