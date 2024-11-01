
# GPIO

## B1: Cấp xung cho GPIO

```bash
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
```
Enable GPIOD clock source. Reference Manual Section 6.3.10
## B2: Cấu hình chế độ hoạt động cho ngoại vi
```bash
  void GPIO_Config(){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
	
}
```
Configure GPIO Pin, clock speed, mode (output), output type (Push Pull), NO pull up or pull down resistor for GPIO. 

## B3: Using ODR  register to control signal from GPIO
```bash
GPIO_SetBits(GPIOD, GPIO_Pin_13);
```
Set only bit 13 of LED3_PIN to 1.
```bash
GPIO_ResetBits(GPIOD, GPIO_Pin_13);
```
Set only bit 13 of LED3_PIN to 0.


## External requirement.
Configure GPIOA0 TO BE Input mode
```bash
	GPIO_InitTypeDef GPIO_InitStructInput;
	GPIO_InitStructInput.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructInput.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructInput.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructInput);
```
READ GPIOA0
```bash
   if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)){
			GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
    }
```
Set up GPIOA0 to be GPIO Input


