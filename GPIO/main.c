#include "main.h"


void RCC_Config(){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
}

void delay(__IO uint32_t timedelay)
{
	for (int i = 0; i<timedelay;++i);
}

int main(){
	RCC_Config();
	GPIO_Config();
	while(1){
		GPIO_SetBits(GPIOD, GPIO_Pin_13);
		delay(300000);
		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		delay(300000);
	}
}