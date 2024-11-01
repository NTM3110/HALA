/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "stdint.h"
#include <stdio.h>
#include "stdlib.h"
#include "string.h"
#include "math.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct {
	int hour;
	int min;
	int sec;
}TIME;

typedef struct {
	float latitude;
	char NS;
	float longitude;
	char EW;
}LOCATION;

typedef struct {
	int Day;
	int Mon;
	int Yr;
}DATE;

typedef struct {
	TIME tim;
	DATE date;
	float speed;
	float course;
	int isValid;
	LOCATION lcation;
}RMCSTRUCT;
typedef struct 
{
  volatile uint8_t* buffer;
  uint16_t size;
  volatile uint8_t* tailPtr;
  DMA_HandleTypeDef* dmaHandle;
} RingBufferDmaU8_TypeDef;


RingBufferDmaU8_TypeDef rs232Ext2RxDMARing;
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;
uint32_t avlMaxDMABufferUsage = 0;

uint8_t gsvSentence[2048];
#define READLOG_BLOCK_BUFFER_LENGHT  2048
uint8_t taxBuffer[128];

RMCSTRUCT rmc;
#define GMT 		000

int isRMCExist = 0;
int inx = 0;
int hr=0,min=0,day=0,mon=0,yr=0;
int daychange = 0;

////////////////////////

void RingBufferDmaU8_initUSARTRx(RingBufferDmaU8_TypeDef* ring, UART_HandleTypeDef* husart, uint8_t* buffer, uint16_t size) // cai dat dma
{
  ring->buffer = buffer;
  ring->size = size;
  ring->tailPtr = buffer;
  ring->dmaHandle = husart->hdmarx;
  HAL_UART_Receive_DMA(husart, buffer, size);
}


#ifdef USING_CCMRAM
__attribute__((section("ccmram")))
#endif
uint16_t RingBufferDmaU8_available(RingBufferDmaU8_TypeDef* ring) // kiem tra trang thai su dung dma
{
#ifdef __HAL_DMA_GET_COUNTER
  uint32_t leftToTransfer = __HAL_DMA_GET_COUNTER(ring->dmaHandle);
#else
  uint32_t leftToTransfer = ring->dmaHandle->Instance->CNDTR;
#endif
  volatile uint8_t const* head = ring->buffer + ring->size - leftToTransfer;
  volatile uint8_t const* tail = ring->tailPtr;
  if (head >= tail) {
    return head - tail;
  } else {
    return head - tail + ring->size;
  }
}

void rs232Ext2_InitializeRxDMA(void)// ham khoi tao lai DMA
{
	HAL_StatusTypeDef ret = HAL_UART_Abort(&huart2);
	if(ret != HAL_OK)
	{
		Error_Handler();			
	}		
	HAL_Delay(50);	//	50 is OK
	//memset(gnssDmaRingBufferMemory, 0x20, sizeof(gnssDmaRingBufferMemory));	// insert buffer with space character	
	RingBufferDmaU8_initUSARTRx(&rs232Ext2RxDMARing, &huart2, gsvSentence, READLOG_BLOCK_BUFFER_LENGHT);
}

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
uint8_t message1[] = "Hello from Task 1\n";
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
void checkDecode(RMCSTRUCT* rmc){
	char long_str[10];
	sprintf(long_str, "%.4f", rmc->lcation.longitude);
	HAL_UART_Transmit(&huart2, (uint8_t*) "Longtitude: ", strlen("Longtitude: "), 1000);
	HAL_UART_Transmit(&huart2, (uint8_t*)long_str, 10,1000); 
	HAL_UART_Transmit(&huart2, (uint8_t*) "\n", strlen("\n"), 1000);
	char lat_str[10];
	sprintf(lat_str, "%.4f", rmc->lcation.latitude);
	HAL_UART_Transmit(&huart2, (uint8_t*) "Latitude: ", strlen("Latitude: "), 1000);
	HAL_UART_Transmit(&huart2, (uint8_t*)lat_str, 10,1000); 
	HAL_UART_Transmit(&huart2, (uint8_t*) "\n", strlen("\n"), 1000);
	char speed_str[10];
	sprintf(speed_str, "%.2f", rmc->speed);
	HAL_UART_Transmit(&huart2, (uint8_t*) "Speed: ", strlen("Speed: "), 1000);
	HAL_UART_Transmit(&huart2, (uint8_t*)speed_str, 10,1000); 
	HAL_UART_Transmit(&huart2, (uint8_t*) "\n", strlen("\n"), 1000);
	char course_str[10];
	sprintf(course_str, "%.2f", rmc->course);
	HAL_UART_Transmit(&huart2, (uint8_t*) "Course: ", strlen("Course: "), 1000);
	HAL_UART_Transmit(&huart2, (uint8_t*)course_str, 4,1000); 
	HAL_UART_Transmit(&huart2, (uint8_t*) "\n", strlen("\n"), 1000);
	char year_str[2];
	sprintf(year_str, "%d", rmc->date.Yr);
	char month_str[2];
	sprintf(month_str, "%d", rmc->date.Mon);
	char day_str[2];
	sprintf(day_str, "%d", rmc->date.Day);
	HAL_UART_Transmit(&huart2, (uint8_t*) "DATE: ", strlen("DATE: "), 1000);
	HAL_UART_Transmit(&huart2, (uint8_t*)day_str, 2,1000); 
	HAL_UART_Transmit(&huart2, (uint8_t*)month_str, 2,1000); 
	HAL_UART_Transmit(&huart2, (uint8_t*)year_str, 2,1000); 
	HAL_UART_Transmit(&huart2, (uint8_t*) "\n", strlen("\n"), 1000);\
	char hour_str[2];
	sprintf(hour_str, "%d", rmc->tim.hour);
	char minute_str[2];
	sprintf(minute_str, "%d", rmc->tim.min);
	char second_str[2];
	sprintf(second_str, "%d", rmc->tim.sec);
	HAL_UART_Transmit(&huart2, (uint8_t*) "TIME: ", strlen("TIME: "), 1000);
	HAL_UART_Transmit(&huart2, (uint8_t*)hour_str, 2,1000); 
	HAL_UART_Transmit(&huart2, (uint8_t*)minute_str, 2,1000); 
	HAL_UART_Transmit(&huart2, (uint8_t*)second_str, 2,1000); 
	HAL_UART_Transmit(&huart2, (uint8_t*) "\n", strlen("\n"), 1000);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	uint16_t j = 1,k=0,cnt=0,check=0;	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */
	HAL_UART_Receive_DMA(&huart2, gsvSentence, READLOG_BLOCK_BUFFER_LENGHT);
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_Delay(1000);
	for(size_t i = 0; i < 2048; i++){
		if (gsvSentence[i] == '$' && gsvSentence[i+1] == 'G' && gsvSentence[i+2] == 'N' && gsvSentence[i+3] == 'R' && gsvSentence[i+4] == 'M' && gsvSentence[i+5] == 'C') isRMCExist = 1;
	}
	if (isRMCExist == 1){
		HAL_UART_Transmit(&huart2, (uint8_t*) "RMC EXIST\n", strlen("RMC EXIST\n"), 1000);
		inx = 0;
		char buffer[12];
		int i = 0;
		while (gsvSentence[inx] != ',') inx++;  // 1st,
		inx++;
		while (gsvSentence[inx] != ',')
		{
		 	buffer[i] = gsvSentence[inx];
		 	i++;
		 	inx++;
		 }
		
		 hr = (atoi(buffer)/10000) + GMT/100;   // get the hours from the 6 digit number

		 min = ((atoi(buffer)/100)%100) + GMT%100;  // get the minutes from the 6 digit number
		
		 
		
		 // Store the time in the RMC structure
		 rmc.tim.hour = hr;
		 rmc.tim.min = min;
		 rmc.tim.sec = atoi(buffer)%100;
		 // After time ,
		inx++;
		if (gsvSentence[inx] == 'A')  // Here 'A' Indicates the data is valid, and 'V' indicates invalid data
		{
			rmc.isValid = 1;
		}
		else
		{
			rmc.isValid = 0;
			return 1;
		}
		//Get latitude
		 inx++;
		 inx++;
		 memset(buffer, 0x00, 12);
		 i = 0;
		 while (gsvSentence[inx] != ','){
		 	buffer[i] = gsvSentence[inx];
		 	i++;
		 	inx++;
		 }
		// if (strlen(buffer) < 6) return 2;  // If the buffer length is not appropriate, return error
		 int16_t num = (atoi(buffer));   // change the buffer to the number. It will only convert upto decimal
		 int j = 0;
		 while (buffer[j] != '.') j++;   // Figure out how many digits before the decimal
		 j++;
		 int declen = (strlen(buffer))-j;  // calculate the number of digit after decimal
		 int dec = atoi ((char *) buffer+j);  // conver the decimal part a a separate number
		 float lat = (num/100.0) + (dec/pow(10, (declen+2)));  // 1234.56789 = 12.3456789
		 rmc.lcation.latitude = lat;  // save the lattitude data into the strucure
		 inx++; 
		// rmc.lcation.NS = gsvSentence[inx];  // save the N/S into the structure


	// /***********************  GET LONGITUDE **********************/
		inx++;  // ',' after NS character
		inx++;  // Reach the first number in the longitude
		memset(buffer, '\0', 12);
		i=0;
		while (gsvSentence[inx] != ',') 
		{
			buffer[i] = gsvSentence[inx];
			i++;
			inx++;
		}
		num = (atoi(buffer));  // change the buffer to the number. It will only convert upto decimal
		j = 0;
		while (buffer[j] != '.') j++;  // Figure out how many digits before the decimal
		j++;
		declen = (strlen(buffer))-j;  // calculate the number of digit after decimal
		dec = atoi ((char *) buffer+j);  // conver the decimal part a a separate number
		lat = (num/100.0) + (dec/pow(10, (declen+2)));  // 1234.56789 = 12.3456789
		rmc.lcation.longitude = lat;  // save the longitude data into the strucure
		inx++;
		rmc.lcation.EW = gsvSentence[inx];
		
		while (gsvSentence[inx] != ',') inx++;  // after EW ,

		// Get Speed
		inx++;
		i=0;
		memset(buffer, '\0', 12);
		 while (gsvSentence[inx] != ',')
		{
			buffer[i] = gsvSentence[inx];
			i++;
		 	inx++;
		}

		if (strlen (buffer) > 0){          // if the speed have some data
			int16_t num = (atoi(buffer));  // convert the data into the number
			int j = 0;
			while (buffer[j] != '.') j++;   // same as above
			j++;
			int declen = (strlen(buffer))-j;
			int dec = atoi ((char *) buffer+j);
			float lat = num + (dec/pow(10, (declen)));
			rmc.speed = lat;
		 }
		else rmc.speed = 0;

		//Get Course
		inx++;
		i=0;
		memset(buffer, '\0', 12);
		while (gsvSentence[inx] != ',')
		{
			buffer[i] = gsvSentence[inx];
			i++;
			inx++;
		}

		if (strlen (buffer) > 0){  // if the course have some data
			int16_t num = (atoi(buffer));   // convert the course data into the number
			int j = 0;
			while (buffer[j] != '.') j++;   // same as above
			j++;
			int declen = (strlen(buffer))-j;
			int dec = atoi ((char *) buffer+j);
			float lat = num + (dec/pow(10, (declen)));
			rmc.course = lat;
		}
		else
		{
			rmc.course = 0;
		}

		//Get Date
		inx++;
		i=0;
		memset(buffer, '\0', 12);
		while (gsvSentence[inx] != ',')
		{
			buffer[i] = gsvSentence[inx];
			i++;
			inx++;
		}

		// Date in the format 280222
		day = atoi(buffer)/10000;  // extract 28
		mon = (atoi(buffer)/100)%100;  // extract 02
		yr = atoi(buffer)%100;  // extract 22

		day = day+daychange;   // correction due to GMT shift

		// save the data into the structure
		rmc.date.Day = day;
		rmc.date.Mon = mon;
		rmc.date.Yr = yr;
		checkDecode(&rmc);
	}
	HAL_UART_Transmit(&huart2, message1, sizeof(message1), 100);
	HAL_UART_Transmit(&huart2, gsvSentence, 100, 1000);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_Delay(1000);
	if(isRMCExist == 1){
		memset(gsvSentence, 0x00, 2048);
		rs232Ext2_InitializeRxDMA();
		isRMCExist = 0;
	}
		
  /* USER CODE END 3 */
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_I2C_SPI_GPIO_Port, CS_I2C_SPI_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : CS_I2C_SPI_Pin */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_I2C_SPI_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_PowerSwitchOn_Pin */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(OTG_FS_PowerSwitchOn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PDM_OUT_Pin */
  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : I2S3_WS_Pin */
  GPIO_InitStruct.Pin = I2S3_WS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(I2S3_WS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI1_SCK_Pin SPI1_MISO_Pin SPI1_MOSI_Pin */
  GPIO_InitStruct.Pin = SPI1_SCK_Pin|SPI1_MISO_Pin|SPI1_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK_IN_Pin */
  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                           Audio_RST_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : I2S3_MCK_Pin I2S3_SCK_Pin I2S3_SD_Pin */
  GPIO_InitStruct.Pin = I2S3_MCK_Pin|I2S3_SCK_Pin|I2S3_SD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : VBUS_FS_Pin */
  GPIO_InitStruct.Pin = VBUS_FS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VBUS_FS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OTG_FS_ID_Pin OTG_FS_DM_Pin OTG_FS_DP_Pin */
  GPIO_InitStruct.Pin = OTG_FS_ID_Pin|OTG_FS_DM_Pin|OTG_FS_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG_FS;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Audio_SCL_Pin Audio_SDA_Pin */
  GPIO_InitStruct.Pin = Audio_SCL_Pin|Audio_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
