
# Timer

## B1: Cấp xung cho TIMER2
```bash
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
```
TIMER2 clock is from bus APB1.
Datasheet 2.2 Block Diagram.


## B2: Cấu hình chế độ hoạt động cho TIMER2
```bash
    TIM_InitStruct.TIM_ClockDivision = TIM_CKD_DIV1;	//16MHz
    TIM_InitStruct.TIM_Prescaler = 1600 - 1;					
    TIM_InitStruct.TIM_Period = 0xFFFF;
    TIM_InitStruct.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM2, &TIM_InitStruct);
    TIM_Cmd(TIM2, ENABLE);	//Cho ph?p timer ho?t d?ngd
```
Default clock frequency is 16MHz. Therefore, each clock period 1/(16*10^6). 

TIM_Prescaler defines after which clock period, counter counts up so it should be (1600 - 1) so that every 1/(16*10^6) * 16 * 10^2 = 1/10000 (s) = 0.1ms, it will count up.

We set the TIM_Period big so we can define the value of counter we want to stop.

## B3: Using TIMER2 for delay 1s
```bash
    TIM_SetCounter(TIM2,0);
	while(TIM_GetCounter(TIM2) < timedelay * 10){}
```
Set counter first to 0. Then wait until the counter reaches 10000 to have a delay of 1s.



