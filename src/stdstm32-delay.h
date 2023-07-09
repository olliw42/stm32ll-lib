//*******************************************************
// Copyright (c) OlliW, OlliW42, www.olliw.eu
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// my stripped down DELAY standard library
//*******************************************************
#ifndef STDSTM32_DELAY_H
#define STDSTM32_DELAY_H
#ifdef __cplusplus
extern "C" {
#endif


#ifdef DELAY_USE_DWT
//-------------------------------------------------------
// DWT timer
//-------------------------------------------------------
// Data Watchpoint and Trace (DWT)

void delay_init(void)
{
  CoreDebug->DEMCR &=~ CoreDebug_DEMCR_TRCENA_Msk;
#if defined STM32F7
  DWT->LAR = 0xC5ACCE55;
#endif
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL &=~ DWT_CTRL_CYCCNTENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
}


#pragma GCC push_options
#pragma GCC optimize ("O3")

static inline void delay_ns(uint32_t ns)
{
  uint32_t startTick = DWT->CYCCNT;
  uint32_t delayTicks = (ns * (SystemCoreClock/1000000) + 500) / 1000;
  while ((DWT->CYCCNT - startTick) < delayTicks) {};
}

#pragma GCC pop_options


void delay_us(uint32_t us)
{
  uint32_t startTick = DWT->CYCCNT;
  uint32_t delayTicks = us * (SystemCoreClock/1000000);
  while ((DWT->CYCCNT - startTick) < delayTicks) {};
}


void delay_ms(uint16_t ms)
{
  delay_us((uint32_t)1000 * ms);
}


#else
//-------------------------------------------------------
// 32 BIT Timer
//-------------------------------------------------------
// G4: TIM2, TIM5
// F7: TIM2, TIM5

#if defined DELAY_32BIT_USE_TIM2 || defined DELAY_32BIT_USE_TIM2_W_INIT
  #define DELAY_32BIT_TIMx  TIM2

#elif defined DELAY_32BIT_USE_TIM5 || defined DELAY_32BIT_USE_TIM5_W_INIT
  #define DELAY_32BIT_TIMx  TIM5
#endif

#ifdef DELAY_32BIT_TIMx

#ifdef STM32F1
  #error F1 does not have 32bit timer !!
#endif


void delay_init(void)
{
  tim_init_up(DELAY_32BIT_TIMx, 0xFFFFFFFF, TIMER_BASE_MAX);
}


#pragma GCC push_options
#pragma GCC optimize ("O3")

static inline void delay_ns(uint32_t ns)
{
  uint32_t startTick = DELAY_32BIT_TIMx->CNT;
  uint32_t delayTicks = (ns * (SystemCoreClock/1000000) + 500) / 1000;
  while ((DELAY_32BIT_TIMx->CNT - startTick) < delayTicks) {};
}

#pragma GCC pop_options


void delay_us(uint32_t us)
{
  uint32_t startTick = DELAY_32BIT_TIMx->CNT;
  uint32_t delayTicks = us * (SystemCoreClock/1000000);
  while ((DELAY_32BIT_TIMx->CNT - startTick) < delayTicks) {};
}


void delay_ms(uint16_t ms)
{
  delay_us((uint32_t)1000 * ms);
}


#else
//-------------------------------------------------------
// 16 BIT Timer
//-------------------------------------------------------

#if defined DELAY_USE_TIM1 || defined DELAY_USE_TIM1_W_INIT || defined DELAY_USE_TIM1_WO_INIT
  #define DELAY_TIMx  TIM1

#elif defined DELAY_USE_TIM2 || defined DELAY_USE_TIM2_W_INIT || defined DELAY_USE_TIM2_WO_INIT
  #define DELAY_TIMx  TIM2

#elif defined DELAY_USE_TIM3 || defined DELAY_USE_TIM3_W_INIT || defined DELAY_USE_TIM3_WO_INIT
  #define DELAY_TIMx  TIM3

#elif defined DELAY_USE_TIM4 || defined DELAY_USE_TIM4_W_INIT || defined DELAY_USE_TIM4_WO_INIT
  #define DELAY_TIMx  TIM4

#elif defined DELAY_USE_TIM5 || defined DELAY_USE_TIM5_W_INIT || defined DELAY_USE_TIM5_WO_INIT
  #define DELAY_TIMx  TIM5

#elif defined DELAY_USE_TIM6 || defined DELAY_USE_TIM6_W_INIT || defined DELAY_USE_TIM6_WO_INIT
  #define DELAY_TIMx  TIM6

#elif defined DELAY_USE_TIM7 || defined DELAY_USE_TIM7_W_INIT || defined DELAY_USE_TIM7_WO_INIT
  #define DELAY_TIMx  TIM7

#elif defined DELAY_USE_TIM8 || defined DELAY_USE_TIM8_W_INIT || defined DELAY_USE_TIM8_WO_INIT
  #define DELAY_TIMx  TIM8

#elif defined DELAY_USE_TIM14 || defined DELAY_USE_TIM14_W_INIT || defined DELAY_USE_TIM14_WO_INIT
  #define DELAY_TIMx  TIM14

#elif defined DELAY_USE_TIM15 || defined DELAY_USE_TIM15_W_INIT || defined DELAY_USE_TIM15_WO_INIT
  #define DELAY_TIMx  TIM15

#elif defined DELAY_USE_TIM16 || defined DELAY_USE_TIM16_W_INIT || defined DELAY_USE_TIM16_WO_INIT
  #define DELAY_TIMx  TIM16

#elif defined DELAY_USE_TIM17 || defined DELAY_USE_TIM17_W_INIT || defined DELAY_USE_TIM17_WO_INIT
  #define DELAY_TIMx  TIM17

#endif

#ifdef DELAY_TIMx


void delay_init(void)
{
#if defined DELAY_USE_TIM1 || defined DELAY_USE_TIM1_W_INIT || \
    defined DELAY_USE_TIM2 || defined DELAY_USE_TIM2_W_INIT || \
    defined DELAY_USE_TIM3 || defined DELAY_USE_TIM3_W_INIT || \
    defined DELAY_USE_TIM4 || defined DELAY_USE_TIM4_W_INIT || \
    defined DELAY_USE_TIM5 || defined DELAY_USE_TIM5_W_INIT || \
    defined DELAY_USE_TIM6 || defined DELAY_USE_TIM6_W_INIT || \
    defined DELAY_USE_TIM7 || defined DELAY_USE_TIM7_W_INIT || \
    defined DELAY_USE_TIM8 || defined DELAY_USE_TIM8_W_INIT || \
    defined DELAY_USE_TIM14 || defined DELAY_USE_TIM14_W_INIT || \
    defined DELAY_USE_TIM15 || defined DELAY_USE_TIM15_W_INIT || \
    defined DELAY_USE_TIM16 || defined DELAY_USE_TIM16_W_INIT || \
    defined DELAY_USE_TIM17 || defined DELAY_USE_TIM17_W_INIT

  tim_init_1us_freerunning(DELAY_TIMx);
#endif
}


// ATTENTION: may not be very precise for short few us delays
void delay_us(uint16_t us)
{
  uint16_t startTick = DELAY_TIMx->CNT;
  us++;
  while ((DELAY_TIMx->CNT - startTick) < us) {};
}


void delay_ms(uint16_t ms)
{
  if (ms < 65) {
    delay_us(1000 * ms);
  } else {
    while (ms) { delay_us(1000); ms--; }
  }
}

#endif

#endif

#endif


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_DELAY_H

