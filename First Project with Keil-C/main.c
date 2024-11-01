#include <stdio.h>
#include "stm32f4xx.h"

#define GPIOAEN                     (1U << 0)
#define GPIODEN 										(1U << 3)

#define PIN0                        (1U << 0)
#define PIN13                       (1U << 13)
#define PIN12												(1U << 12)
#define PIN14                       (1U << 14)

#define LED3_PIN                    PIN13
#define LED4_PIN										PIN12
#define LED5_PIN                    PIN14
#define USER_BTN_PIN                PIN0

#define __IO      volatile
void delay(__IO uint32_t timedelay)
{
	for (int i = 0; i<timedelay;++i);
}
int main(void)
{
   RCC->AHB1ENR |= GPIODEN;
   RCC->AHB1ENR |= GPIOAEN;

   //2. Set PD13 as output pin (LED3)
   GPIOD->MODER |= (1U << 26);
   GPIOD->MODER &= ~(1U <<27);

    //3. Set PD12 as output pin (LED4)
   GPIOD->MODER |= (1U << 24);
   GPIOD->MODER &= ~(1U <<25);

   GPIOD->MODER |= (1U << 28);
   GPIOD->MODER &= ~(1U << 29);

    //4. Set PA0 as input pin (USER_BTN)
   GPIOA->MODER &= ~(1U << 0);
   GPIOA->MODER &= ~(1U << 1);
	 int i;

   while (1){

	
		 GPIOD->ODR |= LED3_PIN;
		 GPIOD->ODR |= LED5_PIN;
		 delay(3000000);
	 	
		 GPIOD->ODR &= ~LED3_PIN;
		 GPIOD->ODR &= ~LED5_PIN;
		 delay(3000000);
	}
}
