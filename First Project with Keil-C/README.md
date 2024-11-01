
# First Project with KEIL-C

## B1: Cấp xung cho GPIO

```bash
  RCC->AHB1ENR |= GPIODEN;
```
Enable bit 3 in RCC_AHB1ENR to enable GPIOD clock source. Reference Manual Section 6.3.10
## B2: Cấu hình chế độ hoạt động cho ngoại vi
```bash
  GPIOD->OSPEEDR |= (1U << 27);
  GPIOC->OSPEEDR &= ~ (1U << 26);
```
Enable bit 26 and 27 (10) for HIGH_SPEED which is 50MHz( 00: Input (reset state) 
01: General purpose output mode 
10: Alternate function mode 
11: Analog mode)

## B3: Using ODR  register to control signal from GPIO
```bash
GPIOD->ODR |= LED3_PIN;
```
Set only bit 13 of LED3_PIN to 1.
```bash
GPIOD->ODR &= ~LED3_PIN;
```
Set only bit 13 of LED3_PIN to 0.


## External requirement.
Configure GPIOA0 TO BE Input mode
```bash
   GPIOA->MODER &= ~(1U << 0);
   GPIOA->MODER &= ~(1U << 1);
```
Set up GPIOA0 to be GPIO Input


