//*******************************************************
// Copyright (c) OlliW, OlliW42, www.olliw.eu
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// my stripped down UART standard library
// only ISR, no DMA, no wait and tmo rx functions, no convenience functions
//*******************************************************
// Interface:
//
// #define UARTE_USE_UART1, UARTE_USE_UART1_PA9PA10, UARTE_USE_UART1_PB6PB7, UARTE_USE_UART1_PC4PC5
// #define UARTE_USE_UART2, UARTE_USE_UART2_PA2PA3, UARTE_USE_UART2_PD5PD6, UARTE_USE_UART2_PB3PB4, UARTE_USE_UART2_PA14PA15
// #define UARTE_USE_UART3, UARTE_USE_UART3_PB10PB11, UARTE_USE_UART3_PC10PC11, UARTE_USE_UART3_PB8PB9
// #define UARTE_USE_UART4, UARTE_USE_UART4_PC10PC11, UARTE_USE_UART4_PA0PA1
// #define UARTE_USE_UART5, UARTE_USE_UART5_PC12PD2
// #define UARTE_USE_UART6, UARTE_USE_UART6_PG14PG9
// #define UARTE_USE_UART7, UARTE_USE_UART7_PE8PE7, UARTE_USE_UART7_PF7PF6
// #define UARTE_USE_UART8, UARTE_USE_UART8_PE1PE0
// #define UARTE_USE_LPUART1, UARTE_USE_LPUART1_PA2PA3, UARTE_USE_LPUART1_PC1PC0
//
// #define UARTE_BAUD
//
// #define UARTE_USE_TX
// #define UARTE_TXBUFSIZE
// #define UARTE_USE_TX_ISR
// #define UARTE_USE_TX_DMA
// #define UARTE_INVERTED
//
// #define UARTE_USE_RX
// #define UARTE_RXBUFSIZE
//
// #define UARTE_IRQ_PRIORITY
//
// #define UARTE_USE_RXERRORCOUNT
//
// #define UARTE_RX_CALLBACK_FULL(c)
// #define UARTE_TC_CALLBACK() // ATTENTION: when used, one never should trigger TC isr without a byte written to the tx buffer
//
//*******************************************************
#ifndef STDSTM32_UARTE_H
#define STDSTM32_UARTE_H
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
#ifdef LL_USART_STOPBITS_0_5
  UART_STOPBIT_0_5 = LL_USART_STOPBITS_0_5, // not allowed for LPUART, and some STM32F0!
#endif
  UART_STOPBIT_1 = LL_USART_STOPBITS_1,
#ifdef LL_USART_STOPBITS_1_5
  UART_STOPBIT_1_5 = LL_USART_STOPBITS_1_5, // not allowed for LPUART, and some STM32!
#endif
  UART_STOPBIT_2 = LL_USART_STOPBITS_2,
  UART_STOPBIT_MAKEITU32 = UINT32_MAX,
} UARTSTOPBITENUM;

#endif


//-------------------------------------------------------
// Defines
//-------------------------------------------------------

#include "stdstm32-peripherals.h"

#if (defined UARTE_USE_TX_ISR) && (!defined UARTE_USE_TX)
  #error UARTE_USE_TX_ISR used without UARTE_USE_TX!
#endif
#if (defined UARTE_USE_RXERRORCOUNT) && (!defined UARTE_USE_RX)
  #error UARTE_USE_RXERRORCOUNT used without UARTE_USE_RX!
#endif

#ifdef UARTE_USE_TX_DMA
  #error UARTE_USE_TX_DMA not supported by this library!
#endif


#ifndef UARTE_BAUD
  #define UARTE_BAUD             115200UL // 9600UL
#endif


#ifdef UARTE_USE_TX_ISR
  #ifndef UARTE_TXBUFSIZE
    #define UARTE_TXBUFSIZE      256 // MUST be 2^N
  #endif
  #if UARTE_TXBUFSIZE < 2
    #error UARTE_TXBUFSIZE must be larger than 1 !
  #elif ((UARTE_TXBUFSIZE & (UARTE_TXBUFSIZE-1)) != 0)
    #error UARTE_TXBUFSIZE must be a power of 2 !
  #endif

  #define UARTE_TXBUFSIZEMASK  (UARTE_TXBUFSIZE-1)

  volatile char uarte_txbuf[UARTE_TXBUFSIZE];
  volatile uint16_t uarte_txwritepos; // pos at which the last byte was stored
  volatile uint16_t uarte_txreadpos; // pos at which the next byte is to be fetched
#endif


#ifdef UARTE_USE_RX
  #ifndef UARTE_RXBUFSIZE
    #define UARTE_RXBUFSIZE      256 //128 //MUST be 2^N
  #endif
  #if UARTE_RXBUFSIZE < 2
    #error UARTE_RXBUFSIZE must be larger than 1 !
  #elif ((UARTE_RXBUFSIZE & (UARTE_RXBUFSIZE-1)) != 0)
    #error UARTE_RXBUFSIZE must be a power of 2 !
  #endif

  #define UARTE_RXBUFSIZEMASK  (UARTE_RXBUFSIZE-1)

  volatile char uarte_rxbuf[UARTE_RXBUFSIZE];
  volatile uint16_t uarte_rxwritepos; // pos at which the last byte was stored
  volatile uint16_t uarte_rxreadpos; // pos at which the next byte is to be fetched

  #ifdef UARTE_USE_RXERRORCOUNT
  volatile uint32_t uarte_errorcnt_rxnoise;
  volatile uint32_t uarte_errorcnt_rxframe;
  volatile uint32_t uarte_errorcnt_rxoverrun;
  #endif
#endif


// catch legacy defines
#if defined UARTE_USE_UART1_REMAPPED || defined UARTE_USE_UART1_REMAPPED2
  #error REMAPPED not supported anymore!
#endif
#if defined UARTE_USE_UART2_REMAPPED || defined UARTE_USE_UART2_REMAPPED2 || defined UARTE_USE_UART2_REMAPPED3
  #error REMAPPED not supported anymore!
#endif
#if defined UARTE_USE_UART3_REMAPPED
  #error REMAPPED not supported anymore!
#endif
#if defined UARTE_USE_UART4_REMAPPED
  #error REMAPPED not supported anymore!
#endif
#if defined UARTE_USE_UART6_REMAPPED
  #error REMAPPED not supported anymore!
#endif
#if defined UARTE_USE_UART7_REMAPPED
  #error REMAPPED not supported anymore!
#endif
#if defined UARTE_USE_LPUART1_REMAPPED
  #error REMAPPED not supported anymore!
#endif


#if defined UARTE_USE_UART1 || defined UARTE_USE_UART1_PA9PA10 || defined UARTE_USE_UART1_PB6PB7 || defined UARTE_USE_UART1_PC4PC5
  #define UARTE_UARTx             USART1
  #ifdef UARTE_USE_UART1
    // user needs to specify UARTE_USE_TX_IO, UARTE_USE_RX_IO, and possibly UARTE_USE_IO_AF
  #elif defined UARTE_USE_UART1_PA9PA10
    #define UARTE_TX_IO           IO_PA9
    #define UARTE_RX_IO           IO_PA10
  #elif defined UARTE_USE_UART1_PB6PB7 // was UART1_REMAPPED
    #define UARTE_TX_IO           IO_PB6
    #define UARTE_RX_IO           IO_PB7
  #elif defined UARTE_USE_UART1_PC4PC5 // only G4 // was UART1_REMAPPED2
    #define UARTE_TX_IO           IO_PC4
    #define UARTE_RX_IO           IO_PC5
  #endif
  #ifndef STM32F0
    #define UARTE_IO_AF           IO_AF_7
  #else
    #ifdef UARTE_USE_UART1
      // user needs to specify UARTE_USE_IO_AF
    #elif defined UARTE_USE_UART1_PA9PA10
      #define UARTE_IO_AF         IO_AF_1
    #elif defined UARTE_USE_UART1_PB6PB7
      #define UARTE_IO_AF         IO_AF_0
    #else
      #error UARTE_USE_UART1 mapping not available on STM32F0 !
    #endif
  #endif
  #define UARTE_IRQn              USART1_IRQn
  #define UARTE_IRQHandler        USART1_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UARTE_USE_UART2 || defined UARTE_USE_UART2_PA2PA3 || defined UARTE_USE_UART2_PD5PD6 || defined UARTE_USE_UART2_PB3PB4 || defined UARTE_USE_UART2_PA14PA15
  #define UARTE_UARTx             USART2
  #ifdef UARTE_USE_UART2
    // user needs to specify UARTE_USE_TX_IO, UARTE_USE_RX_IO, and possibly UARTE_USE_IO_AF
  #elif defined UARTE_USE_UART2_PA2PA3
    #define UARTE_TX_IO           IO_PA2
    #define UARTE_RX_IO           IO_PA3
  #elif defined UARTE_USE_UART2_PD5PD6 // was UART2_REMAPPED
    #define UARTE_TX_IO           IO_PD5
    #define UARTE_RX_IO           IO_PD6
  #elif defined UARTE_USE_UART2_PB3PB4 // only G4 // was UART2_REMAPPED2
    #define UARTE_TX_IO           IO_PB3
    #define UARTE_RX_IO           IO_PB4
  #elif defined UARTE_USE_UART2_PA14PA15 // only G4, F0 // ATTENTION: on G4 PA14 overlaps with SWCLK // was UART2_REMAPPED3
    #define UARTE_TX_IO           IO_PA14
    #define UARTE_RX_IO           IO_PA15
  #endif
  #ifndef STM32F0
    #define UARTE_IO_AF           IO_AF_7
  #else
    #if defined UARTE_USE_UART2
      // user needs to specify UARTE_USE_IO_AF
    #elif defined UARTE_USE_UART2_PA2PA3 || defined UARTE_USE_UART2_PA14PA15
      #define UARTE_IO_AF         IO_AF_1
    #else
      #error UARTE_USE_UART2 mapping not available on STM32F0 !
    #endif
  #endif
  #define UARTE_IRQn              USART2_IRQn
  #define UARTE_IRQHandler        USART2_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UARTE_USE_UART3 || defined UARTE_USE_UART3_PB10PB11 || defined UARTE_USE_UART3_PC10PC11 || defined UARTE_USE_UART3_PB8PB9
  #define UARTE_UARTx             USART3
  #if defined UARTE_USE_UART3
    // user needs to specify UARTE_USE_TX_IO, UARTE_USE_RX_IO, and possibly UARTE_USE_IO_AF
  #elif defined UARTE_USE_UART3_PB10PB11
    #define UARTE_TX_IO           IO_PB10
    #define UARTE_RX_IO           IO_PB11
  #elif defined UARTE_USE_UART3_PC10PC11 // was UART3_REMAPPED
    #define UARTE_TX_IO           IO_PC10
    #define UARTE_RX_IO           IO_PC11
  #elif defined UARTE_USE_UART3_PB8PB9 // only F3
    #define UARTE_TX_IO           IO_PB8
    #define UARTE_RX_IO           IO_PB9
  #else
  #endif
  #ifndef STM32F0
    #define UARTE_IO_AF           IO_AF_7
  #else
    #if defined UARTE_USE_UART3
      // user needs to specify UARTE_USE_IO_AF
    #elif defined defined UARTE_USE_UART3_PB10PB11 || defined UARTE_USE_UART3_PB8PB9
      #define UARTE_IO_AF         IO_AF_4
    #elif defined UARTE_USE_UART3_PC10PC11
      #define UARTE_IO_AF         IO_AF_1
    #else
      #error UARTE_USE_UART3 mapping not available on STM32F0 !
    #endif
  #endif
  #define UARTE_IRQn              USART3_IRQn
  #define UARTE_IRQHandler        USART3_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
    #error UART3 NOT AVAILABLE !
  #elif defined STM32F0
    #undef UARTE_IRQn
    #undef UARTE_IRQHandler
    #define UARTE_IRQn            USART3_4_IRQn
    #define UARTE_IRQHandler      USART3_4_IRQHandler
  #endif

#elif defined UARTE_USE_UART4 || defined UARTE_USE_UART4_PC10PC11 || defined UARTE_USE_UART4_PA0PA1
  #define UARTE_UARTx             UART4
  #ifdef UARTE_USE_UART4
    // user needs to specify UARTE_USE_TX_IO, UARTE_USE_RX_IO, and possibly UARTE_USE_IO_AF
  #elif defined UARTE_USE_UART4_PC10PC11
    #define UARTE_TX_IO           IO_PC10
    #define UARTE_RX_IO           IO_PC11
  #elif defined UARTE_USE_UART4_PA0PA1 // only F7 // was UART4_REMAPPED
    #define UARTE_TX_IO           IO_PA0
    #define UARTE_RX_IO           IO_PA1
  #endif
  #if defined STM32F1
    #define UARTE_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F7
    #define UARTE_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UARTE_IO_AF           IO_AF_5
  #endif
  #define UARTE_IRQn              UART4_IRQn
  #define UARTE_IRQHandler        UART4_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
    #warning UARTE IO_AF & DMA for F3 still TODO
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART4 NOT AVAILABLE !
  #elif defined STM32F0
    #undef UARTE_IRQn
    #undef UARTE_IRQHandler
    #define UARTE_IRQn            USART3_4_IRQn
    #define UARTE_IRQHandler      USART3_4_IRQHandler
  #endif

#elif defined UARTE_USE_UART5 || defined UARTE_USE_UART5_PC12PD2
  #define UARTE_UARTx             UART5
  #ifdef UARTE_USE_UART5
    // user needs to specify UARTE_USE_TX_IO, UARTE_USE_RX_IO, and possibly UARTE_USE_IO_AF
  #elif defined UARTE_USE_UART5_PC12PD2
    #define UARTE_TX_IO           IO_PC12
    #define UARTE_RX_IO           IO_PD2
  #endif
  #define UARTE_IRQn              UART5_IRQn
  #define UARTE_IRQHandler        UART5_IRQHandler
  #if defined STM32F1
    #define UARTE_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F3
    #warning UARTE IO_AF & DMA for F3 still TODO //TODO
  #elif defined STM32F7
    #define UARTE_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UARTE_IO_AF           IO_AF_5
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART5 NOT AVAILABLE !
  #endif

#elif defined UARTE_USE_UART6 || defined UARTE_USE_UART6_PG14PG9
  #define UARTE_UARTx             USART6
  #ifdef UARTE_USE_UART6
    // user needs to specify UARTE_USE_TX_IO, UARTE_USE_RX_IO, and possibly UARTE_USE_IO_AF
  #elif defined UARTE_USE_UART6_PC6PC7
    #define UARTE_TX_IO           IO_PC6
    #define UARTE_RX_IO           IO_PC7
  #elif defined UARTE_USE_UART6_PG14PG9 // was UART6_REMAPPED
    #define UARTE_TX_IO           IO_PG14
    #define UARTE_RX_IO           IO_PG9
  #endif
  #define UARTE_IO_AF             IO_AF_8
  #define UARTE_IRQn              USART6_IRQn
  #define UARTE_IRQHandler        USART6_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART6 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTE_USE_UART7 || defined UARTE_USE_UART7_PE8PE7 || defined UARTE_USE_UART7_PF7PF6
  #define UARTE_UARTx             UART7
  #ifdef UARTE_USE_UART7
    // user needs to specify UARTE_USE_TX_IO, UARTE_USE_RX_IO, and possibly UARTE_USE_IO_AF
  #elif defined UARTE_USE_UART7_PE8PE7
    #define UARTE_TX_IO           IO_PE8
    #define UARTE_RX_IO           IO_PE7
  #elif defined UARTE_USE_UART7_PF7PF6 // was UART7_REMAPPED
    #define UARTE_TX_IO           IO_PF7
    #define UARTE_RX_IO           IO_PF6
  #endif
  #define UARTE_IO_AF             IO_AF_8
  #define UARTE_IRQn              UART7_IRQn
  #define UARTE_IRQHandler        UART7_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART7 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTE_USE_UART8 || defined UARTE_USE_UART8_PE1PE0
  #define UARTE_UARTx             UART8
  #ifdef UARTE_USE_UART8
    // user needs to specify UARTE_USE_TX_IO, UARTE_USE_RX_IO, and possibly UARTE_USE_IO_AF
  #elif defined UARTE_USE_UART8_PE1PE0
    #define UARTE_TX_IO           IO_PE1
    #define UARTE_RX_IO           IO_PE0
  #endif
  #define UARTE_IO_AF             IO_AF_8
  #define UARTE_IRQn              UART8_IRQn
  #define UARTE_IRQHandler        UART8_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART8 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTE_USE_LPUART1 || defined UARTE_USE_LPUART1_PA2PA3 || defined UARTE_USE_LPUART1_PC1PC0
  #define UARTE_UARTx             LPUART1
  #ifdef UARTE_USE_LPUART1
    // user needs to specify UARTE_USE_TX_IO, UARTE_USE_RX_IO, and possibly UARTE_USE_IO_AF
  #elif defined UARTE_USE_LPUART1_PA2PA3
    #define UARTE_TX_IO           IO_PA2
    #define UARTE_RX_IO           IO_PA3
    #ifdef STM32G4
      #define UARTE_IO_AF         IO_AF_12 // G4
    #else
      #define UARTE_IO_AF         IO_AF_8 // WL
    #endif
  #elif defined UARTE_USE_LPUART1_PC1PC0 // was LPUART1_REMAPPED
    #define UARTE_TX_IO           IO_PC1
    #define UARTE_RX_IO           IO_PC0
    #define UARTE_IO_AF           IO_AF_8
  #endif
  #define UARTE_IRQn              LPUART1_IRQn
  #define UARTE_IRQHandler        LPUART1_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32F7
    #error LPUART1 NOT AVAILABLE !
  #elif defined STM32G4
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
  #endif

#else
  #error No UARTE_USE_UARTx defined!
#endif


// allows us to overwrite the default pin assignment
#ifdef UARTE_USE_TX_IO
  #undef UARTE_TX_IO
  #define UARTE_TX_IO             UARTE_USE_TX_IO
#endif
#ifdef UARTE_USE_RX_IO
  #undef UARTE_RX_IO
  #define UARTE_RX_IO             UARTE_USE_RX_IO
#endif
#ifdef UARTE_USE_IO_AF
  #undef UARTE_IO_AF
  #define UARTE_IO_AF             UARTE_USE_IO_AF
#endif


// catch missing defines, compiler will throw error anyhow, but so it is nicer
#if defined UART_USE_TX && !defined UARTE_TX_IO
  #error No UARTE TX IO defined!
#endif
#if defined UART_USE_RX && !defined UARTE_RX_IO
  #error No UARTE RX IO defined!
#endif
#if !defined UARTE_IO_AF
  #error No UARTE IO AF defined!
#endif


//-------------------------------------------------------
// COMMENT:
// LL_USART_XX and LL_LPUART_XX flags are just redefines of USART_XX flags.
// So, we can use USART_XX here and below.
// This has also the advantage that it works for both USART and LPUART.
//
// Note: LPUART does not appear to have STOPBITS_0_5 and STOPBITS_1_5!!
//
// The LL_USART_xx() and LL_LPUART_xx() functions which only relate to flags or
// registers can be used for both USART and LPUART.
// Below we simply use the LL_USART_xx() functions for both.
// Notably, this appears to be incorrect for the init struct functions, which hence
// need special treatment.

#if defined UARTE_USE_LPUART1 || defined UARTE_USE_LPUART1_PA2PA3 || defined UARTE_USE_LPUART1_PC1PC0
#if !(LL_USART_PARITY_NONE == LL_LPUART_PARITY_NONE) || !(LL_USART_PARITY_EVEN == LL_LPUART_PARITY_EVEN) || \
    !(LL_USART_PARITY_ODD == LL_LPUART_PARITY_ODD) || \
    !(LL_USART_STOPBITS_1 == LL_LPUART_STOPBITS_1) || !(LL_USART_STOPBITS_2 == LL_LPUART_STOPBITS_2)
  //#error Missmatch of LL_USART and L_LPUART flags!
#endif
#endif


//-------------------------------------------------------
// Glue macros
//-------------------------------------------------------

#if defined STM32F1
  #define REG_DR        DR
  #define REG_SR        SR
  #define FLAG_SR_RXNE  LL_USART_SR_RXNE
  #define FLAG_SR_ORE   LL_USART_SR_ORE
  #define FLAG_SR_NE    LL_USART_SR_NE
  #define FLAG_SR_FE    LL_USART_SR_FE
  #define FLAG_SR_TXE   LL_USART_SR_TXE
  #define FLAG_SR_TC    LL_USART_SR_TC
  #define FLAG_SR_IDLE  LL_USART_SR_IDLE
#elif defined STM32F3 || defined STM32F7 || defined STM32L4 || defined STM32F0
  #define REG_DR        TDR
  #define REG_SR        ISR
  #define FLAG_SR_RXNE  LL_USART_ISR_RXNE
  #define FLAG_SR_ORE   LL_USART_ISR_ORE
  #define FLAG_SR_NE    LL_USART_ISR_NE
  #define FLAG_SR_FE    LL_USART_ISR_FE
  #define FLAG_SR_TXE   LL_USART_ISR_TXE
  #define FLAG_SR_TC    LL_USART_ISR_TC
  #define FLAG_SR_IDLE  LL_USART_ISR_IDLE
#elif defined STM32G4 || defined STM32WL
  #define REG_DR        TDR
  #define REG_SR        ISR
  #define FLAG_SR_RXNE  LL_USART_ISR_RXNE_RXFNE
  #define FLAG_SR_ORE   LL_USART_ISR_ORE
  #define FLAG_SR_NE    LL_USART_ISR_NE
  #define FLAG_SR_FE    LL_USART_ISR_FE
  #define FLAG_SR_TXE   LL_USART_ISR_TXE_TXFNF
  #define FLAG_SR_TC    LL_USART_ISR_TC
  #define FLAG_SR_IDLE  LL_USART_ISR_IDLE
#endif

#if defined STM32F3 || defined STM32F7 || defined STM32F0
  #define FLAGS_ICR     (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NCF | LL_USART_ICR_FECF | LL_USART_ICR_PECF)
#elif defined STM32G4 || defined STM32L4 || defined STM32WL
  #define FLAGS_ICR     (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NECF | LL_USART_ICR_FECF | LL_USART_ICR_PECF)
#endif


//-------------------------------------------------------
// ISR routine
//-------------------------------------------------------
#if defined UARTE_USE_RX || defined UARTE_USE_TX_ISR

void UARTE_IRQHandler(void)
{
  // Cube HAL for f103rc is also calling this first independent on irqs
  uint16_t usart_sr = LL_USART_ReadReg(UARTE_UARTx, REG_SR); // read USART_SR register

#ifdef UARTE_USE_RX
  if ((usart_sr & FLAG_SR_RXNE) && LL_USART_IsEnabledIT_RXNE(UARTE_UARTx)) {
#ifdef UARTE_USE_RXERRORCOUNT
    if (usart_sr & FLAG_SR_ORE) uarte_errorcnt_rxoverrun++;
    if (usart_sr & FLAG_SR_NE) uarte_errorcnt_rxnoise++;
    if (usart_sr & FLAG_SR_FE) uarte_errorcnt_rxframe++;
#endif
    // Cube LL driver (for f103?) says that reading SR and then writing to DR will clear error flags, so do it after error checks
    uint8_t usart_dr = LL_USART_ReceiveData8(UARTE_UARTx); // read USART_DR register, clears RXNE and RX error flags
#ifdef UARTE_RX_CALLBACK_FULL
    UARTE_RX_CALLBACK_FULL(usart_dr);
#else
    uint16_t next = (uarte_rxwritepos + 1) & UARTE_RXBUFSIZEMASK;
    if (uarte_rxreadpos != next) { // fifo not full
      uarte_rxbuf[next] = usart_dr;
      uarte_rxwritepos = next;
    }
#endif
  }
#if defined STM32F1
  // ORE is enabled along with RXNE, so we need to handle it
  // ORE bit is reset by read to USART_SR followed by read to USART_DR
  if (usart_sr & FLAG_SR_ORE) {
    LL_USART_ReceiveData8(UARTE_UARTx); // read USART_DR register, clears RXNE and RX error flags
  }
#else
  // F3: ORE is enabled along with RXNE, but can be cleared by writing 1 to ICR
  // F7: these are cleared by write 1 to the ICR register, not by a read of DR register!
  LL_USART_WriteReg(UARTE_UARTx, ICR, FLAGS_ICR);
#endif
#endif

#ifdef UARTE_USE_TX_ISR
  if ((usart_sr & FLAG_SR_TXE) && LL_USART_IsEnabledIT_TXE(UARTE_UARTx)) {
    if (uarte_txwritepos != uarte_txreadpos) { // fifo not empty
      uarte_txreadpos = (uarte_txreadpos + 1) & UARTE_TXBUFSIZEMASK;
      LL_USART_TransmitData8(UARTE_UARTx, uarte_txbuf[uarte_txreadpos]); // write to USART_DR register, clears TXE flag, as well as TC
    } else {
      LL_USART_DisableIT_TXE(UARTE_UARTx); // disable interrupt when empty
#ifdef UARTE_TC_CALLBACK
      // ATTENTION: doesn't distinguish between empty fifo and last byte just send!
      LL_USART_ClearFlag_TC(UARTE_UARTx); // enable TC interrupt
      LL_USART_EnableIT_TC(UARTE_UARTx);
#endif
#ifdef UARTE_IDLE_CALLBACK
      LL_USART_ClearFlag_IDLE(UARTE_UARTx); // enable IDLE interrupt
      LL_USART_EnableIT_IDLE(UARTE_UARTx);
#endif
    }
  }

#ifdef UARTE_TC_CALLBACK
  if ((usart_sr & FLAG_SR_TC) && LL_USART_IsEnabledIT_TC(UARTE_UARTx)) {
    LL_USART_DisableIT_TC(UARTE_UARTx);
    LL_USART_ClearFlag_TC(UARTE_UARTx);
    UARTE_TC_CALLBACK();
  }
#endif
#ifdef UARTE_IDLE_CALLBACK
  if ((usart_sr & FLAG_SR_IDLE) && LL_USART_IsEnabledIT_IDLE(UARTE_UARTx)) {
    LL_USART_DisableIT_IDLE(UARTE_UARTx);
    LL_USART_ClearFlag_IDLE(UARTE_UARTx);
    UARTE_IDLE_CALLBACK();
  }
#endif
#endif
}

#endif // defined UARTE_USE_RX || defined UARTE_USE_TX_ISR


//-------------------------------------------------------
// TX routines
//-------------------------------------------------------
#ifdef UARTE_USE_TX

uint16_t uarte_putc(char c)
{
#ifndef UARTE_USE_TX_ISR
  while (!LL_USART_IsActiveFlag_TC(UARTE_UARTx)) {}
  LL_USART_TransmitData8(UARTE_UARTx, c);
  return 1;
#else
  uint16_t next = (uarte_txwritepos + 1) & UARTE_TXBUFSIZEMASK;
  if (uarte_txreadpos != next) { // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
    uarte_txbuf[next] = c;
    uarte_txwritepos = next;
    LL_USART_EnableIT_TXE(UARTE_UARTx); // enable interrupt // then TDR is empty, the TXE bit is set, and hence the isr is triggered
    return 1;
  }
  return 0;
#endif
}


void uarte_puts(const char *s)
{
  while (*s) {
    uarte_putc(*s);
    s++;
  }
}


void uarte_putbuf(uint8_t* buf, uint16_t len)
{
uint16_t i;

  for (i = 0; i < len; i++) uarte_putc(buf[i]);
}


#ifdef UARTE_USE_TX_ISR
uint16_t uarte_tx_notfull(void)
{
  uint16_t next = (uarte_txwritepos + 1) & UARTE_TXBUFSIZEMASK;
  return (uarte_txreadpos != next); // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
}


static inline uint16_t uarte_tx_isempty(void)
{
  if (uarte_txwritepos == uarte_txreadpos) return 1;
  return 0;
}


static inline void uarte_tx_flush(void)
{
  uarte_txwritepos = uarte_txreadpos = 0;
}
#else
static inline void uarte_tx_flush(void) {}
#endif

#endif // defined UARTE_USE_TX


//-------------------------------------------------------
// RX routines
//-------------------------------------------------------
#ifdef UARTE_USE_RX

static inline uint16_t uarte_rx_available(void)
{
  if (uarte_rxwritepos == uarte_rxreadpos) return 0; // fifo empty
  return 1;
}


uint16_t uarte_rx_bytesavailable(void)
{
int16_t d;

  d = (int16_t)uarte_rxwritepos - (int16_t)uarte_rxreadpos;
  return (d < 0) ? d + (UARTE_RXBUFSIZEMASK + 1) : d;
}


char uarte_getc(void)
{
  while (uarte_rxwritepos == uarte_rxreadpos) {};
  uarte_rxreadpos = (uarte_rxreadpos + 1) & UARTE_RXBUFSIZEMASK;
  return uarte_rxbuf[uarte_rxreadpos];
}


static inline char uarte_lookatnextc(void)
{
  return uarte_rxbuf[(uarte_rxreadpos + 1) & UARTE_RXBUFSIZEMASK];
}


static inline void uarte_rx_flush(void)
{
  uarte_rxwritepos = uarte_rxreadpos = 0;
}

#endif // defined UARTE_USE_RX


//-------------------------------------------------------
// INIT routines
//-------------------------------------------------------
// ATTENTION:
// LPUART does not support 0.5 or 1.5 stop bits !!

// helper, usually should not be called itself
void _uarte_initprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
#if !(defined UARTE_USE_LPUART1 || defined UARTE_USE_LPUART1_PA2PA3 || defined UARTE_USE_LPUART1_PC1PC0)
LL_USART_InitTypeDef UART_InitStruct = {};

  UART_InitStruct.BaudRate = baud;
  UART_InitStruct.DataWidth = (parity != XUART_PARITY_NO) ? LL_USART_DATAWIDTH_9B : LL_USART_DATAWIDTH_8B;
  UART_InitStruct.StopBits = stopbits;
  UART_InitStruct.Parity = parity;
  UART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  UART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  UART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
#if defined STM32G4
  UART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
#endif

  LL_USART_Init(UARTE_UARTx, &UART_InitStruct);
#else
LL_LPUART_InitTypeDef UART_InitStruct = {};

  UART_InitStruct.BaudRate = baud;
  UART_InitStruct.DataWidth = (parity != LL_USART_PARITY_NONE) ? LL_LPUART_DATAWIDTH_9B : LL_LPUART_DATAWIDTH_8B;
  UART_InitStruct.StopBits = stopbits;
  UART_InitStruct.Parity = parity;
  UART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  UART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  UART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;

  LL_LPUART_Init(UARTE_UARTx, &UART_InitStruct);
#endif
}


void uarte_setprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
#if !(defined UARTE_USE_LPUART1 || defined UARTE_USE_LPUART1_PA2PA3 || defined UARTE_USE_LPUART1_PC1PC0)
  LL_USART_Disable(UARTE_UARTx); // must be disabled to configure some registers
  _uarte_initprotocol(baud, parity, stopbits);
  LL_USART_Enable(UARTE_UARTx);
#else
  LL_LPUART_Disable(UARTE_UARTx); // must be disabled to configure some registers
  _uarte_initprotocol(baud, parity, stopbits);
  LL_LPUART_Enable(UARTE_UARTx);
#endif
}


void uarte_setbaudrate(uint32_t baud)
{
  uarte_setprotocol(baud, XUART_PARITY_NO, UART_STOPBIT_1);
}


void uarte_tx_enablepin(FunctionalState flag)
{
#ifdef UARTE_USE_TX
  if (flag == ENABLE) {
    gpio_change_af(UARTE_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UARTE_IO_AF, IO_SPEED_VERYFAST);
  } else {
    gpio_change(UARTE_TX_IO, IO_MODE_INPUT_ANALOG, IO_SPEED_VERYFAST);
  }
#endif
}


void uarte_rx_enableisr(FunctionalState flag)
{
#ifdef UARTE_USE_RX
  if (flag == ENABLE) {
    // Enable Receive Data register not empty interrupt
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UARTE_UARTx);
#endif
    LL_USART_ReceiveData8(UARTE_UARTx); // read DR to clear RXNE and error flags
    LL_USART_EnableIT_RXNE(UARTE_UARTx);
  } else {
    LL_USART_DisableIT_RXNE(UARTE_UARTx);
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UARTE_UARTx);
#endif
  }
#endif
}


void uarte_init_isroff(void)
{
  LL_USART_Disable(UARTE_UARTx);

  // Initialize clocks for USART/LPUART, GPIO, AFIO
  rcc_init_afio();
  rcc_init_uart(UARTE_UARTx);
#ifdef STM32F1
#if defined UARTE_USE_UART1_PB6PB7
  LL_GPIO_AF_EnableRemap_USART1();
#endif
#if defined UARTE_USE_UART2_PD5PD6
  LL_GPIO_AF_EnableRemap_USART2();
#endif
#if defined UARTE_USE_UART3_PC10PC11
  LL_GPIO_AF_RemapPartial_USART3();
#endif
#endif // STM32F1

  // Configure pin USART/LPUART Tx as alternative function push-pull
#ifdef UARTE_USE_TX
  gpio_init_af(UARTE_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UARTE_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure pin USART/LPUART Rx as input with pull up
#ifdef UARTE_USE_RX
#ifndef UARTE_INVERTED
  gpio_init_af(UARTE_RX_IO, IO_MODE_INPUT_PU, UARTE_IO_AF, IO_SPEED_VERYFAST);
#else
  gpio_init_af(UARTE_RX_IO, IO_MODE_INPUT_PD, UARTE_IO_AF, IO_SPEED_VERYFAST);
#endif
#endif

  // Configure USART/LPUART
  _uarte_initprotocol(UARTE_BAUD, XUART_PARITY_NO, UART_STOPBIT_1);
#if !(defined UARTE_USE_LPUART1 || defined UARTE_USE_LPUART1_PA2PA3 || defined UARTE_USE_LPUART1_PC1PC0)
  LL_USART_ConfigAsyncMode(UARTE_UARTx);
#if defined STM32G4
  LL_USART_DisableFIFO(UARTE_UARTx);
  LL_USART_SetTXFIFOThreshold(UARTE_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(UARTE_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
#endif
#else
  LL_LPUART_DisableFIFO(UARTE_UARTx);
  LL_LPUART_SetTXFIFOThreshold(UARTE_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
  LL_LPUART_SetRXFIFOThreshold(UARTE_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
#endif

#ifdef UARTE_INVERTED
#ifdef UARTE_USE_TX
  LL_USART_SetTXPinLevel(UARTE_UARTx, LL_USART_TXPIN_LEVEL_INVERTED);
#endif
#ifdef UARTE_USE_RX
  LL_USART_SetRXPinLevel(UARTE_UARTx, LL_USART_RXPIN_LEVEL_INVERTED);
#endif
#endif

  // Configure NVIC channel
#if defined UARTE_USE_RX || defined UARTE_USE_TX_ISR
#ifdef UARTE_PREEMPTIONPRIORITY
  #error Legacy UARTE_PREEMPTIONPRIORITY used !
#endif
#ifdef UARTE_SUBPRIORITY
  #error Legacy UARTE_SUBPRIORITY used !
#endif
#ifdef UARTE_PRIORITY
#error Legacy UARTE_PRIORITY used !
#endif
#ifndef UARTE_IRQ_PRIORITY
  #define UARTE_IRQ_PRIORITY  15 // set priority to lowest
#endif
  nvic_irq_enable_w_priority(UARTE_IRQn, UARTE_IRQ_PRIORITY);
#endif

#ifdef UARTE_USE_TX_ISR
  // Disable Transmit Data Register empty interrupt
  LL_USART_DisableIT_TXE(UARTE_UARTx);
  LL_USART_DisableIT_TC(UARTE_UARTx);
  LL_USART_ClearFlag_TC(UARTE_UARTx);
  // Flush buffer
  uarte_txwritepos = uarte_txreadpos = 0;
#endif

#ifdef UARTE_USE_RX
  // Disable Receive Data register not empty interrupt
  LL_USART_DisableIT_RXNE(UARTE_UARTx);
#ifdef STM32F1
  LL_USART_ClearFlag_RXNE(UARTE_UARTx);
#endif
  // Flush buffer
  uarte_rxwritepos = uarte_rxreadpos = 0;
#ifdef UARTE_USE_RXERRORCOUNT
  uarte_errorcnt_rxnoise = 0;
  uarte_errorcnt_rxframe = 0;
  uarte_errorcnt_rxoverrun = 0;
#endif
#endif

  // Cube HAL for f103rc
  /* In asynchronous mode, the following bits must be kept cleared:
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(UARTE_UARTx->CR2, USART_CR2_CLKEN);
  CLEAR_BIT(UARTE_UARTx->CR3, USART_CR3_HDSEL);
#if defined USART_CR2_LINEN
  CLEAR_BIT(UARTE_UARTx->CR2, USART_CR2_LINEN);
#endif
#if defined USART_CR3_SCEN || defined USART_CR3_IREN // assume that always both are defined
  CLEAR_BIT(UARTE_UARTx->CR3, (USART_CR3_SCEN | USART_CR3_IREN));
#endif

#if defined STM32F7
  // reading the F722 manual I've added this
  CLEAR_BIT(UARTE_UARTx->CR1, (USART_CR1_EOBIE | USART_CR1_RTOIE | USART_CR1_CMIE | USART_CR1_PEIE | USART_CR1_IDLEIE));
  CLEAR_BIT(UARTE_UARTx->CR2, (USART_CR2_RTOEN | USART_CR2_CPOL | USART_CR2_CPHA | USART_CR2_LBCL | USART_CR2_LBDIE | USART_CR2_LBDL));
  CLEAR_BIT(UARTE_UARTx->CR3, (USART_CR3_TCBGTIE | USART_CR3_CTSIE | USART_CR3_EIE));
#if !defined UARTE_USE_RXERRORCOUNT
  // Disable ORE in the first place
  LL_USART_DisableOverrunDetect(UARTE_UARTx); // disables ORE, check how F1 is behaving
#endif
#endif

#if defined STM32G4 || defined STM32F3 || defined STM32WL || defined STM32F0
#if !defined UARTE_USE_RXERRORCOUNT
  LL_USART_DisableOverrunDetect(UARTE_UARTx);
#endif
#endif

  // Enable USART/LPUART
  LL_USART_Enable(UARTE_UARTx);

#if defined STM32G4 ||defined STM32F3 || defined STM32WL || defined STM32F0
  // Polling UART/LPUART initialisation
  while((!(LL_USART_IsActiveFlag_TEACK(UARTE_UARTx))) || (!(LL_USART_IsActiveFlag_REACK(UARTE_UARTx)))) {};
#endif
}


void uarte_init(void)
{
  uarte_init_isroff();
  uarte_rx_enableisr(ENABLE);
}


//-------------------------------------------------------
// System bootloader
//-------------------------------------------------------
// AN2606 Application note: STM32 microcontroller system memory boot mode

#if defined STM32F1
  // STM32F10xx
  #if defined UARTE_USE_UART1_PA9PA10
    #define UARTE_HAS_SYSTEMBOOT
  #endif
  // STM32F105xx/107x
  // STM32F10xxx XL-densit
  // TODO

#elif defined STM32F3
  // STM32F302xB(C)/303xB(C)
  #if defined UARTE_USE_UART1_PA9PA10 || defined UARTE_USE_UART2_PD5PD6
    #define UARTE_HAS_SYSTEMBOOT
  #endif
  // STM32F301xx/302x4(6/8)
  // STM32F302xD(E)/303xD(E)
  // STM32F303x4(6/8)/334xx/328xx
  //#if defined UARTE_USE_UART1_PA9PA10 || defined UARTE_USE_UART2_PA2PA3
  //  #define UARTE_HAS_SYSTEMBOOT
  //#endif

#elif defined STM32F7

  #error TODO

#elif defined STM32G4
  // STM32G431xx/441x
  // STM32G47xxx/48xx
  // STM32G491xx/4A1x
  #if defined UARTE_USE_UART1_PA9PA10 || defined UARTE_USE_UART2_PA2PA3 || defined UARTE_USE_UART3_PC10PC11
    #define UARTE_HAS_SYSTEMBOOT
  #endif

#elif defined STM32L4
  // STM32L412xx/422x
  // STM32L43xxx/44xx
  // STM32L45xxx/46xxx
  // STM32L47xxx/48xx
  #if defined UARTE_USE_UART1_PA9PA10 || defined UARTE_USE_UART2_PA2PA3 || defined UARTE_USE_UART3_PC10PC11
    #define UARTE_HAS_SYSTEMBOOT
  #endif

#elif defined STM32WL
  // STM32WLE5xx/55x
  #if defined UARTE_USE_UART1_PA9PA10 || defined UARTE_USE_UART2_PA2PA3
    #define UARTE_HAS_SYSTEMBOOT
  #endif

#elif defined STM32F0
  // STM32F070x6
  // STM32F070xB
  // STM32F071xx/072x
  #if defined UARTE_USE_UART1_PA9PA10 || defined UARTE_USE_UART2_PA14PA15
    #define UARTE_HAS_SYSTEMBOOT
  #endif
  // STM32F09xxx
  // TODO

#endif


uint8_t uarte_has_systemboot(void)
{
#ifdef UARTE_HAS_SYSTEMBOOT
    return 1;
#else
    return 0;
#endif
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_UARTE_H
