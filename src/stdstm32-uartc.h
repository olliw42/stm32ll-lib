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
// #define UARTC_USE_UART1, UARTC_USE_UART2, UARTC_USE_UART3, UARTC_USE_UART3_REMAPPED, UARTC_USE_UART4, UARTC_USE_UART5
// #define UARTC_USE_LPUART1, UARTC_USE_LPUART1_REMAPPED
//
// #define UARTC_BAUD
//
// #define UARTC_USE_TX
// #define UARTC_TXBUFSIZE
// #define UARTC_USE_TX_ISR
// #define UARTC_USE_TX_DMA
//
// #define UARTC_USE_RX
// #define UARTC_RXBUFSIZE
//
// #define UARTC_IRQ_PRIORITY
//
// #define UARTC_USE_RXERRORCOUNT
//
// #define UARTC_RX_CALLBACK_FULL(c)
// #define UARTC_TC_CALLBACK() // ATTENTION: when used, one never should trigger TC isr without a byte written to the tx buffer
//
//*******************************************************
#ifndef STDSTM32_UARTC_H
#define STDSTM32_UARTC_H
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
  UART_STOPBIT_0_5 = LL_USART_STOPBITS_0_5, // not allowed for LPUART!
  UART_STOPBIT_1 = LL_USART_STOPBITS_1,
  UART_STOPBIT_1_5 = LL_USART_STOPBITS_1_5, // not allowed for LPUART!
  UART_STOPBIT_2 = LL_USART_STOPBITS_2,
  UART_STOPBIT_MAKEITU32 = UINT32_MAX,
} UARTSTOPBITENUM;

#endif


//-------------------------------------------------------
// Defines
//-------------------------------------------------------

#include "stdstm32-peripherals.h"

#if (defined UARTC_USE_TX_ISR) && (!defined UARTC_USE_TX)
  #error UARTC_USE_TX_ISR used without UARTC_USE_TX!
#endif
#if (defined UARTC_USE_RXERRORCOUNT) && (!defined UARTC_USE_RX)
  #error UARTC_USE_RXERRORCOUNT used without UARTC_USE_RX!
#endif

#ifdef UARTC_USE_TX_DMA
  #error UARTC_USE_TX_DMA not supported by this library!
#endif


#ifndef UARTC_BAUD
  #define UARTC_BAUD             115200UL // 9600UL
#endif


#ifdef UARTC_USE_TX_ISR
  #ifndef UARTC_TXBUFSIZE
    #define UARTC_TXBUFSIZE      256 // MUST be 2^N
  #endif
  #if UARTC_TXBUFSIZE < 2
    #error UARTC_TXBUFSIZE must be larger than 1 !
  #elif ((UARTC_TXBUFSIZE & (UARTC_TXBUFSIZE-1)) != 0)
    #error UARTC_TXBUFSIZE must be a power of 2 !
  #endif

  #define UARTC_TXBUFSIZEMASK  (UARTC_TXBUFSIZE-1)

  volatile char uartc_txbuf[UARTC_TXBUFSIZE];
  volatile uint16_t uartc_txwritepos; // pos at which the last byte was stored
  volatile uint16_t uartc_txreadpos; // pos at which the next byte is to be fetched
#endif


#ifdef UARTC_USE_RX
  #ifndef UARTC_RXBUFSIZE
    #define UARTC_RXBUFSIZE      256 //128 //MUST be 2^N
  #endif
  #if UARTC_RXBUFSIZE < 2
    #error UARTC_RXBUFSIZE must be larger than 1 !
  #elif ((UARTC_RXBUFSIZE & (UARTC_RXBUFSIZE-1)) != 0)
    #error UARTC_RXBUFSIZE must be a power of 2 !
  #endif

  #define UARTC_RXBUFSIZEMASK  (UARTC_RXBUFSIZE-1)

  volatile char uartc_rxbuf[UARTC_RXBUFSIZE];
  volatile uint16_t uartc_rxwritepos; // pos at which the last byte was stored
  volatile uint16_t uartc_rxreadpos; // pos at which the next byte is to be fetched

  #ifdef UARTC_USE_RXERRORCOUNT
  volatile uint32_t uartc_errorcnt_rxnoise;
  volatile uint32_t uartc_errorcnt_rxframe;
  volatile uint32_t uartc_errorcnt_rxoverrun;
  #endif
#endif


#if defined UARTC_USE_UART1 || defined UARTC_USE_UART1_REMAPPED || defined UARTC_USE_UART1_REMAPPED2
  #define UARTC_UARTx             USART1
  #ifdef UARTC_USE_UART1
    #define UARTC_TX_IO           IO_PA9
    #define UARTC_RX_IO           IO_PA10
  #elif defined UARTC_USE_UART1_REMAPPED
    #define UARTC_TX_IO           IO_PB6
    #define UARTC_RX_IO           IO_PB7
  #elif defined UARTC_USE_UART1_REMAPPED2 // only G4
    #define UARTC_TX_IO           IO_PC4 // on G4
    #define UARTC_RX_IO           IO_PC5
  #endif
  #define UARTC_IO_AF             IO_AF_7
  #define UARTC_IRQn              USART1_IRQn
  #define UARTC_IRQHandler        USART1_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UARTC_USE_UART2 || defined UARTC_USE_UART2_REMAPPED || defined UARTC_USE_UART2_REMAPPED2 || defined UARTC_USE_UART2_REMAPPED3
  #define UARTC_UARTx             USART2
  #ifdef UARTC_USE_UART2
    #define UARTC_TX_IO           IO_PA2
    #define UARTC_RX_IO           IO_PA3
  #elif defined UARTC_USE_UART2_REMAPPED
    #define UARTC_TX_IO           IO_PD5
    #define UARTC_RX_IO           IO_PD6
  #elif defined UARTC_USE_UART2_REMAPPED2 // only G4
    #define UARTC_TX_IO           IO_PB3
    #define UARTC_RX_IO           IO_PB4
  #elif defined UARTC_USE_UART2_REMAPPED3 // only G4 // ATTENTION: PA14 overlaps with SWCLK
    #define UARTC_TX_IO           IO_PA14
    #define UARTC_RX_IO           IO_PA15
  #endif
  #define UARTC_IO_AF             IO_AF_7
  #define UARTC_IRQn              USART2_IRQn
  #define UARTC_IRQHandler        USART2_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UARTC_USE_UART3 || defined UARTC_USE_UART3_REMAPPED
  #define UARTC_UARTx             USART3
  #if defined UARTC_USE_UART3 && defined STM32F3
    #define UARTC_TX_IO           IO_PB8
    #define UARTC_RX_IO           IO_PB9
  #elif defined UARTC_USE_UART3
    #define UARTC_TX_IO           IO_PB10
    #define UARTC_RX_IO           IO_PB11
  #else
    #define UARTC_TX_IO           IO_PC10
    #define UARTC_RX_IO           IO_PC11
  #endif
  #define UARTC_IO_AF             IO_AF_7
  #define UARTC_IRQn              USART3_IRQn
  #define UARTC_IRQHandler        USART3_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
    #error UART3 NOT AVAILABLE !
  #endif

#elif defined UARTC_USE_UART4 || defined UARTC_USE_UART4_REMAPPED
  #define UARTC_UARTx             UART4
  #ifdef UARTC_USE_UART4
    #define UARTC_TX_IO           IO_PC10
    #define UARTC_RX_IO           IO_PC11
  #else
    #define UARTC_TX_IO           IO_PA0 // only F7 !
    #define UARTC_RX_IO           IO_PA1
  #endif
  #define UARTC_IRQn              UART4_IRQn
  #define UARTC_IRQHandler        UART4_IRQHandler
  #if defined STM32F1
    #define UARTC_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F3
    #warning UARTC IO_AF & DMA for F3 still TODO
  #elif defined STM32F7
    #define UARTC_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UARTC_IO_AF           IO_AF_5
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART4 NOT AVAILABLE !
  #endif

#elif defined UARTC_USE_UART5
  #define UARTC_UARTx             UART5
  #define UARTC_TX_IO             IO_PC12
  #define UARTC_RX_IO             IO_PD2
  #define UARTC_IRQn              UART5_IRQn
  #define UARTC_IRQHandler        UART5_IRQHandler
  #if defined STM32F1
    #define UARTC_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F3
    #warning UARTC IO_AF & DMA for F3 still TODO //TODO
  #elif defined STM32F7
    #define UARTC_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UARTC_IO_AF           IO_AF_5
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART5 NOT AVAILABLE !
  #endif

#elif defined UARTC_USE_UART6 || defined UARTC_USE_UART6_REMAPPED
  #define UARTC_UARTx             USART6
  #ifdef UARTC_USE_UART6
    #define UARTC_TX_IO           IO_PC6
    #define UARTC_RX_IO           IO_PC7
  #else
    #define UARTC_TX_IO           IO_PG14
    #define UARTC_RX_IO           IO_PG9
  #endif
  #define UARTC_IO_AF             IO_AF_8
  #define UARTC_IRQn              USART6_IRQn
  #define UARTC_IRQHandler        USART6_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART6 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTC_USE_UART7 || defined UARTC_USE_UART7_REMAPPED
  #define UARTC_UARTx             UART7
  #ifdef UARTC_USE_UART7
    #define UARTC_TX_IO           IO_PE8
    #define UARTC_RX_IO           IO_PE7
  #else
    #define UARTC_TX_IO           IO_PF7
    #define UARTC_RX_IO           IO_PF6
  #endif
  #define UARTC_IO_AF             IO_AF_8
  #define UARTC_IRQn              UART7_IRQn
  #define UARTC_IRQHandler        UART7_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART7 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTC_USE_UART8
  #define UARTC_UARTx             UART8
  #define UARTC_TX_IO             IO_PE1
  #define UARTC_RX_IO             IO_PE0
  #define UARTC_IO_AF             IO_AF_8
  #define UARTC_IRQn              UART8_IRQn
  #define UARTC_IRQHandler        UART8_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART8 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTC_USE_LPUART1 || defined UARTC_USE_LPUART1_REMAPPED
  #define UARTC_UARTx             LPUART1
  #ifdef UARTC_USE_LPUART1
    #define UARTC_TX_IO           IO_PA2
    #define UARTC_RX_IO           IO_PA3
    #ifdef STM32G4
      #define UARTC_IO_AF         IO_AF_12 // G4
    #else
      #define UARTC_IO_AF         IO_AF_8 // WL
    #endif
  #elif defined UARTC_USE_LPUART1_REMAPPED
    #define UARTC_TX_IO           IO_PC1
    #define UARTC_RX_IO           IO_PC0
    #define UARTC_IO_AF           IO_AF_8
  #endif
  #define UARTC_IRQn              LPUART1_IRQn
  #define UARTC_IRQHandler        LPUART1_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32F7
    #error LPUART1 NOT AVAILABLE !
  #elif defined STM32G4
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
  #endif

#else
  #error No UARTC_USE_UARTx defined!
#endif


// allows us to overwrite the default pin assignment
#ifdef UARTC_USE_TX_IO
  #undef UARTC_TX_IO
  #define UARTC_TX_IO             UARTC_USE_TX_IO
#endif
#ifdef UARTC_USE_RX_IO
  #undef UARTC_RX_IO
  #define UARTC_RX_IO             UARTC_USE_RX_IO
#endif
#ifdef UARTC_USE_IO_AF
  #undef UARTC_IO_AF
  #define UARTC_IO_AF             UARTC_USE_IO_AF
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
  #define FLAG_SR_RXNE  USART_SR_RXNE
  #define FLAG_SR_ORE   USART_SR_ORE
  #define FLAG_SR_NE    USART_SR_NE
  #define FLAG_SR_FE    USART_SR_FE
  #define FLAG_SR_TXE   USART_SR_TXE
  #define FLAG_SR_TC    USART_SR_TC
#elif defined STM32F3 || defined STM32F7 || defined STM32L4
  #define REG_DR        TDR
  #define REG_SR        ISR
  #define FLAG_SR_RXNE  USART_ISR_RXNE
  #define FLAG_SR_ORE   USART_ISR_ORE
  #define FLAG_SR_NE    USART_ISR_NE
  #define FLAG_SR_FE    USART_ISR_FE
  #define FLAG_SR_TXE   USART_ISR_TXE
  #define FLAG_SR_TC    USART_ISR_TC
#elif defined STM32G4 || defined STM32WL
  #define REG_DR        TDR
  #define REG_SR        ISR
  #define FLAG_SR_RXNE  USART_ISR_RXNE_RXFNE
  #define FLAG_SR_ORE   USART_ISR_ORE
  #define FLAG_SR_NE    USART_ISR_NE
  #define FLAG_SR_FE    USART_ISR_FE
  #define FLAG_SR_TXE   USART_ISR_TXE_TXFNF
  #define FLAG_SR_TC    USART_ISR_TC
#endif


//-------------------------------------------------------
// ISR routine
//-------------------------------------------------------
#if defined UARTC_USE_RX || defined UARTC_USE_TX_ISR

void UARTC_IRQHandler(void)
{
  // Cube HAL for f103rc is also calling this first independent on irqs
  uint16_t usart_sr = LL_USART_ReadReg(UARTC_UARTx, REG_SR); // read USART_SR register

#ifdef UARTC_USE_RX
  if (usart_sr & FLAG_SR_RXNE) {
#ifdef UARTC_USE_RXERRORCOUNT
    if (usart_sr & FLAG_SR_ORE) uartc_errorcnt_rxoverrun++;
    if (usart_sr & FLAG_SR_NE) uartc_errorcnt_rxnoise++;
    if (usart_sr & FLAG_SR_FE) uartc_errorcnt_rxframe++;
#endif
    // Cube LL driver (for f103?) says that reading SR and then writing to DR will clear error flags, so do it after error checks
    uint8_t usart_dr = LL_USART_ReceiveData8(UARTC_UARTx); // read USART_DR register, clears RXNE and RX error flags
#ifdef UARTC_RX_CALLBACK_FULL
    UARTC_RX_CALLBACK_FULL(usart_dr);
#else
    uint16_t next = (uartc_rxwritepos + 1) & UARTC_RXBUFSIZEMASK;
    if (uartc_rxreadpos != next) { // fifo not full
      uartc_rxbuf[next] = usart_dr;
      uartc_rxwritepos = next;
    }
#endif
  }
#if defined STM32F1
  // ORE is enabled along with RXNE, so we need to handle it
  // ORE bit is reset by read to USART_SR followed by read to USART_DR
  if (usart_sr & FLAG_SR_ORE) {
    LL_USART_ReceiveData8(UARTC_UARTx); // read USART_DR register, clears RXNE and RX error flags
  }
#elif defined STM32F3
  // ORE is enabled along with RXNE, but can be cleared by writing 1 to ICR
  LL_USART_WriteReg(UARTC_UARTx, ICR, (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NCF | LL_USART_ICR_FECF | LL_USART_ICR_PECF));
#elif defined STM32F7
  #warning UARTC F7 check clear flags, should be LL !?
  // these are cleared by write 1 to the ICR register, not by a read of DR register!
  LL_USART_WriteReg(UARTC_UARTx, ICR, (USART_ICR_IDLECF | USART_ICR_ORECF | USART_ICR_NCF | USART_ICR_FECF | USART_ICR_PECF));
#elif defined STM32G4 || defined STM32L4 || defined STM32WL
  LL_USART_WriteReg(UARTC_UARTx, ICR, (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NECF | LL_USART_ICR_FECF | LL_USART_ICR_PECF));
#endif
#endif

#ifdef UARTC_USE_TX_ISR
  if (usart_sr & FLAG_SR_TXE) {
    if (uartc_txwritepos != uartc_txreadpos) { // fifo not empty
      uartc_txreadpos = (uartc_txreadpos + 1) & UARTC_TXBUFSIZEMASK;
      LL_USART_TransmitData8(UARTC_UARTx, uartc_txbuf[uartc_txreadpos]); // write to USART_DR register, clears TXE flag, as well as TC
    } else {
      LL_USART_DisableIT_TXE(UARTC_UARTx); // disable interrupt when empty
#ifdef UARTC_TC_CALLBACK
      // ATTENTION: doesn't distinguish between empty fifo and last byte just send!
      LL_USART_ClearFlag_TC(UARTC_UARTx); // now enable TC interrupt
      LL_USART_EnableIT_TC(UARTC_UARTx);
#endif
    }
  }

#ifdef UARTC_TC_CALLBACK
  if (usart_sr & FLAG_SR_TC) {
    LL_USART_DisableIT_TC(UARTC_UARTx);
    LL_USART_ClearFlag_TC(UARTC_UARTx);
    UARTC_TC_CALLBACK();
  }
#endif
#endif
}

#endif // defined UARTC_USE_RX || defined UARTC_USE_TX_ISR


//-------------------------------------------------------
// TX routines
//-------------------------------------------------------
#ifdef UARTC_USE_TX

uint16_t uartc_putc(char c)
{
#ifndef UARTC_USE_TX_ISR
  while (!LL_USART_IsActiveFlag_TC(UARTC_UARTx)) {}
  LL_USART_TransmitData8(UARTC_UARTx, c);
  return 1;
#else
  uint16_t next = (uartc_txwritepos + 1) & UARTC_TXBUFSIZEMASK;
  if (uartc_txreadpos != next) { // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
    uartc_txbuf[next] = c;
    uartc_txwritepos = next;
    LL_USART_EnableIT_TXE(UARTC_UARTx); // enable interrupt // then TDR is empty, the TXE bit is set, and hence the isr is triggered
    return 1;
  }
  return 0;
#endif
}


void uartc_puts(const char *s)
{
  while (*s) {
    uartc_putc(*s);
    s++;
  }
}


void uartc_putbuf(uint8_t* buf, uint16_t len)
{
uint16_t i;

  for (i = 0; i < len; i++) uartc_putc(buf[i]);
}


#ifdef UARTC_USE_TX_ISR
uint16_t uartc_tx_notfull(void)
{
  uint16_t next = (uartc_txwritepos + 1) & UARTC_TXBUFSIZEMASK;
  return (uartc_txreadpos != next); // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
}


static inline uint16_t uartc_tx_isempty(void)
{
  if (uartc_txwritepos == uartc_txreadpos) return 1;
  return 0;
}


static inline void uartc_tx_flush(void)
{
  uartc_txwritepos = uartc_txreadpos = 0;
}
#else
static inline void uartc_tx_flush(void) {}
#endif

#endif // defined UARTC_USE_TX


//-------------------------------------------------------
// RX routines
//-------------------------------------------------------
#ifdef UARTC_USE_RX

static inline uint16_t uartc_rx_available(void)
{
  if (uartc_rxwritepos == uartc_rxreadpos) return 0; // fifo empty
  return 1;
}


uint16_t uartc_rx_bytesavailable(void)
{
int16_t d;

  d = (int16_t)uartc_rxwritepos - (int16_t)uartc_rxreadpos;
  return (d < 0) ? d + (UARTC_RXBUFSIZEMASK + 1) : d;
}


char uartc_getc(void)
{
  while (uartc_rxwritepos == uartc_rxreadpos) {};
  uartc_rxreadpos = (uartc_rxreadpos + 1) & UARTC_RXBUFSIZEMASK;
  return uartc_rxbuf[uartc_rxreadpos];
}


static inline char uartc_lookatnextc(void)
{
  return uartc_rxbuf[(uartc_rxreadpos + 1) & UARTC_RXBUFSIZEMASK];
}


static inline void uartc_rx_flush(void)
{
  uartc_rxwritepos = uartc_rxreadpos = 0;
}

#endif // defined UARTC_USE_RX


//-------------------------------------------------------
// INIT routines
//-------------------------------------------------------
// ATTENTION:
// LPUART does not support 0.5 or 1.5 stop bits !!

// helper, usually should not be called itself
void _uartc_initprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
#if !(defined UARTC_USE_LPUART1 || defined UARTC_USE_LPUART1_REMAPPED)
LL_USART_InitTypeDef UART_InitStruct = {0};

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

  LL_USART_Init(UARTC_UARTx, &UART_InitStruct);
#else
LL_LPUART_InitTypeDef UART_InitStruct = {0};

  UART_InitStruct.BaudRate = baud;
  UART_InitStruct.DataWidth = (parity != LL_USART_PARITY_NONE) ? LL_LPUART_DATAWIDTH_9B : LL_LPUART_DATAWIDTH_8B;
  UART_InitStruct.StopBits = stopbits;
  UART_InitStruct.Parity = parity;
  UART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  UART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  UART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;

  LL_LPUART_Init(UARTC_UARTx, &UART_InitStruct);
#endif
}


void uartc_setprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
#if !(defined UARTC_USE_LPUART1 || defined UARTC_USE_LPUART1_REMAPPED)
  LL_USART_Disable(UARTC_UARTx); // must be disabled to configure some registers
  _uartc_initprotocol(baud, parity, stopbits);
  LL_USART_Enable(UARTC_UARTx);
#else
  LL_LPUART_Disable(UARTC_UARTx); // must be disabled to configure some registers
  _uartc_initprotocol(baud, parity, stopbits);
  LL_LPUART_Enable(UARTC_UARTx);
#endif
}


void uartc_setbaudrate(uint32_t baud)
{
  uartc_setprotocol(baud, XUART_PARITY_NO, UART_STOPBIT_1);
}


void uartc_tx_enable(FunctionalState flag)
{
#ifdef UARTC_USE_TX
  if (flag == ENABLE) {
    gpio_change_af(UARTC_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UARTC_IO_AF, IO_SPEED_VERYFAST);
  } else {
    gpio_change(UARTC_TX_IO, IO_MODE_INPUT_ANALOG, IO_SPEED_VERYFAST);
  }
#endif
}


void uartc_rx_enableisr(FunctionalState flag)
{
#ifdef UARTC_USE_RX
  if (flag == ENABLE) {
    // Enable Receive Data register not empty interrupt
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UARTC_UARTx);
#endif
    LL_USART_EnableIT_RXNE(UARTC_UARTx);
  } else {
    LL_USART_DisableIT_RXNE(UARTC_UARTx);
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UARTC_UARTx);
#endif
  }
#endif
}


void uartc_init_isroff(void)
{
  LL_USART_Disable(UARTC_UARTx);

  // Initialize clocks for USART/LPUART, GPIO, AFIO
  rcc_init_afio();
  rcc_init_uart(UARTC_UARTx);
#ifdef STM32F1
#if defined UARTC_USE_UART1_REMAPPED
  LL_GPIO_AF_EnableRemap_USART1();
#endif
#if defined UARTC_USE_UART2_REMAPPED
  LL_GPIO_AF_EnableRemap_USART2();
#endif
#if defined UARTC_USE_UART3_REMAPPED
  LL_GPIO_AF_RemapPartial_USART3();
#endif
#endif // STM32F1

  // Configure pin USART/LPUART Tx as alternative function push-pull
#ifdef UARTC_USE_TX
  gpio_init_af(UARTC_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UARTC_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure pin USART/LPUART Rx as input with pull up
#ifdef UARTC_USE_RX
  gpio_init_af(UARTC_RX_IO, IO_MODE_INPUT_PU, UARTC_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure USART/LPUART
  _uartc_initprotocol(UARTC_BAUD, XUART_PARITY_NO, UART_STOPBIT_1);
#if !(defined UARTC_USE_LPUART1 || defined UARTC_USE_LPUART1_REMAPPED)
  LL_USART_ConfigAsyncMode(UARTC_UARTx);
#if defined STM32G4
  LL_USART_DisableFIFO(UARTC_UARTx);
  LL_USART_SetTXFIFOThreshold(UARTC_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(UARTC_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
#endif
#else
  LL_LPUART_DisableFIFO(UARTC_UARTx);
  LL_LPUART_SetTXFIFOThreshold(UARTC_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
  LL_LPUART_SetRXFIFOThreshold(UARTC_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
#endif

  // Configure NVIC channel
#if defined UARTC_USE_RX || defined UARTC_USE_TX_ISR
#ifdef UARTC_PREEMPTIONPRIORITY
  #error Legacy UARTC_PREEMPTIONPRIORITY used !
#endif
#ifdef UARTC_SUBPRIORITY
  #error Legacy UARTC_SUBPRIORITY used !
#endif
#ifdef UARTC_PRIORITY
#error Legacy UARTC_PRIORITY used !
#endif
#ifndef UARTC_IRQ_PRIORITY
  #define UARTC_IRQ_PRIORITY  15 // set priority to lowest
#endif
  nvic_irq_enable_w_priority(UARTC_IRQn, UARTC_IRQ_PRIORITY);
#endif

#ifdef UARTC_USE_TX_ISR
  // Disable Transmit Data Register empty interrupt
  LL_USART_DisableIT_TXE(UARTC_UARTx);
  LL_USART_ClearFlag_TC(UARTC_UARTx);
  // Flush buffer
  uartc_txwritepos = uartc_txreadpos = 0;
#endif

#ifdef UARTC_USE_RX
  // Disable Receive Data register not empty interrupt
#ifdef STM32F1
  LL_USART_ClearFlag_RXNE(UARTC_UARTx);
#endif
  LL_USART_EnableIT_RXNE(UARTC_UARTx);
  // Flush buffer
  uartc_rxwritepos = uartc_rxreadpos = 0;
#ifdef UARTC_USE_RXERRORCOUNT
  uartc_errorcnt_rxnoise = 0;
  uartc_errorcnt_rxframe = 0;
  uartc_errorcnt_rxoverrun = 0;
#endif
#endif

  // Cube HAL for f103rc
  /* In asynchronous mode, the following bits must be kept cleared:
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(UARTC_UARTx->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(UARTC_UARTx->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

#if defined STM32F7
  // reading the F722 manual I've added this
  CLEAR_BIT(UARTC_UARTx->CR1, (USART_CR1_EOBIE | USART_CR1_RTOIE | USART_CR1_CMIE | USART_CR1_PEIE | USART_CR1_IDLEIE));
  CLEAR_BIT(UARTC_UARTx->CR2, (USART_CR2_RTOEN | USART_CR2_CPOL | USART_CR2_CPHA | USART_CR2_LBCL | USART_CR2_LBDIE | USART_CR2_LBDL));
  CLEAR_BIT(UARTC_UARTx->CR3, (USART_CR3_TCBGTIE | USART_CR3_CTSIE | USART_CR3_EIE));
#if !defined UARTC_USE_RXERRORCOUNT
  // Disable ORE in the first place
  LL_USART_DisableOverrunDetect(UARTC_UARTx); // disables ORE, check how F1 is behaving
#endif
#endif

#if defined STM32G4 || defined STM32WL
#if !defined UARTC_USE_RXERRORCOUNT
  LL_USART_DisableOverrunDetect(UARTC_UARTx);
#endif
#endif

  // Enable USART/LPUART
  LL_USART_Enable(UARTC_UARTx);

#if defined STM32G4 || defined STM32WL
  // Polling UART/LPUART initialisation
  while((!(LL_USART_IsActiveFlag_TEACK(UARTC_UARTx))) || (!(LL_USART_IsActiveFlag_REACK(UARTC_UARTx)))) {};
#endif
}


void uartc_init(void)
{
  uartc_init_isroff();
  uartc_rx_enableisr(ENABLE);
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_UARTC_H
