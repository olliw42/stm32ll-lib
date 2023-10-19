//*******************************************************
// Copyright (c) OlliW, OlliW42, www.olliw.eu
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// PERIPHERALS standard library
//*******************************************************
#ifndef STDSTM32_PERIPHERALS_H
#define STDSTM32_PERIPHERALS_H
#ifdef __cplusplus
extern "C" {
#endif


//-------------------------------------------------------
// NVIC
//-------------------------------------------------------

static inline void nvic_irq_enable_w_priority(IRQn_Type IRQn, uint32_t priority)
{
  // we do not do
  // NVIC_SetPriority(IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 13, 0));
  // as we want to use 0..15 as consequtive numbers
  NVIC_SetPriority(IRQn, priority);
  NVIC_EnableIRQ(IRQn); // works only for IRQn >= 0
}


//-------------------------------------------------------
// RCC
//-------------------------------------------------------

void rcc_init_afio(void)
{
#ifdef STM32F1
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_AFIO);
#endif
}


void rcc_init_gpio(GPIO_TypeDef* GPIOx)
{
#if defined STM32F1
  if (GPIOx == GPIOA) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA); }
  if (GPIOx == GPIOB) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB); }
  if (GPIOx == GPIOC) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOC); }
#if defined(GPIOD)
  if (GPIOx == GPIOD) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOD); }
#endif  
#if defined(GPIOE)
  if (GPIOx == GPIOE) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOE); }
#endif  

#elif defined STM32F3
  if (GPIOx == GPIOA) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA); }
  if (GPIOx == GPIOB) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB); }
  if (GPIOx == GPIOC) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC); }
#if defined(GPIOD)
  if (GPIOx == GPIOD) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD); }
#endif
#if defined(GPIOE)
  if (GPIOx == GPIOE) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE); }
#endif
#if defined(GPIOF)
  if (GPIOx == GPIOF) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF); }
#endif
#if defined(GPIOG)
  if (GPIOx == GPIOG) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG); }
#endif

#elif defined STM32F7
  if (GPIOx == GPIOA) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA); }
  if (GPIOx == GPIOB) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB); }
  if (GPIOx == GPIOC) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC); }
#if defined(GPIOD)
  if (GPIOx == GPIOD) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD); }
#endif
#if defined(GPIOE)
  if (GPIOx == GPIOE) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE); }
#endif
#if defined(GPIOF)
  if (GPIOx == GPIOF) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF); }
#endif
#if defined(GPIOG)
  if (GPIOx == GPIOG) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOG); }
#endif

#elif defined STM32G4 || defined STM32L4
  if (GPIOx == GPIOA) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA); }
  if (GPIOx == GPIOB) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB); }
  if (GPIOx == GPIOC) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC); }
#if defined(GPIOD)
  if (GPIOx == GPIOD) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD); }
#endif  
#if defined(GPIOE)
  if (GPIOx == GPIOE) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOE); }
#endif  
#if defined(GPIOF)
  if (GPIOx == GPIOF) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOF); }
#endif
#if defined(GPIOG)
  if (GPIOx == GPIOG) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOG); }
#endif

#elif defined STM32WL
  if (GPIOx == GPIOA) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA); }
  if (GPIOx == GPIOB) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB); }
  if (GPIOx == GPIOC) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC); }
  if (GPIOx == GPIOH) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH); }

#elif defined STM32F0
  if (GPIOx == GPIOA) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA); }
  if (GPIOx == GPIOB) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB); }
  if (GPIOx == GPIOC) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOC); }
#if defined(GPIOD)
  if (GPIOx == GPIOD) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOD); }
#endif
#if defined(GPIOE)
  if (GPIOx == GPIOE) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOE); }
#endif
  if (GPIOx == GPIOF) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOF); }

#endif
}


void rcc_init_tim(TIM_TypeDef* TIMx)
{
#if defined(TIM1)
  #ifndef STM32F0
  if (TIMx == TIM1) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1); }
  #else
  if (TIMx == TIM1) { LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM1); }
  #endif
#endif
#if defined(TIM2)
  if (TIMx == TIM2) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2); }
#endif
#if defined(TIM3)
  if (TIMx == TIM3) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3); }
#endif
#if defined(TIM4)
  if (TIMx == TIM4) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4); }
#endif
#if defined(TIM5)
  if (TIMx == TIM5) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM5); }
#endif
#if defined(TIM6)
  if (TIMx == TIM6) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6); }
#endif
#if defined(TIM7)
  if (TIMx == TIM7) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM7); }
#endif
#if defined(TIM8)
  if (TIMx == TIM8) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM8); }
#endif
#if defined(TIM15)
  #ifndef STM32F0
  if (TIMx == TIM15) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM15); }
  #else
  if (TIMx == TIM15) { LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM15); }
  #endif
#endif
#if defined(TIM16)
  #ifndef STM32F0
  if (TIMx == TIM16) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM16); }
  #else
  if (TIMx == TIM16) { LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM16); }
  #endif
#endif
#if defined(TIM17)
  #ifndef STM32F0
  if (TIMx == TIM17) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM17); }
  #else
  if (TIMx == TIM17) { LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM17); }
  #endif
#endif
}


#if defined LPTIM1 || defined LPTIM2 || defined LPTIM3

void rcc_init_lptim(LPTIM_TypeDef* LPTIMx)
{
#if defined(LPTIM1)
  if (LPTIMx == LPTIM1) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPTIM1); }
#endif
#if defined(LPTIM2)
  if (LPTIMx == LPTIM2) { LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPTIM2); }
#endif
#if defined(LPTIM3)
  if (LPTIMx == LPTIM3) { LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPTIM3); }
#endif
}

#endif


void rcc_init_uart(USART_TypeDef *USARTx)
{
#if defined(USART1)
  #ifndef STM32F0
  if (USARTx == USART1) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1); }
  #else
  if (USARTx == USART1) { LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1); }
  #endif
#endif
#if defined(USART2)
  if (USARTx == USART2) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2); }
#endif
#if defined(USART3)
  if (USARTx == USART3) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3); }
#endif
#if defined(UART4)
  if (USARTx == UART4)  { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4); }
#endif
#if defined(UART5)
  if (USARTx == UART5)  { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART5); }
#endif
#if defined(USART6)
  if (USARTx == USART6) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART6); }
#endif
#if defined(UART7)
  if (USARTx == UART7)  { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART7); }
#endif
#if defined(UART8)
  if (USARTx == UART8)  { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART8); }
#endif

#if defined(LPUART1)
  if (USARTx == LPUART1) { LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_LPUART1); }
#endif
}


void rcc_init_spi(SPI_TypeDef *SPIx)
{
#if defined(SPI1)
  #ifndef STM32F0
  if (SPIx == SPI1) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI1); }
  #else
  if (SPIx == SPI1) { LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SPI1); }
  #endif
#endif
#if defined(SPI2)
  if (SPIx == SPI2) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2); }
#endif
#if defined(SPI3)
  if (SPIx == SPI3) { LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3); }
#endif

#if defined(SUBGHZSPI)
  if (SPIx == SUBGHZSPI) { LL_APB3_GRP1_EnableClock(LL_APB3_GRP1_PERIPH_SUBGHZSPI); }
#endif
}


void rcc_init_adc(ADC_TypeDef *ADCx)
{
#if defined STM32F1 || defined STM32F7
#if defined(ADC1)
  if (ADCx == ADC1) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1); }
#endif
#if defined(ADC2)
  if (ADCx == ADC2) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC2); }
#endif
#if defined(ADC3)
  if (ADCx == ADC3) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC3); }
#endif

#elif defined STM32F3
#if defined(ADC1)
  #warning F3 ADC1 still TODO
#endif
#if defined(ADC2)
  if (ADCx == ADC2) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC12); }
#endif
#if defined(ADC3)
  if (ADCx == ADC3) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC34); }
#endif
#if defined(ADC4)
  if (ADCx == ADC4) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC34); }
#endif

#elif defined STM32G4
#if defined(ADC1)
  if (ADCx == ADC1) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12); }
#endif
#if defined(ADC2)
  if (ADCx == ADC2) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC12); }
#endif
#if defined(ADC3)
  if (ADCx == ADC3) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC345); }
#endif

#elif defined STM32L4
#if defined(ADC1)
  if (ADCx == ADC1) { LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_ADC); }
#endif

#elif defined STM32WL
#if defined(ADC)
  if (ADCx == ADC) { LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC); }
#endif

#elif defined STM32F0
#if defined(ADC1)
  if (ADCx == ADC1) { LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1); }
#endif

#endif
}


void rcc_init_dma(DMA_TypeDef *DMAx)
{
#if defined(LL_AHB1_GRP1_PERIPH_DMAMUX1)
  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMAMUX1);
#endif

#if defined(DMA1)
  if (DMAx == DMA1) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1); }
#endif
#if defined(DMA2)
  if (DMAx == DMA2) { LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA2); }
#endif
}


//-------------------------------------------------------
// GPIO
//-------------------------------------------------------

#if defined(GPIOA)
#define IO_PA0   GPIOA, LL_GPIO_PIN_0
#define IO_PA1   GPIOA, LL_GPIO_PIN_1
#define IO_PA2   GPIOA, LL_GPIO_PIN_2
#define IO_PA3   GPIOA, LL_GPIO_PIN_3
#define IO_PA4   GPIOA, LL_GPIO_PIN_4
#define IO_PA5   GPIOA, LL_GPIO_PIN_5
#define IO_PA6   GPIOA, LL_GPIO_PIN_6
#define IO_PA7   GPIOA, LL_GPIO_PIN_7

#define IO_PA8   GPIOA, LL_GPIO_PIN_8
#define IO_PA9   GPIOA, LL_GPIO_PIN_9
#define IO_PA10  GPIOA, LL_GPIO_PIN_10
#define IO_PA11  GPIOA, LL_GPIO_PIN_11
#define IO_PA12  GPIOA, LL_GPIO_PIN_12
#define IO_PA13  GPIOA, LL_GPIO_PIN_13
#define IO_PA14  GPIOA, LL_GPIO_PIN_14
#define IO_PA15  GPIOA, LL_GPIO_PIN_15
#endif

#if defined(GPIOB)
#define IO_PB0   GPIOB, LL_GPIO_PIN_0
#define IO_PB1   GPIOB, LL_GPIO_PIN_1
#define IO_PB2   GPIOB, LL_GPIO_PIN_2
#define IO_PB3   GPIOB, LL_GPIO_PIN_3
#define IO_PB4   GPIOB, LL_GPIO_PIN_4
#define IO_PB5   GPIOB, LL_GPIO_PIN_5
#define IO_PB6   GPIOB, LL_GPIO_PIN_6
#define IO_PB7   GPIOB, LL_GPIO_PIN_7

#define IO_PB8   GPIOB, LL_GPIO_PIN_8
#define IO_PB9   GPIOB, LL_GPIO_PIN_9
#define IO_PB10  GPIOB, LL_GPIO_PIN_10
#define IO_PB11  GPIOB, LL_GPIO_PIN_11
#define IO_PB12  GPIOB, LL_GPIO_PIN_12
#define IO_PB13  GPIOB, LL_GPIO_PIN_13
#define IO_PB14  GPIOB, LL_GPIO_PIN_14
#define IO_PB15  GPIOB, LL_GPIO_PIN_15
#endif

#if defined(GPIOC)
#define IO_PC0   GPIOC, LL_GPIO_PIN_0
#define IO_PC1   GPIOC, LL_GPIO_PIN_1
#define IO_PC2   GPIOC, LL_GPIO_PIN_2
#define IO_PC3   GPIOC, LL_GPIO_PIN_3
#define IO_PC4   GPIOC, LL_GPIO_PIN_4
#define IO_PC5   GPIOC, LL_GPIO_PIN_5
#define IO_PC6   GPIOC, LL_GPIO_PIN_6
#define IO_PC7   GPIOC, LL_GPIO_PIN_7

#define IO_PC8   GPIOC, LL_GPIO_PIN_8
#define IO_PC9   GPIOC, LL_GPIO_PIN_9
#define IO_PC10  GPIOC, LL_GPIO_PIN_10
#define IO_PC11  GPIOC, LL_GPIO_PIN_11
#define IO_PC12  GPIOC, LL_GPIO_PIN_12
#define IO_PC13  GPIOC, LL_GPIO_PIN_13
#define IO_PC14  GPIOC, LL_GPIO_PIN_14
#define IO_PC15  GPIOC, LL_GPIO_PIN_15
#endif

#if defined(GPIOD)
#define IO_PD0   GPIOD, LL_GPIO_PIN_0
#define IO_PD1   GPIOD, LL_GPIO_PIN_1
#define IO_PD2   GPIOD, LL_GPIO_PIN_2
#define IO_PD5   GPIOD, LL_GPIO_PIN_5
#define IO_PD6   GPIOD, LL_GPIO_PIN_6
#define IO_PD7   GPIOD, LL_GPIO_PIN_7

#define IO_PD8   GPIOD, LL_GPIO_PIN_8
#define IO_PD9   GPIOD, LL_GPIO_PIN_9
#define IO_PD10  GPIOD, LL_GPIO_PIN_10
#define IO_PD11  GPIOD, LL_GPIO_PIN_11
#define IO_PD12  GPIOD, LL_GPIO_PIN_12
#define IO_PD13  GPIOD, LL_GPIO_PIN_13
#define IO_PD14  GPIOD, LL_GPIO_PIN_14
#define IO_PD15  GPIOD, LL_GPIO_PIN_15
#endif

#if defined(GPIOE)
#define IO_PE0   GPIOE, LL_GPIO_PIN_0
#define IO_PE1   GPIOE, LL_GPIO_PIN_1
#define IO_PE7   GPIOE, LL_GPIO_PIN_7
#define IO_PE8   GPIOE, LL_GPIO_PIN_8
#define IO_PE9   GPIOE, LL_GPIO_PIN_9
#endif

#if defined(GPIOF)
#define IO_PF3   GPIOF, LL_GPIO_PIN_3
#define IO_PF5   GPIOF, LL_GPIO_PIN_5
#define IO_PF6   GPIOF, LL_GPIO_PIN_6
#define IO_PF7   GPIOF, LL_GPIO_PIN_7
#endif

#if defined(GPIOG)
#define IO_PG0   GPIOG, LL_GPIO_PIN_0
#define IO_PG9   GPIOG, LL_GPIO_PIN_9
#define IO_PG14  GPIOG, LL_GPIO_PIN_14
#endif

#if defined(GPIOH)
#define IO_PH0   GPIOH, LL_GPIO_PIN_0
#endif


typedef enum {
  IO_MODE_Z = 0,
  IO_MODE_INPUT_ANALOG, // use for ADC
  IO_MODE_INPUT_PD,
  IO_MODE_INPUT_PU,
  IO_MODE_OUTPUT_PP,
  IO_MODE_OUTPUT_PP_LOW,
  IO_MODE_OUTPUT_PP_HIGH,
  IO_MODE_OUTPUT_OD,
  IO_MODE_OUTPUT_OD_LOW,
  IO_MODE_OUTPUT_OD_HIGH,
  IO_MODE_OUTPUT_ALTERNATE_PP,
  IO_MODE_OUTPUT_ALTERNATE_OD,
} IOMODEENUM;


typedef enum {
  IO_SPEED_DEFAULT = 0,
  IO_SPEED_SLOW,
  IO_SPEED_FAST,
  IO_SPEED_VERYFAST,
} IOSPEEDENUM;


typedef enum {
  IO_AF_DEFAULT = 0,
  IO_AF_0, IO_AF_1, IO_AF_2, IO_AF_3, IO_AF_4, IO_AF_5, IO_AF_6, IO_AF_7,
  IO_AF_8, IO_AF_9, IO_AF_10, IO_AF_11, IO_AF_12,  IO_AF_13, IO_AF_14, IO_AF_15,
} IOAFENUM;


typedef enum { // just for convenience to use in higher level code
  IO_LOW = 0,
  IO_HIGH,
} IOSTATEENUM;


GPIO_TypeDef* IO_GPIOx(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin) { return GPIOx; }
uint32_t IO_PINx(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin) { return GPIO_Pin; }


void _gpio_init(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, IOMODEENUM mode, IOSPEEDENUM speed, IOAFENUM af, uint8_t init_rcc)
{
LL_GPIO_InitTypeDef GPIO_InitStruct = {};

  GPIO_InitStruct.Pin = GPIO_Pin;

  switch (mode) {
    case IO_MODE_Z:
      GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
      break;
    case IO_MODE_INPUT_ANALOG:
      GPIO_InitStruct.Mode = LL_GPIO_MODE_ANALOG;
      break;
    case IO_MODE_INPUT_PD:
      GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
      break;
    case IO_MODE_INPUT_PU:
      GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
      GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
      break;
    case IO_MODE_OUTPUT_PP: case IO_MODE_OUTPUT_PP_LOW: case IO_MODE_OUTPUT_PP_HIGH:
      GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
      GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
      break;
    case IO_MODE_OUTPUT_OD: case IO_MODE_OUTPUT_OD_LOW: case IO_MODE_OUTPUT_OD_HIGH:
      GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
      GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
      break;
    case IO_MODE_OUTPUT_ALTERNATE_PP:
      GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
      GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
      break;
    case IO_MODE_OUTPUT_ALTERNATE_OD:
      GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
      GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
      break;
    default: while (1) {}
  }

  switch (speed) {
    case IO_SPEED_DEFAULT: GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH; break;
    case IO_SPEED_SLOW: GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW; break;
    case IO_SPEED_FAST: GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM; break;
    case IO_SPEED_VERYFAST: GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH; break;
    default: while (1) {}
  }

#if defined STM32F7 || defined STM32G4 || defined STM32F3 || defined STM32L4 || defined STM32WL || defined STM32F0
  switch (af) {
    case IO_AF_DEFAULT: GPIO_InitStruct.Alternate = LL_GPIO_AF_0; break;
    case IO_AF_0: GPIO_InitStruct.Alternate = LL_GPIO_AF_0; break;
    case IO_AF_1: GPIO_InitStruct.Alternate = LL_GPIO_AF_1; break;
    case IO_AF_2: GPIO_InitStruct.Alternate = LL_GPIO_AF_2; break;
    case IO_AF_3: GPIO_InitStruct.Alternate = LL_GPIO_AF_3; break;
    case IO_AF_4: GPIO_InitStruct.Alternate = LL_GPIO_AF_4; break;
    case IO_AF_5: GPIO_InitStruct.Alternate = LL_GPIO_AF_5; break;
    case IO_AF_6: GPIO_InitStruct.Alternate = LL_GPIO_AF_6; break;
    case IO_AF_7: GPIO_InitStruct.Alternate = LL_GPIO_AF_7; break;
#if defined LL_GPIO_AF_8 // assume that if AF_8 is available, all up to AF_15 are available too
    case IO_AF_8: GPIO_InitStruct.Alternate = LL_GPIO_AF_8; break;
    case IO_AF_9: GPIO_InitStruct.Alternate = LL_GPIO_AF_9; break;
    case IO_AF_10: GPIO_InitStruct.Alternate = LL_GPIO_AF_10; break;
    case IO_AF_11: GPIO_InitStruct.Alternate = LL_GPIO_AF_11; break;
    case IO_AF_12: GPIO_InitStruct.Alternate = LL_GPIO_AF_12; break;
    case IO_AF_13: GPIO_InitStruct.Alternate = LL_GPIO_AF_13; break;
    case IO_AF_14: GPIO_InitStruct.Alternate = LL_GPIO_AF_14; break;
    case IO_AF_15: GPIO_InitStruct.Alternate = LL_GPIO_AF_15; break;
#endif
    default: while (1) {}
  }
  if (af > IO_AF_DEFAULT) {
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  }
#endif

  if (init_rcc) rcc_init_gpio(GPIOx);

  switch (mode) {
    case IO_MODE_OUTPUT_PP_LOW: case IO_MODE_OUTPUT_OD_LOW:
      LL_GPIO_ResetOutputPin(GPIOx, GPIO_Pin);
      break;
    case IO_MODE_OUTPUT_PP_HIGH: case IO_MODE_OUTPUT_OD_HIGH:
      LL_GPIO_SetOutputPin(GPIOx, GPIO_Pin);
      break;
    default:
      break;
  }

  LL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}


static inline void gpio_init(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, IOMODEENUM mode, IOSPEEDENUM speed)
{
  _gpio_init(GPIOx, GPIO_Pin, mode, speed, IO_AF_DEFAULT, 1);
}


static inline void gpio_change(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, IOMODEENUM mode, IOSPEEDENUM speed)
{
  _gpio_init(GPIOx, GPIO_Pin, mode, speed, IO_AF_DEFAULT, 0);
}


static inline void gpio_deinit(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, IOMODEENUM mode, IOSPEEDENUM speed)
{
  _gpio_init(GPIOx, GPIO_Pin, mode, speed, IO_AF_DEFAULT, 0);
}


static inline void gpio_init_af(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, IOMODEENUM mode, IOAFENUM af, IOSPEEDENUM speed)
{
  _gpio_init(GPIOx, GPIO_Pin, mode, speed, af, 1);
}


static inline void gpio_change_af(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin, IOMODEENUM mode, IOAFENUM af, IOSPEEDENUM speed)
{
  _gpio_init(GPIOx, GPIO_Pin, mode, speed, af, 0);
}


static inline void gpio_init_outpp(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin)
{
  _gpio_init(GPIOx, GPIO_Pin, IO_MODE_OUTPUT_PP, IO_SPEED_DEFAULT, IO_AF_DEFAULT, 1);
}


#define GPIO_INLINE_FORCED static inline __attribute__ ((always_inline))


// these should be fast
#pragma GCC push_options
#pragma GCC optimize ("O3")

GPIO_INLINE_FORCED void gpio_low(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin)
{
  LL_GPIO_ResetOutputPin(GPIOx, GPIO_Pin);
}


GPIO_INLINE_FORCED void gpio_high(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin)
{
  LL_GPIO_SetOutputPin(GPIOx, GPIO_Pin);
}


GPIO_INLINE_FORCED void gpio_toggle(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin)
{
  LL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}


GPIO_INLINE_FORCED uint16_t gpio_read_activehigh(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin)
{
  return (LL_GPIO_IsInputPinSet(GPIOx, GPIO_Pin)) ? 1 : 0;
}


GPIO_INLINE_FORCED uint16_t gpio_read_activelow(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin)
{
  return (!LL_GPIO_IsInputPinSet(GPIOx, GPIO_Pin)) ? 1 : 0;
}


GPIO_INLINE_FORCED uint16_t gpio_readoutput(GPIO_TypeDef* GPIOx, uint32_t GPIO_Pin)
{
  return (LL_GPIO_IsOutputPinSet(GPIOA, GPIO_Pin)) ? 1 : 0;
}

#pragma GCC pop_options
// end of these should be fast


//-------------------------------------------------------
// GPIO auxiliary defines
//-------------------------------------------------------
// these are here just for use in some hal's etc

#define PIN_PA0   0x000
#define PIN_PA1   0x001
#define PIN_PA2   0x002
#define PIN_PA3   0x003
#define PIN_PA4   0x004
#define PIN_PA5   0x005
#define PIN_PA6   0x006
#define PIN_PA7   0x007
#define PIN_PA8   0x008
#define PIN_PA9   0x009
#define PIN_PA10  0x010
#define PIN_PA11  0x011
#define PIN_PA12  0x012
#define PIN_PA13  0x013
#define PIN_PA14  0x014
#define PIN_PA15  0x015

#define PIN_PB0   0x100
#define PIN_PB1   0x101
#define PIN_PB2   0x102
#define PIN_PB3   0x103
#define PIN_PB4   0x104
#define PIN_PB5   0x105
#define PIN_PB6   0x106
#define PIN_PB7   0x107
#define PIN_PB8   0x108
#define PIN_PB9   0x109
#define PIN_PB10  0x110
#define PIN_PB11  0x111
#define PIN_PB12  0x112
#define PIN_PB13  0x113
#define PIN_PB14  0x114
#define PIN_PB15  0x115


//-------------------------------------------------------
// Timer
//-------------------------------------------------------

typedef enum {
  TIMER_BASE_1US = 0,
  TIMER_BASE_1MHZ = TIMER_BASE_1US,
#if defined STM32F1 || defined STM32F3 || defined STM32WL || defined STM32F0
  TIMER_BASE_8MHZ,
#endif
#if defined STM32G4 || defined STM32L4
  TIMER_BASE_10MHZ,
#endif
  TIMER_BASE_18MHZ, // this may be achieved only approximate depending on the STM32
  TIMER_BASE_10US,
  TIMER_BASE_MAX,
} TIMERBASEENUM;


// helper to correct for timer base clock
uint16_t _tim_devider(TIM_TypeDef* TIMx)
{
#if defined STM32F1 // all timer run on 72 MHz
  return 1;

#elif defined STM32F3 // all timer run on 72 MHz
  return 1;

#elif defined STM32F7
// for F722, timers on APB1 run with 108 MHz and those on APB2 run with 216 MHz
// TIM1, TIM8 are on APB2 and run with 216 MHz
// the other run with 108 MHz
#if defined(TIM2)
  if (TIMx == TIM2) return 2;
#endif
#if defined(TIM3)
  if (TIMx == TIM3) return 2;
#endif
#if defined(TIM4)
  if (TIMx == TIM4) return 2;
#endif
#if defined(TIM5)
  if (TIMx == TIM5) return 2;
#endif
  return 1;

#elif defined STM32G4 // all timer run on 170 MHz
  return 1;

#elif defined STM32L4 // all timer run on 80 MHz
  return 1;

#elif defined STM32WL || defined STM32F0 // all timer run on 48 MHz
  return 1;

#endif
}


// do not enable timer
void tim_config_up(TIM_TypeDef* TIMx, uint32_t period, TIMERBASEENUM rate)
{
LL_TIM_InitTypeDef TIM_InitStruct = {};

  rcc_init_tim(TIMx);

  // figure it out
  uint16_t prescaler = 0;
  switch (rate) {
    case TIMER_BASE_18MHZ:
      prescaler = (SystemCoreClock/18000000);
      prescaler /= _tim_devider(TIMx);
      break;
    case TIMER_BASE_1US:
      prescaler = (SystemCoreClock/1000000);
      prescaler /= _tim_devider(TIMx);
      break;
#if defined STM32F1 || defined STM32F3 || defined STM32WL || defined STM32F0
    case TIMER_BASE_8MHZ:
      prescaler = (SystemCoreClock/1000000)/8;
      prescaler /= _tim_devider(TIMx);
      break;
#endif
#if defined STM32G4 || defined STM32L4
    case TIMER_BASE_10MHZ:
      prescaler = (SystemCoreClock/1000000)/10;
      prescaler /= _tim_devider(TIMx);
      break;
#endif
    case TIMER_BASE_10US:
      prescaler = (SystemCoreClock/1000000)*10;
      prescaler /= _tim_devider(TIMx);
      break;
    case TIMER_BASE_MAX:
      prescaler = 1;
      break;
    default:
      while (1) {};
  }

  // Configure time base
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = period; // this should work for both 16 and 32 bit timer
  TIM_InitStruct.RepetitionCounter = 0;
  TIM_InitStruct.Prescaler = prescaler - 1;
  LL_TIM_Init(TIMx, &TIM_InitStruct);

  LL_TIM_DisableARRPreload(TIMx);
  LL_TIM_SetClockSource(TIMx, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIMx, LL_TIM_TRGO_RESET);
#if defined(LL_TIM_TRGO2_RESET)
  LL_TIM_SetTriggerOutput2(TIMx, LL_TIM_TRGO2_RESET);
#endif
  LL_TIM_DisableMasterSlaveMode(TIMx);
}


void tim_config_1us(TIM_TypeDef* TIMx, uint32_t period)
{
  tim_config_up(TIMx, period, TIMER_BASE_1US);
}


void tim_config_1us_freerunning(TIM_TypeDef* TIMx)
{
  tim_config_up(TIMx, 0xFFFFFFFF, TIMER_BASE_1US); // works for both 16 and 32 bit timer
}


void tim_enable(TIM_TypeDef* TIMx)
{
  LL_TIM_EnableCounter(TIMx);
}


void tim_init_up(TIM_TypeDef* TIMx, uint32_t period, TIMERBASEENUM rate)
{
  tim_config_up(TIMx, period, rate);

  LL_TIM_EnableCounter(TIMx);
}


void tim_init_1us(TIM_TypeDef* TIMx, uint32_t period)
{
  tim_init_up(TIMx, period, TIMER_BASE_1US);
}


void tim_init_1us_freerunning(TIM_TypeDef* TIMx)
{
  tim_init_up(TIMx, 0xFFFFFFFF, TIMER_BASE_1US); // works for both 16 and 32 bit timer
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_PERIPHERALS_H
