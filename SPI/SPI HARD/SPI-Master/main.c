#include "main.h"

#define SPI_MISO GPIO_Pin_1
#define SPI_MOSI GPIO_Pin_2
#define SPI_CS GPIO_Pin_3
#define SPI_SCK GPIO_Pin_4
#define SPI_GPIO GPIOC
#define SPI_RCC RCC_AHB1Periph_GPIOC

void RCC_Config(){
	RCC_AHB1PeriphClockCmd(SPI_RCC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
}


void SPI_Init_Soft(){
	GPIO_WriteBit(SPI_GPIO, SPI_SCK, 0);
	GPIO_WriteBit(SPI_GPIO, SPI_CS, 1);
	GPIO_WriteBit(SPI_GPIO, SPI_MISO, 0);
	GPIO_WriteBit(SPI_GPIO, SPI_MOSI, 0);
}

void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = SPI_SCK | SPI_MOSI | SPI_CS;
	//GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
	//GPIO_Init(GPIOD, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = SPI_MISO;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
}

void TIM_Config(){
	TIM_TimeBaseInitTypeDef TIM_InitStruct;
	
	// Count up 1 every 0.1ms
	TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//72MHz
	TIM_InitStruct.TIM_Prescaler = 1600 - 1;					
	TIM_InitStruct.TIM_Period = 0xFFFF;
	TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
	TIM_Cmd(TIM2, ENABLE);	//Cho ph?p timer ho?t d?ngd
}

void delay_ms(uint32_t timedelay)  //	delay 1ms = 1
{
	TIM_SetCounter(TIM2,0);
	while(TIM_GetCounter(TIM2) < timedelay * 10){}
}

void Clock(){
	GPIO_WriteBit(SPI_GPIO, SPI_SCK, Bit_SET);
	delay_ms(4);
	GPIO_WriteBit(SPI_GPIO, SPI_SCK, Bit_RESET);
	delay_ms(4);
}

void SPI_Master_Transmit(uint8_t u8Data){	//0b10010000
	uint8_t u8Mask = 0x80;	// 0b10000000
	uint8_t tempData;
	GPIO_WriteBit(SPI_GPIO, SPI_CS, Bit_RESET);
	delay_ms(1);
	for(int i = 0; i < 8; i++){
		tempData = u8Data & u8Mask;
		if(tempData){
			GPIO_WriteBit(SPI_GPIO, SPI_MOSI, Bit_SET);
			delay_ms(1);
		} else{
			GPIO_WriteBit(SPI_GPIO, SPI_MOSI, Bit_RESET);
			delay_ms(1);
		}
		u8Data = u8Data << 1;
		Clock();
	}
	GPIO_WriteBit(SPI_GPIO, SPI_CS, Bit_SET);
	delay_ms(1);
}


uint8_t DataTrans[] = {'A','B','C','d','E','J','H'};//Data
int main(){
	RCC_Config();
	GPIO_Config();
	TIM_Config();
	SPI_Init_Soft();
	while(1){	
		for(int i = 0; i < 7; i++){
			//GPIO_SetBits(GPIOC, GPIO_Pin_2| GPIO_Pin_3 | GPIO_Pin_4);
			//delay_ms(1000);
			//GPIO_ResetBits(GPIOC, GPIO_Pin_2| GPIO_Pin_3| GPIO_Pin_4);
			//delay_ms(1000);
			SPI_Master_Transmit(DataTrans[i]);
			delay_ms(3000);

		}
	}
}