//*******************************************************
// Copyright (c) OlliW, OlliW42, www.olliw.eu
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// my stripped down UART standard library
// only TX, no RX, no halfduplex, no wait and tmo rx functions, no convenience functions, quite a number more strips
//*******************************************************
// Interface:
//
// #define SWUART_USE_TIM1/2/3/4/5/15 // requires a timer, with 8 MHz or 10 MHz ticks, uses OC1/2 irq
// #define SWUART_TX_IO
//
// #define SWUART_DONOTSETUPTIMER     // do not set up timer in init()
// #define SWUART_DONOTSETUPTX        // do not set up TX pin in init()
//
// #define SWUART_BAUD
// #define SWUART_INVERTED
//
// #define SWUART_USE_TX
// #define SWUART_TXBUFSIZE
//
// #define SWUART_IRQ_OC_PRIORITY
//
//*******************************************************
#ifndef STDSTM32_UARTSW_H
#define STDSTM32_UARTSW_H
#ifdef __cplusplus
extern "C" {
#endif


//-------------------------------------------------------
// Enums
//-------------------------------------------------------
// in order to not introduce hardware dependency upstream
// we define our own enums for parity and stopbits

#ifndef UART_ENUMS
#define UART_ENUMS

typedef enum {
  XUART_PARITY_NO = LL_USART_PARITY_NONE, // XUART_xxx to avoid overlap with HAL
  XUART_PARITY_EVEN = LL_USART_PARITY_EVEN,
  XUART_PARITY_ODD = LL_USART_PARITY_ODD,
  XUART_PARITY_MAKEITU32 = UINT32_MAX,
} UARTPARITYENUM;

typedef enum {
  UART_STOPBIT_0_5 = LL_USART_STOPBITS_0_5,
  UART_STOPBIT_1 = LL_USART_STOPBITS_1,
  UART_STOPBIT_1_5 = LL_USART_STOPBITS_1_5,
  UART_STOPBIT_2 = LL_USART_STOPBITS_2,
  UART_STOPBIT_MAKEITU32 = UINT32_MAX,
} UARTSTOPBITENUM;

#endif


//-------------------------------------------------------
// Defines
//-------------------------------------------------------

#include "stdstm32.h"
#include "stdstm32-peripherals.h"

#ifndef SWUART_BAUD
  #define SWUART_BAUD           115200UL // 9600UL
#endif

#ifdef SWUART_USE_TX
  #ifndef SWUART_TXBUFSIZE
    #define SWUART_TXBUFSIZE    256 // MUST be 2^N
  #endif
  #if SWUART_TXBUFSIZE < 2
    #error SWUART_TXBUFSIZE must be larger than 1 !
  #elif ((SWUART_TXBUFSIZE & (SWUART_TXBUFSIZE-1)) != 0)
    #error SWUART_TXBUFSIZE must be a power of 2 !
  #endif

  #define SWUART_TXBUFSIZEMASK  (SWUART_TXBUFSIZE-1)

  volatile char swuart_txbuf[SWUART_TXBUFSIZE];
  volatile uint16_t swuart_txwritepos; // pos at which the last byte was stored
  volatile uint16_t swuart_txreadpos; // pos at which the next byte is to be fetched

  volatile uint16_t swuart_txbitpos;
  volatile uint16_t swuart_txc;
#endif


#if defined SWUART_USE_TX || defined SWUART_USE_RX
  volatile uint16_t swuart_bittime_ccr;
  volatile uint16_t swuart_bittime_ccr15; // time for rx to find middle of bit
#endif


#if (defined SWUART_USE_TIM1)
  #define SWUART_TIMx             TIM1
  #define SWUART_TIMx_IRQn        TIM1_CC_IRQn
  #define SWUART_TIMx_IRQHandler  TIM1_CC_IRQHandler
#elif (defined SWUART_USE_TIM2)
  #define SWUART_TIMx             TIM2
  #define SWUART_TIMx_IRQn        TIM2_IRQn
  #define SWUART_TIMx_IRQHandler  TIM2_IRQHandler
#elif (defined SWUART_USE_TIM3)
  #define SWUART_TIMx             TIM3
  #define SWUART_TIMx_IRQn        TIM3_IRQn
  #define SWUART_TIMx_IRQHandler  TIM3_IRQHandler
#elif (defined SWUART_USE_TIM4)
  #define SWUART_TIMx             TIM4
  #define SWUART_TIMx_IRQn        TIM4_IRQn
  #define SWUART_TIMx_IRQHandler  TIM4_IRQHandler
#elif (defined SWUART_USE_TIM5)
  #define SWUART_TIMx             TIM5
  #define SWUART_TIMx_IRQn        TIM5_IRQn
  #define SWUART_TIMx_IRQHandler  TIM5_IRQHandler
#elif (defined SWUART_USE_TIM15)
  #define SWUART_TIMx             TIM15
  #define SWUART_TIMx_IRQn        TIM1_BRK_TIM15_IRQn
  #define SWUART_TIMx_IRQHandler  TIM1_BRK_TIM15_IRQHandler
#else
  #error SWUART_USE_TIM not defined!
#endif

#ifndef SWUART_INVERTED
  #define SWUART_TX_HIGH          gpio_high(SWUART_TX_IO)
  #define SWUART_TX_LOW           gpio_low(SWUART_TX_IO)
#else
  #define SWUART_TX_HIGH          gpio_low(SWUART_TX_IO)
  #define SWUART_TX_RESETBITS     gpio_high(SWUART_TX_IO)
#endif


//-------------------------------------------------------
// ISR routines
//-------------------------------------------------------

#ifdef SWUART_USE_TX
void _swuart_tx_start(void)
{
  if (!LL_TIM_IsEnabledIT_CC1(SWUART_TIMx)) { // isr not yet running, so start
    swuart_txbitpos = 1; // indicates SWUART_STATUS_TX
    SWUART_TIMx->CCR1 = (SWUART_TIMx->CNT + 16); // postpone it by 2us
    LL_TIM_ClearFlag_CC1(SWUART_TIMx);
    LL_TIM_EnableIT_CC1(SWUART_TIMx);
  }
}
#endif


//#pragma GCC push_options
//#pragma GCC optimize ("O3")

#ifdef SWUART_USE_RX
#endif


void SWUART_TIMx_IRQHandler(void)
{
#ifdef SWUART_USE_TX
  if (LL_TIM_IsActiveFlag_CC1(SWUART_TIMx)) {
    LL_TIM_ClearFlag_CC1(SWUART_TIMx);
    SWUART_TIMx->CCR1 += swuart_bittime_ccr;

    if (swuart_txbitpos) { // indicates SWUARTSTATUS_TX
      swuart_txbitpos++;

      if (swuart_txbitpos <= 2) {
        SWUART_TX_LOW;
        // get the next char from the buffer
        swuart_txreadpos = (swuart_txreadpos + 1) & SWUART_TXBUFSIZEMASK;
        swuart_txc = swuart_txbuf[swuart_txreadpos];
      } else
      if (swuart_txbitpos <= 10) {
        if (swuart_txc & 0x01) { SWUART_TX_HIGH; }else{ SWUART_TX_LOW; }
        swuart_txc >>= 1;
      } else
      if (swuart_txbitpos == 11) {
        SWUART_TX_HIGH;
        // is there another char?
        if (swuart_txwritepos != swuart_txreadpos) swuart_txbitpos = 1; // fifo not empty
      } else {
        SWUART_TX_HIGH;
        LL_TIM_DisableIT_CC1(SWUART_TIMx);
        swuart_txbitpos = 0; // indicates SWUART_STATUS_IDLE
      }
    } else {
      // something is wrong
      SWUART_TX_HIGH;
      LL_TIM_DisableIT_CC1(SWUART_TIMx);
      swuart_txbitpos = 0; // indicates SWUART_STATUS_IDLE
    }

  } // TIM_IT_CC1
#endif

#ifdef SWUART_USE_RX
#endif
}

//#pragma GCC pop_options


//-------------------------------------------------------
// TX routines
//-------------------------------------------------------
#ifdef SWUART_USE_TX

uint16_t swuart_putc(char c)
{
  uint16_t next = (swuart_txwritepos + 1) & SWUART_TXBUFSIZEMASK;
  if (swuart_txreadpos != next) { // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
    swuart_txbuf[next] = c;
    swuart_txwritepos = next;
    _swuart_tx_start();
    return 1;
  }
  return 0;
}


void swuart_puts(char *s)
{
  while (*s) {
    swuart_putc(*s);
    s++;
  }
}


void swuart_putbuf(uint8_t* buf, uint16_t len)
{
uint16_t i;

  for (i = 0; i < len; i++) swuart_putc(buf[i]);
}


uint16_t swuart_tx_notfull(void)
{
  uint16_t next = (swuart_txwritepos + 1) & SWUART_TXBUFSIZEMASK;
  return (swuart_txreadpos != next); // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
}


static inline uint16_t swuart_tx_isempty(void)
{
  if (swuart_txwritepos == swuart_txreadpos) return 1;
  return 0;
}


static inline void swuart_tx_flush(void)
{
  swuart_txwritepos = swuart_txreadpos = 0;
}


#endif // defined SWUART_USE_TX


//-------------------------------------------------------
// RX routines
//-------------------------------------------------------
#ifdef SWUART_USE_RX
#endif // defined SWUART_USE_RX


//-------------------------------------------------------
// INIT routines
//-------------------------------------------------------

void swuart_setprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
  // TODO
}


// STM32F1
// 115200 = 8.68 us     1/8 us timer:  69.444 => use  69 => 0.64% error, should be fine, and works fine
// 57600  = 17.36 us    1/8 us timer: 138.888 => use 139 => 0.08% error, should be fine, and works fine
// 9600   = 104.17 us   1/8 us timer: 833.333 => use 833 => 0.04% error, should be fine
void swuart_setbaudrate(uint32_t baud)
{
#if defined SWUART_USE_TX || defined SWUART_USE_RX
#if defined STM32F1
  switch (baud) {
  case 115200: ATOMIC(swuart_bittime_ccr = 69; swuart_bittime_ccr15 = 104 - 20); break;
  case 57600:  ATOMIC(swuart_bittime_ccr = 139; swuart_bittime_ccr15 = 208); break;
  case 9600:   ATOMIC(swuart_bittime_ccr = 833; swuart_bittime_ccr15 = 1250); break;
  default:
    ATOMIC(swuart_bittime_ccr = 69; swuart_bittime_ccr15 = 104 - 20);
  }
#elif defined STM32L4
  switch (baud) {
  case 115200: ATOMIC(swuart_bittime_ccr = 87; swuart_bittime_ccr15 = 130 - 20); break;
  case 57600:  ATOMIC(swuart_bittime_ccr = 174; swuart_bittime_ccr15 = 260); break;
  case 9600:   ATOMIC(swuart_bittime_ccr = 1042; swuart_bittime_ccr15 = 1563); break;
  default:
    ATOMIC(swuart_bittime_ccr = 87; swuart_bittime_ccr15 = 130 - 20);
  }
#endif
#endif
}


void swuart_tx_enable(FunctionalState flag)
{
#ifdef SWUART_USE_TX
  // just here for compatibility & consistency
#endif
}


void swuart_rx_enableisr(FunctionalState flag)
{
#ifdef SWUART_USE_RX
  // just here for compatibility & consistency
#endif
}


void swuart_init_isroff(void)
{
#if defined SWUART_USE_TX || defined SWUART_USE_RX

  // Configure Tx pin as alternative function push-pull
#ifdef SWUART_USE_TX
#ifndef SWUART_DONOTSETUPTX
#ifdef SWUART_INVERTED
  gpio_init(SWUART_TX_IO, IO_MODE_OUTPUT_PP_HIGH, IO_SPEED_VERYFAST);
#else
  gpio_init(SWUART_TX_IO, IO_MODE_OUTPUT_PP_LOW, IO_SPEED_VERYFAST);
#endif
  SWUART_TX_HIGH; // should not be needed, but can't hurt either
#endif
#endif

  // Configure Rx pin as input with pull up
#ifdef SWUART_USE_RX
#endif

  // Configure Timer
#ifndef SWUART_DONOTSETUPTIMER
#if defined STM32F1
  tim_init_up(SWUART_TIMx, 0xFFFF, TIMER_BASE_8MHZ);
#elif defined STM32L4
  tim_init_up(SWUART_TIMx, 0xFFFF, TIMER_BASE_10MHZ);
#else
  #error stm not suppoerted by swuart !
#endif
#endif

  // Configure TIMx OC, since we do not output to a pin, we can use any channel we want
  // use OC1 for Tx and OC2 for Rx
LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};

  TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_FROZEN;
  TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
  TIM_OC_InitStruct.CompareValue = 0;
  TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
  TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
  TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
#ifdef SWUART_USE_TX
  LL_TIM_OC_Init(SWUART_TIMx, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);
  LL_TIM_OC_DisableFast(SWUART_TIMx, LL_TIM_CHANNEL_CH1); // Cube LL does also do this
#endif
#ifdef SWUART_USE_RX
  // RX channel uses OC2
#endif

  // Configure NVIC channel for OC ISR, required for both TX and RX
#ifdef SWUART_OC_PREEMPTIONPRIORITY
  #error Legacy SWUART_OC_PREEMPTIONPRIORITY used !
#endif
#ifdef SWUART_OC_SUBPRIORITY
  #error Legacy SWUART_OC_SUBPRIORITY used !
#endif

#ifndef SWUART_IRQ_OC_PRIORITY
  #define SWUART_IRQ_OC_PRIORITY  15 // set priority to lowest
#endif

#ifdef SWUART_USE_TX
  LL_TIM_ClearFlag_CC1(SWUART_TIMx);
#endif
#ifdef SWUART_USE_RX
#endif
  nvic_irq_enable_w_priority(SWUART_TIMx_IRQn, SWUART_IRQ_OC_PRIORITY);

  // Configure TIMx PinChange for RX
#ifdef SWUART_USE_RX
#endif

  // Init variables
  swuart_setbaudrate(SWUART_BAUD);

#ifdef SWUART_USE_TX
  // Flush buffer
  swuart_txwritepos = swuart_txreadpos = 0;
  swuart_txbitpos = 0; // indicates SWUART_STATUS_IDLE
#endif

#ifdef SWUART_USE_RX
#endif

#endif
}


void swuart_init(void)
{
  swuart_init_isroff();
  swuart_rx_enableisr(ENABLE);
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_SWUART_H

