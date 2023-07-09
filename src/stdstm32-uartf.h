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
// #define UARTF_USE_UART1, UARTF_USE_UART2, UARTF_USE_UART3, UARTF_USE_UART3_REMAPPED, UARTF_USE_UART4, UARTF_USE_UART5
// #define UARTF_USE_LPUART1, UARTF_USE_LPUART1_REMAPPED
//
// #define UARTF_BAUD
//
// #define UARTF_USE_TX
// #define UARTF_TXBUFSIZE
// #define UARTF_USE_TX_ISR
// #define UARTF_USE_TX_DMA
//
// #define UARTF_USE_RX
// #define UARTF_RXBUFSIZE
//
// #define UARTF_IRQ_PRIORITY
//
// #define UARTF_USE_RXERRORCOUNT
//
// #define UARTF_RX_CALLBACK_FULL(c)
// #define UARTF_TC_CALLBACK() // ATTENTION: when used, one never should trigger TC isr without a byte written to the tx buffer
//
//*******************************************************
#ifndef STDSTM32_UARTF_H
#define STDSTM32_UARTF_H
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
  UART_STOPBIT_0_5 = LL_USART_STOPBITS_0_5, // not allowed for LPUART!
#endif
  UART_STOPBIT_1 = LL_USART_STOPBITS_1,
#ifdef LL_USART_STOPBITS_1_5
  UART_STOPBIT_1_5 = LL_USART_STOPBITS_1_5, // not allowed for LPUART!
#endif
  UART_STOPBIT_2 = LL_USART_STOPBITS_2,
  UART_STOPBIT_MAKEITU32 = UINT32_MAX,
} UARTSTOPBITENUM;

#endif


//-------------------------------------------------------
// Defines
//-------------------------------------------------------

#include "stdstm32-peripherals.h"

#if (defined UARTF_USE_TX_ISR) && (!defined UARTF_USE_TX)
  #error UARTF_USE_TX_ISR used without UARTF_USE_TX!
#endif
#if (defined UARTF_USE_RXERRORCOUNT) && (!defined UARTF_USE_RX)
  #error UARTF_USE_RXERRORCOUNT used without UARTF_USE_RX!
#endif

#ifdef UARTF_USE_TX_DMA
  #error UARTF_USE_TX_DMA not supported by this library!
#endif


#ifndef UARTF_BAUD
  #define UARTF_BAUD             115200UL // 9600UL
#endif


#ifdef UARTF_USE_TX_ISR
  #ifndef UARTF_TXBUFSIZE
    #define UARTF_TXBUFSIZE      256 // MUST be 2^N
  #endif
  #if UARTF_TXBUFSIZE < 2
    #error UARTF_TXBUFSIZE must be larger than 1 !
  #elif ((UARTF_TXBUFSIZE & (UARTF_TXBUFSIZE-1)) != 0)
    #error UARTF_TXBUFSIZE must be a power of 2 !
  #endif

  #define UARTF_TXBUFSIZEMASK  (UARTF_TXBUFSIZE-1)

  volatile char uartf_txbuf[UARTF_TXBUFSIZE];
  volatile uint16_t uartf_txwritepos; // pos at which the last byte was stored
  volatile uint16_t uartf_txreadpos; // pos at which the next byte is to be fetched
#endif


#ifdef UARTF_USE_RX
  #ifndef UARTF_RXBUFSIZE
    #define UARTF_RXBUFSIZE      256 //128 //MUST be 2^N
  #endif
  #if UARTF_RXBUFSIZE < 2
    #error UARTF_RXBUFSIZE must be larger than 1 !
  #elif ((UARTF_RXBUFSIZE & (UARTF_RXBUFSIZE-1)) != 0)
    #error UARTF_RXBUFSIZE must be a power of 2 !
  #endif

  #define UARTF_RXBUFSIZEMASK  (UARTF_RXBUFSIZE-1)

  volatile char uartf_rxbuf[UARTF_RXBUFSIZE];
  volatile uint16_t uartf_rxwritepos; // pos at which the last byte was stored
  volatile uint16_t uartf_rxreadpos; // pos at which the next byte is to be fetched

  #ifdef UARTF_USE_RXERRORCOUNT
  volatile uint32_t uartf_errorcnt_rxnoise;
  volatile uint32_t uartf_errorcnt_rxframe;
  volatile uint32_t uartf_errorcnt_rxoverrun;
  #endif
#endif


#if defined UARTF_USE_UART1 || defined UARTF_USE_UART1_REMAPPED || defined UARTF_USE_UART1_REMAPPED2
  #define UARTF_UARTx             USART1
  #ifdef UARTF_USE_UART1
    #define UARTF_TX_IO           IO_PA9
    #define UARTF_RX_IO           IO_PA10
  #elif defined UARTF_USE_UART1_REMAPPED
    #define UARTF_TX_IO           IO_PB6
    #define UARTF_RX_IO           IO_PB7
  #elif defined UARTF_USE_UART1_REMAPPED2 // only G4
    #define UARTF_TX_IO           IO_PC4 // on G4
    #define UARTF_RX_IO           IO_PC5
  #endif
  #define UARTF_IO_AF             IO_AF_7
  #define UARTF_IRQn              USART1_IRQn
  #define UARTF_IRQHandler        USART1_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UARTF_USE_UART2 || defined UARTF_USE_UART2_REMAPPED || defined UARTF_USE_UART2_REMAPPED2 || defined UARTF_USE_UART2_REMAPPED3
  #define UARTF_UARTx             USART2
  #ifdef UARTF_USE_UART2
    #define UARTF_TX_IO           IO_PA2
    #define UARTF_RX_IO           IO_PA3
  #elif defined UARTF_USE_UART2_REMAPPED
    #define UARTF_TX_IO           IO_PD5
    #define UARTF_RX_IO           IO_PD6
  #elif defined UARTF_USE_UART2_REMAPPED2 // only G4
    #define UARTF_TX_IO           IO_PB3
    #define UARTF_RX_IO           IO_PB4
  #elif defined UARTF_USE_UART2_REMAPPED3 // only G4 // ATTENTION: PA14 overlaps with SWCLK
    #define UARTF_TX_IO           IO_PA14
    #define UARTF_RX_IO           IO_PA15
  #endif
  #define UARTF_IO_AF             IO_AF_7
  #define UARTF_IRQn              USART2_IRQn
  #define UARTF_IRQHandler        USART2_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UARTF_USE_UART3 || defined UARTF_USE_UART3_REMAPPED
  #define UARTF_UARTx             USART3
  #if defined UARTF_USE_UART3 && defined STM32F3
    #define UARTF_TX_IO           IO_PB8
    #define UARTF_RX_IO           IO_PB9
  #elif defined UARTF_USE_UART3
    #define UARTF_TX_IO           IO_PB10
    #define UARTF_RX_IO           IO_PB11
  #else
    #define UARTF_TX_IO           IO_PC10
    #define UARTF_RX_IO           IO_PC11
  #endif
  #define UARTF_IO_AF             IO_AF_7
  #define UARTF_IRQn              USART3_IRQn
  #define UARTF_IRQHandler        USART3_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
    #error UART3 NOT AVAILABLE !
  #endif

#elif defined UARTF_USE_UART4 || defined UARTF_USE_UART4_REMAPPED
  #define UARTF_UARTx             UART4
  #ifdef UARTF_USE_UART4
    #define UARTF_TX_IO           IO_PC10
    #define UARTF_RX_IO           IO_PC11
  #else
    #define UARTF_TX_IO           IO_PA0 // only F7 !
    #define UARTF_RX_IO           IO_PA1
  #endif
  #if defined STM32F1
    #define UARTF_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F7
    #define UARTF_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UARTF_IO_AF           IO_AF_5
  #endif
  #define UARTF_IRQn              UART4_IRQn
  #define UARTF_IRQHandler        UART4_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
    #warning UARTF IO_AF & DMA for F3 still TODO
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART4 NOT AVAILABLE !
  #endif

#elif defined UARTF_USE_UART5
  #define UARTF_UARTx             UART5
  #define UARTF_TX_IO             IO_PC12
  #define UARTF_RX_IO             IO_PD2
  #define UARTF_IRQn              UART5_IRQn
  #define UARTF_IRQHandler        UART5_IRQHandler
  #if defined STM32F1
    #define UARTF_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F3
    #warning UARTF IO_AF & DMA for F3 still TODO //TODO
  #elif defined STM32F7
    #define UARTF_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UARTF_IO_AF           IO_AF_5
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART5 NOT AVAILABLE !
  #endif

#elif defined UARTF_USE_UART6 || defined UARTF_USE_UART6_REMAPPED
  #define UARTF_UARTx             USART6
  #ifdef UARTF_USE_UART6
    #define UARTF_TX_IO           IO_PC6
    #define UARTF_RX_IO           IO_PC7
  #else
    #define UARTF_TX_IO           IO_PG14
    #define UARTF_RX_IO           IO_PG9
  #endif
  #define UARTF_IO_AF             IO_AF_8
  #define UARTF_IRQn              USART6_IRQn
  #define UARTF_IRQHandler        USART6_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART6 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTF_USE_UART7 || defined UARTF_USE_UART7_REMAPPED
  #define UARTF_UARTx             UART7
  #ifdef UARTF_USE_UART7
    #define UARTF_TX_IO           IO_PE8
    #define UARTF_RX_IO           IO_PE7
  #else
    #define UARTF_TX_IO           IO_PF7
    #define UARTF_RX_IO           IO_PF6
  #endif
  #define UARTF_IO_AF             IO_AF_8
  #define UARTF_IRQn              UART7_IRQn
  #define UARTF_IRQHandler        UART7_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART7 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTF_USE_UART8
  #define UARTF_UARTx             UART8
  #define UARTF_TX_IO             IO_PE1
  #define UARTF_RX_IO             IO_PE0
  #define UARTF_IO_AF             IO_AF_8
  #define UARTF_IRQn              UART8_IRQn
  #define UARTF_IRQHandler        UART8_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART8 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTF_USE_LPUART1 || defined UARTF_USE_LPUART1_REMAPPED
  #define UARTF_UARTx             LPUART1
  #ifdef UARTF_USE_LPUART1
    #define UARTF_TX_IO           IO_PA2
    #define UARTF_RX_IO           IO_PA3
    #ifdef STM32G4
      #define UARTF_IO_AF         IO_AF_12 // G4
    #else
      #define UARTF_IO_AF         IO_AF_8 // WL
    #endif
  #elif defined UARTF_USE_LPUART1_REMAPPED
    #define UARTF_TX_IO           IO_PC1
    #define UARTF_RX_IO           IO_PC0
    #define UARTF_IO_AF           IO_AF_8
  #endif
  #define UARTF_IRQn              LPUART1_IRQn
  #define UARTF_IRQHandler        LPUART1_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32F7
    #error LPUART1 NOT AVAILABLE !
  #elif defined STM32G4
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
  #endif

#else
  #error No UARTF_USE_UARTx defined!
#endif


// allows us to overwrite the default pin assignment
#ifdef UARTF_USE_TX_IO
  #undef UARTF_TX_IO
  #define UARTF_TX_IO             UARTF_USE_TX_IO
#endif
#ifdef UARTF_USE_RX_IO
  #undef UARTF_RX_IO
  #define UARTF_RX_IO             UARTF_USE_RX_IO
#endif
#ifdef UARTF_USE_IO_AF
  #undef UARTF_IO_AF
  #define UARTF_IO_AF             UARTF_USE_IO_AF
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

#if defined UART_USE_LPUART1 || defined UART_USE_LPUART1_REMAPPED
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
#elif defined STM32F3 || defined STM32F7 || defined STM32L4
  #define REG_DR        TDR
  #define REG_SR        ISR
  #define FLAG_SR_RXNE  LL_USART_ISR_RXNE
  #define FLAG_SR_ORE   LL_USART_ISR_ORE
  #define FLAG_SR_NE    LL_USART_ISR_NE
  #define FLAG_SR_FE    LL_USART_ISR_FE
  #define FLAG_SR_TXE   LL_USART_ISR_TXE
  #define FLAG_SR_TC    LL_USART_ISR_TC
#elif defined STM32G4 || defined STM32WL
  #define REG_DR        TDR
  #define REG_SR        ISR
  #define FLAG_SR_RXNE  LL_USART_ISR_RXNE_RXFNE
  #define FLAG_SR_ORE   LL_USART_ISR_ORE
  #define FLAG_SR_NE    LL_USART_ISR_NE
  #define FLAG_SR_FE    LL_USART_ISR_FE
  #define FLAG_SR_TXE   LL_USART_ISR_TXE_TXFNF
  #define FLAG_SR_TC    LL_USART_ISR_TC
#endif


//-------------------------------------------------------
// ISR routine
//-------------------------------------------------------
#if defined UARTF_USE_RX || defined UARTF_USE_TX_ISR

void UARTF_IRQHandler(void)
{
  // Cube HAL for f103rc is also calling this first independent on irqs
  uint16_t usart_sr = LL_USART_ReadReg(UARTF_UARTx, REG_SR); // read USART_SR register

#ifdef UARTF_USE_RX
  if ((usart_sr & FLAG_SR_RXNE) && LL_USART_IsEnabledIT_RXNE(UARTF_UARTx)) {
#ifdef UARTF_USE_RXERRORCOUNT
    if (usart_sr & FLAG_SR_ORE) uartf_errorcnt_rxoverrun++;
    if (usart_sr & FLAG_SR_NE) uartf_errorcnt_rxnoise++;
    if (usart_sr & FLAG_SR_FE) uartf_errorcnt_rxframe++;
#endif
    // Cube LL driver (for f103?) says that reading SR and then writing to DR will clear error flags, so do it after error checks
    uint8_t usart_dr = LL_USART_ReceiveData8(UARTF_UARTx); // read USART_DR register, clears RXNE and RX error flags
#ifdef UARTF_RX_CALLBACK_FULL
    UARTF_RX_CALLBACK_FULL(usart_dr);
#else
    uint16_t next = (uartf_rxwritepos + 1) & UARTF_RXBUFSIZEMASK;
    if (uartf_rxreadpos != next) { // fifo not full
      uartf_rxbuf[next] = usart_dr;
      uartf_rxwritepos = next;
    }
#endif
  }
#if defined STM32F1
  // ORE is enabled along with RXNE, so we need to handle it
  // ORE bit is reset by read to USART_SR followed by read to USART_DR
  if (usart_sr & FLAG_SR_ORE) {
    LL_USART_ReceiveData8(UARTF_UARTx); // read USART_DR register, clears RXNE and RX error flags
  }
#elif defined STM32F3
  // ORE is enabled along with RXNE, but can be cleared by writing 1 to ICR
  LL_USART_WriteReg(UARTF_UARTx, ICR, (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NCF | LL_USART_ICR_FECF | LL_USART_ICR_PECF));
#elif defined STM32F7
  #warning UARTF F7 check clear flags, should be LL !?
  // these are cleared by write 1 to the ICR register, not by a read of DR register!
  LL_USART_WriteReg(UARTF_UARTx, ICR, (USART_ICR_IDLECF | USART_ICR_ORECF | USART_ICR_NCF | USART_ICR_FECF | USART_ICR_PECF));
#elif defined STM32G4 || defined STM32L4 || defined STM32WL
  LL_USART_WriteReg(UARTF_UARTx, ICR, (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NECF | LL_USART_ICR_FECF | LL_USART_ICR_PECF));
#endif
#endif

#ifdef UARTF_USE_TX_ISR
  if ((usart_sr & FLAG_SR_TXE) && LL_USART_IsEnabledIT_TXE(UARTF_UARTx)) {
    if (uartf_txwritepos != uartf_txreadpos) { // fifo not empty
      uartf_txreadpos = (uartf_txreadpos + 1) & UARTF_TXBUFSIZEMASK;
      LL_USART_TransmitData8(UARTF_UARTx, uartf_txbuf[uartf_txreadpos]); // write to USART_DR register, clears TXE flag, as well as TC
    } else {
      LL_USART_DisableIT_TXE(UARTF_UARTx); // disable interrupt when empty
#ifdef UARTF_TC_CALLBACK
      // ATTENTION: doesn't distinguish between empty fifo and last byte just send!
      LL_USART_ClearFlag_TC(UARTF_UARTx); // now enable TC interrupt
      LL_USART_EnableIT_TC(UARTF_UARTx);
#endif
    }
  }

#ifdef UARTF_TC_CALLBACK
  if ((usart_sr & FLAG_SR_TC) && LL_USART_IsEnabledIT_TC(UARTF_UARTx)) {
    LL_USART_DisableIT_TC(UARTF_UARTx);
    LL_USART_ClearFlag_TC(UARTF_UARTx);
    UARTF_TC_CALLBACK();
  }
#endif
#endif
}

#endif // defined UARTF_USE_RX || defined UARTF_USE_TX_ISR


//-------------------------------------------------------
// TX routines
//-------------------------------------------------------
#ifdef UARTF_USE_TX

uint16_t uartf_putc(char c)
{
#ifndef UARTF_USE_TX_ISR
  while (!LL_USART_IsActiveFlag_TC(UARTF_UARTx)) {}
  LL_USART_TransmitData8(UARTF_UARTx, c);
  return 1;
#else
  uint16_t next = (uartf_txwritepos + 1) & UARTF_TXBUFSIZEMASK;
  if (uartf_txreadpos != next) { // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
    uartf_txbuf[next] = c;
    uartf_txwritepos = next;
    LL_USART_EnableIT_TXE(UARTF_UARTx); // enable interrupt // then TDR is empty, the TXE bit is set, and hence the isr is triggered
    return 1;
  }
  return 0;
#endif
}


void uartf_puts(const char *s)
{
  while (*s) {
    uartf_putc(*s);
    s++;
  }
}


void uartf_putbuf(uint8_t* buf, uint16_t len)
{
uint16_t i;

  for (i = 0; i < len; i++) uartf_putc(buf[i]);
}


#ifdef UARTF_USE_TX_ISR
uint16_t uartf_tx_notfull(void)
{
  uint16_t next = (uartf_txwritepos + 1) & UARTF_TXBUFSIZEMASK;
  return (uartf_txreadpos != next); // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
}


static inline uint16_t uartf_tx_isempty(void)
{
  if (uartf_txwritepos == uartf_txreadpos) return 1;
  return 0;
}


static inline void uartf_tx_flush(void)
{
  uartf_txwritepos = uartf_txreadpos = 0;
}
#else
static inline void uartf_tx_flush(void) {}
#endif

#endif // defined UARTF_USE_TX


//-------------------------------------------------------
// RX routines
//-------------------------------------------------------
#ifdef UARTF_USE_RX

static inline uint16_t uartf_rx_available(void)
{
  if (uartf_rxwritepos == uartf_rxreadpos) return 0; // fifo empty
  return 1;
}


uint16_t uartf_rx_bytesavailable(void)
{
int16_t d;

  d = (int16_t)uartf_rxwritepos - (int16_t)uartf_rxreadpos;
  return (d < 0) ? d + (UARTF_RXBUFSIZEMASK + 1) : d;
}


char uartf_getc(void)
{
  while (uartf_rxwritepos == uartf_rxreadpos) {};
  uartf_rxreadpos = (uartf_rxreadpos + 1) & UARTF_RXBUFSIZEMASK;
  return uartf_rxbuf[uartf_rxreadpos];
}


static inline char uartf_lookatnextc(void)
{
  return uartf_rxbuf[(uartf_rxreadpos + 1) & UARTF_RXBUFSIZEMASK];
}


static inline void uartf_rx_flush(void)
{
  uartf_rxwritepos = uartf_rxreadpos = 0;
}

#endif // defined UARTF_USE_RX


//-------------------------------------------------------
// INIT routines
//-------------------------------------------------------
// ATTENTION:
// LPUART does not support 0.5 or 1.5 stop bits !!

// helper, usually should not be called itself
void _uartf_initprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
#if !(defined UARTF_USE_LPUART1 || defined UARTF_USE_LPUART1_REMAPPED)
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

  LL_USART_Init(UARTF_UARTx, &UART_InitStruct);
#else
LL_LPUART_InitTypeDef UART_InitStruct = {};

  UART_InitStruct.BaudRate = baud;
  UART_InitStruct.DataWidth = (parity != LL_USART_PARITY_NONE) ? LL_LPUART_DATAWIDTH_9B : LL_LPUART_DATAWIDTH_8B;
  UART_InitStruct.StopBits = stopbits;
  UART_InitStruct.Parity = parity;
  UART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  UART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  UART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;

  LL_LPUART_Init(UARTF_UARTx, &UART_InitStruct);
#endif
}


void uartf_setprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
#if !(defined UARTF_USE_LPUART1 || defined UARTF_USE_LPUART1_REMAPPED)
  LL_USART_Disable(UARTF_UARTx); // must be disabled to configure some registers
  _uartf_initprotocol(baud, parity, stopbits);
  LL_USART_Enable(UARTF_UARTx);
#else
  LL_LPUART_Disable(UARTF_UARTx); // must be disabled to configure some registers
  _uartf_initprotocol(baud, parity, stopbits);
  LL_LPUART_Enable(UARTF_UARTx);
#endif
}


void uartf_setbaudrate(uint32_t baud)
{
  uartf_setprotocol(baud, XUART_PARITY_NO, UART_STOPBIT_1);
}


void uartf_tx_enablepin(FunctionalState flag)
{
#ifdef UARTF_USE_TX
  if (flag == ENABLE) {
    gpio_change_af(UARTF_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UARTF_IO_AF, IO_SPEED_VERYFAST);
  } else {
    gpio_change(UARTF_TX_IO, IO_MODE_INPUT_ANALOG, IO_SPEED_VERYFAST);
  }
#endif
}


void uartf_rx_enableisr(FunctionalState flag)
{
#ifdef UARTF_USE_RX
  if (flag == ENABLE) {
    // Enable Receive Data register not empty interrupt
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UARTF_UARTx);
#endif
    LL_USART_ReceiveData8(UARTF_UARTx); // read DR to clear RXNE and error flags
    LL_USART_EnableIT_RXNE(UARTF_UARTx);
  } else {
    LL_USART_DisableIT_RXNE(UARTF_UARTx);
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UARTF_UARTx);
#endif
  }
#endif
}


void uartf_init_isroff(void)
{
  LL_USART_Disable(UARTF_UARTx);

  // Initialize clocks for USART/LPUART, GPIO, AFIO
  rcc_init_afio();
  rcc_init_uart(UARTF_UARTx);
#ifdef STM32F1
#if defined UARTF_USE_UART1_REMAPPED
  LL_GPIO_AF_EnableRemap_USART1();
#endif
#if defined UARTF_USE_UART2_REMAPPED
  LL_GPIO_AF_EnableRemap_USART2();
#endif
#if defined UARTF_USE_UART3_REMAPPED
  LL_GPIO_AF_RemapPartial_USART3();
#endif
#endif // STM32F1

  // Configure pin USART/LPUART Tx as alternative function push-pull
#ifdef UARTF_USE_TX
  gpio_init_af(UARTF_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UARTF_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure pin USART/LPUART Rx as input with pull up
#ifdef UARTF_USE_RX
  gpio_init_af(UARTF_RX_IO, IO_MODE_INPUT_PU, UARTF_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure USART/LPUART
  _uartf_initprotocol(UARTF_BAUD, XUART_PARITY_NO, UART_STOPBIT_1);
#if !(defined UARTF_USE_LPUART1 || defined UARTF_USE_LPUART1_REMAPPED)
  LL_USART_ConfigAsyncMode(UARTF_UARTx);
#if defined STM32G4
  LL_USART_DisableFIFO(UARTF_UARTx);
  LL_USART_SetTXFIFOThreshold(UARTF_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(UARTF_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
#endif
#else
  LL_LPUART_DisableFIFO(UARTF_UARTx);
  LL_LPUART_SetTXFIFOThreshold(UARTF_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
  LL_LPUART_SetRXFIFOThreshold(UARTF_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
#endif

  // Configure NVIC channel
#if defined UARTF_USE_RX || defined UARTF_USE_TX_ISR
#ifdef UARTF_PREEMPTIONPRIORITY
  #error Legacy UARTF_PREEMPTIONPRIORITY used !
#endif
#ifdef UARTF_SUBPRIORITY
  #error Legacy UARTF_SUBPRIORITY used !
#endif
#ifdef UARTF_PRIORITY
#error Legacy UARTF_PRIORITY used !
#endif
#ifndef UARTF_IRQ_PRIORITY
  #define UARTF_IRQ_PRIORITY  15 // set priority to lowest
#endif
  nvic_irq_enable_w_priority(UARTF_IRQn, UARTF_IRQ_PRIORITY);
#endif

#ifdef UARTF_USE_TX_ISR
  // Disable Transmit Data Register empty interrupt
  LL_USART_DisableIT_TXE(UARTF_UARTx);
  LL_USART_DisableIT_TC(UARTF_UARTx);
  LL_USART_ClearFlag_TC(UARTF_UARTx);
  // Flush buffer
  uartf_txwritepos = uartf_txreadpos = 0;
#endif

#ifdef UARTF_USE_RX
  // Disable Receive Data register not empty interrupt
  LL_USART_DisableIT_RXNE(UARTF_UARTx);
#ifdef STM32F1
  LL_USART_ClearFlag_RXNE(UARTF_UARTx);
#endif
  // Flush buffer
  uartf_rxwritepos = uartf_rxreadpos = 0;
#ifdef UARTF_USE_RXERRORCOUNT
  uartf_errorcnt_rxnoise = 0;
  uartf_errorcnt_rxframe = 0;
  uartf_errorcnt_rxoverrun = 0;
#endif
#endif

  // Cube HAL for f103rc
  /* In asynchronous mode, the following bits must be kept cleared:
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(UARTF_UARTx->CR2, USART_CR2_CLKEN);
  CLEAR_BIT(UARTF_UARTx->CR3, USART_CR3_HDSEL);
#if defined USART_CR2_LINEN
  CLEAR_BIT(UARTF_UARTx->CR2, USART_CR2_LINEN);
#endif
#if defined USART_CR3_SCEN || defined USART_CR3_IREN // assume that always both are defined
  CLEAR_BIT(UARTF_UARTx->CR3, (USART_CR3_SCEN | USART_CR3_IREN));
#endif

#if defined STM32F7
  // reading the F722 manual I've added this
  CLEAR_BIT(UARTF_UARTx->CR1, (USART_CR1_EOBIE | USART_CR1_RTOIE | USART_CR1_CMIE | USART_CR1_PEIE | USART_CR1_IDLEIE));
  CLEAR_BIT(UARTF_UARTx->CR2, (USART_CR2_RTOEN | USART_CR2_CPOL | USART_CR2_CPHA | USART_CR2_LBCL | USART_CR2_LBDIE | USART_CR2_LBDL));
  CLEAR_BIT(UARTF_UARTx->CR3, (USART_CR3_TCBGTIE | USART_CR3_CTSIE | USART_CR3_EIE));
#if !defined UARTF_USE_RXERRORCOUNT
  // Disable ORE in the first place
  LL_USART_DisableOverrunDetect(UARTF_UARTx); // disables ORE, check how F1 is behaving
#endif
#endif

#if defined STM32G4 || defined STM32WL
#if !defined UARTF_USE_RXERRORCOUNT
  LL_USART_DisableOverrunDetect(UARTF_UARTx);
#endif
#endif

  // Enable USART/LPUART
  LL_USART_Enable(UARTF_UARTx);

#if defined STM32G4 || defined STM32WL
  // Polling UART/LPUART initialisation
  while((!(LL_USART_IsActiveFlag_TEACK(UARTF_UARTx))) || (!(LL_USART_IsActiveFlag_REACK(UARTF_UARTx)))) {};
#endif
}


void uartf_init(void)
{
  uartf_init_isroff();
  uartf_rx_enableisr(ENABLE);
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_UARTF_H
