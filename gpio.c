
#include "MDR32F9Q2I.h"                 // Device header
#include "MDR32FxQI_port.h"             // Drivers:PORT
#include "MDR32FxQI_rst_clk.h"          // Drivers:RST_CLK

#include "gpio.h"
#include "system.h"

volatile uint8_t led1_enabled = 0;
volatile uint8_t led2_enabled = 0;
volatile uint8_t led3_enabled = 0;
volatile uint8_t led4_enabled = 0;


/*=====================================================================================
init_GPIO: Инициализация портов ввода-вывода для управления светодиодами на PORTB.
=====================================================================================*/
void init_GPIO(void)
{
    // --- 1. ИНИЦИАЛИЗАЦИЯ ТАКТИРОВАНИЯ ПЕРИФЕРИИ ---
    // Включаем тактирование для UART2 и портов D (для UART) и B (для светодиода)
    RST_CLK_PCLKcmd(RST_CLK_PCLK_PORTB, ENABLE);


    // --- 3. НАСТРОЙКА ПОРТОВ (GPIO) ---
    // Светодиод на PORTB, пин 3
    PORT_InitTypeDef PORT_InitStructure_LED;
    PORT_StructInit(&PORT_InitStructure_LED);
    PORT_InitStructure_LED.PORT_Pin   = LED_1|LED_2|LED_3|LED_4;
    PORT_InitStructure_LED.PORT_OE    = PORT_OE_OUT;      // Выход
    PORT_InitStructure_LED.PORT_FUNC  = PORT_FUNC_PORT;   // Функция - GPIO
    PORT_InitStructure_LED.PORT_MODE  = PORT_MODE_DIGITAL;
    PORT_InitStructure_LED.PORT_SPEED = PORT_SPEED_SLOW;
    PORT_Init(LED_PORT, &PORT_InitStructure_LED);
}

void startup_animation(void)
{
    LED_OFF(LED_ALL);
    delay_ms(100);

    for (int i = 0; i < 3; i++)
    {
        LED_ON(LED_1);
        delay_ms(70);
        LED_ON(LED_2);
        delay_ms(70);
        LED_ON(LED_3);
        delay_ms(70);
        LED_ON(LED_4);
        delay_ms(70);

        LED_OFF(LED_ALL);
        delay_ms(150);
    }
}

void set_led_state(uint8_t led_num, uint8_t state)
{
    uint32_t pin;

    switch (led_num)
    {
    case 1:
        pin = LED_1;
        break;
    case 2:
        pin = LED_2;
        break;
    case 3:
        pin = LED_3;
        break;
    case 4:
        pin = LED_4;
        break;
    default:
        return;
    }

    if (state)
        LED_ON(pin);
    else
        LED_OFF(pin);
}

void connect_animation(void)
{
    for (int i = 0; i < 3; i++)
    {
        LED_ON(LED_ALL);
        delay_ms(200);
        LED_OFF(LED_ALL);
        delay_ms(200);
    }
}