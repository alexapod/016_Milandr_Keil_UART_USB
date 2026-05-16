#ifndef GPIO_H
#define GPIO_H
#include "MDR32FxQI_port.h"             // Drivers:PORT
#include "MDR32FxQI_rst_clk.h"          // Drivers:RST_CLK

//#include <stdint.h>


/*=====================================================================================
Светодиоды на порту A
===Светодиоды подключены к +3.3V===
=====================================================================================*/
#define LED_PORT    MDR_PORTB
#define LED_1       PORT_Pin_0 // 
#define LED_2       PORT_Pin_1 // 
#define LED_3       PORT_Pin_2 // 
#define LED_4       PORT_Pin_3 // 



/*=====================================================================================
===Входы-выходы состояния и управления===
=====================================================================================*/

#define RESULT_PORT      MDR_PORTE
#define X3_Cont3_Result  PORT_Pin_0 // PE0 -- X3_Cont3_Result
#define X3_Cont2_Result  PORT_Pin_1 // PE1 -- X3_Cont2_Result
#define X3_Cont1_Result  PORT_Pin_2 // PE2 -- X3_Cont1_Result
#define SBI_BOR2_Result  PORT_Pin_3 // PE3 -- SBI_BOR2_Result
#define SBI_BOR1_Result  PORT_Pin_6 // PE6 -- SBI_BOR1_Result
#define SBI_BRM_Result   PORT_Pin_7 // PE7 -- SBI_BRM_Result

#define CONTROL_PORT     MDR_PORTF
#define X3_Cont_Chk      PORT_Pin_0 // PF0 -- X3_Cont_Chk

/*=====================================================================================
Макросы для управления светодиодами. Упрощают код и делают его читаемым.
===Светодиоды подключены к +3.3V===  Обратная логика
=====================================================================================*/
#define LED_ON(led)   PORT_SetBits(LED_PORT, led) // Установить бит в регистре данных -> включить светодиод
#define LED_OFF(led)  PORT_ResetBits(LED_PORT, led) // Сбросить бит в регистре данных -> выключить светодиод

#define LED_ALL       PORT_Pin_0|PORT_Pin_1|PORT_Pin_2|PORT_Pin_3

#define LED_TOGGLE(led) do { \
    uint32_t mask = (led); \
    LED_PORT->RXTX ^= mask; \
} while(0)

// Глобальные флаги состояния светодиодов
extern volatile uint8_t led1_enabled;
extern volatile uint8_t led2_enabled;
extern volatile uint8_t led3_enabled;
extern volatile uint8_t led4_enabled;

/*=====================================================================================
Прототипы функций для работы с GPIO.
=====================================================================================*/
void init_GPIO(void);      // Инициализация портов для светодиодов
void startup_animation(void); //Анимация включения

void set_led_state(uint8_t led_num, uint8_t state);

void connect_animation(void);


#endif // GPIO_H