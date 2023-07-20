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
// #define UART_USE_UART1, UART_USE_UART2, UART_USE_UART3, UART_USE_UART3_REMAPPED, UART_USE_UART4, UART_USE_UART5
// #define UART_USE_LPUART1, UART_USE_LPUART1_REMAPPED
//
// #define UART_BAUD
//
// #define UART_USE_TX
// #define UART_TXBUFSIZE
// #define UART_USE_TX_ISR
// #define UART_USE_TX_DMA
//
// #define UART_USE_RX
// #define UART_RXBUFSIZE
//
// #define UART_IRQ_PRIORITY
//
// #define UART_USE_RXERRORCOUNT
//
// #define UART_RX_CALLBACK_FULL(c)
// #define UART_TC_CALLBACK() // ATTENTION: when used, one never should trigger TC isr without a byte written to the tx buffer
//
//*******************************************************
#ifndef STDSTM32_UART_H
#define STDSTM32_UART_H
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

#if (defined UART_USE_TX_ISR) && (!defined UART_USE_TX)
  #error UART_USE_TX_ISR used without UART_USE_TX!
#endif
#if (defined UART_USE_RXERRORCOUNT) && (!defined UART_USE_RX)
  #error UART_USE_RXERRORCOUNT used without UART_USE_RX!
#endif

#ifdef UART_USE_TX_DMA
  #error UART_USE_TX_DMA not supported by this library!
#endif


#ifndef UART_BAUD
  #define UART_BAUD             115200UL // 9600UL
#endif


#ifdef UART_USE_TX_ISR
  #ifndef UART_TXBUFSIZE
    #define UART_TXBUFSIZE      256 // MUST be 2^N
  #endif
  #if UART_TXBUFSIZE < 2
    #error UART_TXBUFSIZE must be larger than 1 !
  #elif ((UART_TXBUFSIZE & (UART_TXBUFSIZE-1)) != 0)
    #error UART_TXBUFSIZE must be a power of 2 !
  #endif

  #define UART_TXBUFSIZEMASK  (UART_TXBUFSIZE-1)

  volatile char uart_txbuf[UART_TXBUFSIZE];
  volatile uint16_t uart_txwritepos; // pos at which the last byte was stored
  volatile uint16_t uart_txreadpos; // pos at which the next byte is to be fetched
#endif


#ifdef UART_USE_RX
  #ifndef UART_RXBUFSIZE
    #define UART_RXBUFSIZE      256 //128 //MUST be 2^N
  #endif
  #if UART_RXBUFSIZE < 2
    #error UART_RXBUFSIZE must be larger than 1 !
  #elif ((UART_RXBUFSIZE & (UART_RXBUFSIZE-1)) != 0)
    #error UART_RXBUFSIZE must be a power of 2 !
  #endif

  #define UART_RXBUFSIZEMASK  (UART_RXBUFSIZE-1)

  volatile char uart_rxbuf[UART_RXBUFSIZE];
  volatile uint16_t uart_rxwritepos; // pos at which the last byte was stored
  volatile uint16_t uart_rxreadpos; // pos at which the next byte is to be fetched

  #ifdef UART_USE_RXERRORCOUNT
  volatile uint32_t uart_errorcnt_rxnoise;
  volatile uint32_t uart_errorcnt_rxframe;
  volatile uint32_t uart_errorcnt_rxoverrun;
  #endif
#endif


#if defined UART_USE_UART1 || defined UART_USE_UART1_REMAPPED || defined UART_USE_UART1_REMAPPED2
  #define UART_UARTx             USART1
  #ifdef UART_USE_UART1
    #define UART_TX_IO           IO_PA9
    #define UART_RX_IO           IO_PA10
  #elif defined UART_USE_UART1_REMAPPED
    #define UART_TX_IO           IO_PB6
    #define UART_RX_IO           IO_PB7
  #elif defined UART_USE_UART1_REMAPPED2 // only G4
    #define UART_TX_IO           IO_PC4 // on G4
    #define UART_RX_IO           IO_PC5
  #endif
  #ifndef STM32F0
    #define UART_IO_AF           IO_AF_7
  #else
    #if defined UART_USE_UART1
      #define UART_IO_AF         IO_AF_1
    #elif defined UART_USE_UART1_REMAPPED
      #define UART_IO_AF         IO_AF_0
	#else  
      #error UART_USE_UART1 mapping not available on STM32F0 !
	#endif  
  #endif
  #define UART_IRQn              USART1_IRQn
  #define UART_IRQHandler        USART1_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UART_USE_UART2 || defined UART_USE_UART2_REMAPPED || defined UART_USE_UART2_REMAPPED2 || defined UART_USE_UART2_REMAPPED3
  #define UART_UARTx             USART2
  #ifdef UART_USE_UART2
    #define UART_TX_IO           IO_PA2
    #define UART_RX_IO           IO_PA3
  #elif defined UART_USE_UART2_REMAPPED
    #define UART_TX_IO           IO_PD5
    #define UART_RX_IO           IO_PD6
  #elif defined UART_USE_UART2_REMAPPED2 // only G4
    #define UART_TX_IO           IO_PB3
    #define UART_RX_IO           IO_PB4
  #elif defined UART_USE_UART2_REMAPPED3 // only G4, F0 // ATTENTION: on G4 PA14 overlaps with SWCLK
    #define UART_TX_IO           IO_PA14
    #define UART_RX_IO           IO_PA15
  #endif
  #ifndef STM32F0
    #define UART_IO_AF           IO_AF_7
  #else
    #if defined UART_USE_UART2 || defined UART_USE_UART2_REMAPPED3
      #define UART_IO_AF         IO_AF_1
	#else  
      #error UART_USE_UART2 mapping not available on STM32F0 !
	#endif  
  #endif
  #define UART_IRQn              USART2_IRQn
  #define UART_IRQHandler        USART2_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UART_USE_UART3 || defined UART_USE_UART3_REMAPPED
  #define UART_UARTx             USART3
  #if defined UART_USE_UART3 && defined STM32F3
    #define UART_TX_IO           IO_PB8
    #define UART_RX_IO           IO_PB9
  #elif defined UART_USE_UART3
    #define UART_TX_IO           IO_PB10
    #define UART_RX_IO           IO_PB11
  #else
    #define UART_TX_IO           IO_PC10
    #define UART_RX_IO           IO_PC11
  #endif
  #ifndef STM32F0
    #define UART_IO_AF           IO_AF_7
  #else
    #if defined UART_USE_UART3
      #define UART_IO_AF         IO_AF_4
	#elif defined UART_USE_UART3_REMAPPED
      #define UART_IO_AF         IO_AF_1
	#else  
      #error UART_USE_UART3 mapping not available on STM32F0 !
	#endif  
  #endif
  #define UART_IRQn              USART3_IRQn
  #define UART_IRQHandler        USART3_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
    #error UART3 NOT AVAILABLE !
  #elif defined STM32F0
    #undef UART_IRQn
    #undef UART_IRQHandler
    #define UART_IRQn            USART3_4_IRQn
    #define UART_IRQHandler      USART3_4_IRQHandler
  #endif

#elif defined UART_USE_UART4 || defined UART_USE_UART4_REMAPPED
  #define UART_UARTx             UART4
  #ifdef UART_USE_UART4
    #define UART_TX_IO           IO_PC10
    #define UART_RX_IO           IO_PC11
  #else
    #define UART_TX_IO           IO_PA0 // only F7 !
    #define UART_RX_IO           IO_PA1
  #endif
  #if defined STM32F1
    #define UART_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F7
    #define UART_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UART_IO_AF           IO_AF_5
  #endif
  #define UART_IRQn              UART4_IRQn
  #define UART_IRQHandler        UART4_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
    #warning UART IO_AF & DMA for F3 still TODO
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART4 NOT AVAILABLE !
  #elif defined STM32F0
    #undef UART_IRQn
    #undef UART_IRQHandler
    #define UART_IRQn            USART3_4_IRQn
    #define UART_IRQHandler      USART3_4_IRQHandler
  #endif

#elif defined UART_USE_UART5
  #define UART_UARTx             UART5
  #define UART_TX_IO             IO_PC12
  #define UART_RX_IO             IO_PD2
  #define UART_IRQn              UART5_IRQn
  #define UART_IRQHandler        UART5_IRQHandler
  #if defined STM32F1
    #define UART_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F3
    #warning UART IO_AF & DMA for F3 still TODO //TODO
  #elif defined STM32F7
    #define UART_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UART_IO_AF           IO_AF_5
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART5 NOT AVAILABLE !
  #endif

#elif defined UART_USE_UART6 || defined UART_USE_UART6_REMAPPED
  #define UART_UARTx             USART6
  #ifdef UART_USE_UART6
    #define UART_TX_IO           IO_PC6
    #define UART_RX_IO           IO_PC7
  #else
    #define UART_TX_IO           IO_PG14
    #define UART_RX_IO           IO_PG9
  #endif
  #define UART_IO_AF             IO_AF_8
  #define UART_IRQn              USART6_IRQn
  #define UART_IRQHandler        USART6_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART6 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UART_USE_UART7 || defined UART_USE_UART7_REMAPPED
  #define UART_UARTx             UART7
  #ifdef UART_USE_UART7
    #define UART_TX_IO           IO_PE8
    #define UART_RX_IO           IO_PE7
  #else
    #define UART_TX_IO           IO_PF7
    #define UART_RX_IO           IO_PF6
  #endif
  #define UART_IO_AF             IO_AF_8
  #define UART_IRQn              UART7_IRQn
  #define UART_IRQHandler        UART7_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART7 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UART_USE_UART8
  #define UART_UARTx             UART8
  #define UART_TX_IO             IO_PE1
  #define UART_RX_IO             IO_PE0
  #define UART_IO_AF             IO_AF_8
  #define UART_IRQn              UART8_IRQn
  #define UART_IRQHandler        UART8_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART8 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UART_USE_LPUART1 || defined UART_USE_LPUART1_REMAPPED
  #define UART_UARTx             LPUART1
  #ifdef UART_USE_LPUART1
    #define UART_TX_IO           IO_PA2
    #define UART_RX_IO           IO_PA3
    #ifdef STM32G4
      #define UART_IO_AF         IO_AF_12 // G4
    #else
      #define UART_IO_AF         IO_AF_8 // WL
    #endif
  #elif defined UART_USE_LPUART1_REMAPPED
    #define UART_TX_IO           IO_PC1
    #define UART_RX_IO           IO_PC0
    #define UART_IO_AF           IO_AF_8
  #endif
  #define UART_IRQn              LPUART1_IRQn
  #define UART_IRQHandler        LPUART1_IRQHandler

  #if defined STM32F1 || defined STM32F3 || defined STM32F7
    #error LPUART1 NOT AVAILABLE !
  #elif defined STM32G4
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
  #endif

#else
  #error No UART_USE_UARTx defined!
#endif


// allows us to overwrite the default pin assignment
#ifdef UART_USE_TX_IO
  #undef UART_TX_IO
  #define UART_TX_IO             UART_USE_TX_IO
#endif
#ifdef UART_USE_RX_IO
  #undef UART_RX_IO
  #define UART_RX_IO             UART_USE_RX_IO
#endif
#ifdef UART_USE_IO_AF
  #undef UART_IO_AF
  #define UART_IO_AF             UART_USE_IO_AF
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
#elif defined STM32F3 || defined STM32F7 || defined STM32L4 || defined STM32F0
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
#if defined UART_USE_RX || defined UART_USE_TX_ISR

void UART_IRQHandler(void)
{
  // Cube HAL for f103rc is also calling this first independent on irqs
  uint16_t usart_sr = LL_USART_ReadReg(UART_UARTx, REG_SR); // read USART_SR register

#ifdef UART_USE_RX
  if ((usart_sr & FLAG_SR_RXNE) && LL_USART_IsEnabledIT_RXNE(UART_UARTx)) {
#ifdef UART_USE_RXERRORCOUNT
    if (usart_sr & FLAG_SR_ORE) uart_errorcnt_rxoverrun++;
    if (usart_sr & FLAG_SR_NE) uart_errorcnt_rxnoise++;
    if (usart_sr & FLAG_SR_FE) uart_errorcnt_rxframe++;
#endif
    // Cube LL driver (for f103?) says that reading SR and then writing to DR will clear error flags, so do it after error checks
    uint8_t usart_dr = LL_USART_ReceiveData8(UART_UARTx); // read USART_DR register, clears RXNE and RX error flags
#ifdef UART_RX_CALLBACK_FULL
    UART_RX_CALLBACK_FULL(usart_dr);
#else
    uint16_t next = (uart_rxwritepos + 1) & UART_RXBUFSIZEMASK;
    if (uart_rxreadpos != next) { // fifo not full
      uart_rxbuf[next] = usart_dr;
      uart_rxwritepos = next;
    }
#endif
  }
#if defined STM32F1
  // ORE is enabled along with RXNE, so we need to handle it
  // ORE bit is reset by read to USART_SR followed by read to USART_DR
  if (usart_sr & FLAG_SR_ORE) {
    LL_USART_ReceiveData8(UART_UARTx); // read USART_DR register, clears RXNE and RX error flags
  }
#elif defined STM32F3
  // ORE is enabled along with RXNE, but can be cleared by writing 1 to ICR
  LL_USART_WriteReg(UART_UARTx, ICR, (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NCF | LL_USART_ICR_FECF | LL_USART_ICR_PECF));
#elif defined STM32F7
  #warning UART F7 check clear flags, should be LL !?
  // these are cleared by write 1 to the ICR register, not by a read of DR register!
  LL_USART_WriteReg(UART_UARTx, ICR, (USART_ICR_IDLECF | USART_ICR_ORECF | USART_ICR_NCF | USART_ICR_FECF | USART_ICR_PECF));
#elif defined STM32G4 || defined STM32L4 || defined STM32WL
  LL_USART_WriteReg(UART_UARTx, ICR, (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NECF | LL_USART_ICR_FECF | LL_USART_ICR_PECF));
#endif
#endif

#ifdef UART_USE_TX_ISR
  if ((usart_sr & FLAG_SR_TXE) && LL_USART_IsEnabledIT_TXE(UART_UARTx)) {
    if (uart_txwritepos != uart_txreadpos) { // fifo not empty
      uart_txreadpos = (uart_txreadpos + 1) & UART_TXBUFSIZEMASK;
      LL_USART_TransmitData8(UART_UARTx, uart_txbuf[uart_txreadpos]); // write to USART_DR register, clears TXE flag, as well as TC
    } else {
      LL_USART_DisableIT_TXE(UART_UARTx); // disable interrupt when empty
#ifdef UART_TC_CALLBACK
      // ATTENTION: doesn't distinguish between empty fifo and last byte just send!
      LL_USART_ClearFlag_TC(UART_UARTx); // now enable TC interrupt
      LL_USART_EnableIT_TC(UART_UARTx);
#endif
    }
  }

#ifdef UART_TC_CALLBACK
  if ((usart_sr & FLAG_SR_TC) && LL_USART_IsEnabledIT_TC(UART_UARTx)) {
    LL_USART_DisableIT_TC(UART_UARTx);
    LL_USART_ClearFlag_TC(UART_UARTx);
    UART_TC_CALLBACK();
  }
#endif
#endif
}

#endif // defined UART_USE_RX || defined UART_USE_TX_ISR


//-------------------------------------------------------
// TX routines
//-------------------------------------------------------
#ifdef UART_USE_TX

uint16_t uart_putc(char c)
{
#ifndef UART_USE_TX_ISR
  while (!LL_USART_IsActiveFlag_TC(UART_UARTx)) {}
  LL_USART_TransmitData8(UART_UARTx, c);
  return 1;
#else
  uint16_t next = (uart_txwritepos + 1) & UART_TXBUFSIZEMASK;
  if (uart_txreadpos != next) { // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
    uart_txbuf[next] = c;
    uart_txwritepos = next;
    LL_USART_EnableIT_TXE(UART_UARTx); // enable interrupt // then TDR is empty, the TXE bit is set, and hence the isr is triggered
    return 1;
  }
  return 0;
#endif
}


void uart_puts(const char *s)
{
  while (*s) {
    uart_putc(*s);
    s++;
  }
}


void uart_putbuf(uint8_t* buf, uint16_t len)
{
uint16_t i;

  for (i = 0; i < len; i++) uart_putc(buf[i]);
}


#ifdef UART_USE_TX_ISR
uint16_t uart_tx_notfull(void)
{
  uint16_t next = (uart_txwritepos + 1) & UART_TXBUFSIZEMASK;
  return (uart_txreadpos != next); // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
}


static inline uint16_t uart_tx_isempty(void)
{
  if (uart_txwritepos == uart_txreadpos) return 1;
  return 0;
}


static inline void uart_tx_flush(void)
{
  uart_txwritepos = uart_txreadpos = 0;
}
#else
static inline void uart_tx_flush(void) {}
#endif

#endif // defined UART_USE_TX


//-------------------------------------------------------
// RX routines
//-------------------------------------------------------
#ifdef UART_USE_RX

static inline uint16_t uart_rx_available(void)
{
  if (uart_rxwritepos == uart_rxreadpos) return 0; // fifo empty
  return 1;
}


uint16_t uart_rx_bytesavailable(void)
{
int16_t d;

  d = (int16_t)uart_rxwritepos - (int16_t)uart_rxreadpos;
  return (d < 0) ? d + (UART_RXBUFSIZEMASK + 1) : d;
}


char uart_getc(void)
{
  while (uart_rxwritepos == uart_rxreadpos) {};
  uart_rxreadpos = (uart_rxreadpos + 1) & UART_RXBUFSIZEMASK;
  return uart_rxbuf[uart_rxreadpos];
}


static inline char uart_lookatnextc(void)
{
  return uart_rxbuf[(uart_rxreadpos + 1) & UART_RXBUFSIZEMASK];
}


static inline void uart_rx_flush(void)
{
  uart_rxwritepos = uart_rxreadpos = 0;
}

#endif // defined UART_USE_RX


//-------------------------------------------------------
// INIT routines
//-------------------------------------------------------
// ATTENTION:
// LPUART does not support 0.5 or 1.5 stop bits !!

// helper, usually should not be called itself
void _uart_initprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
#if !(defined UART_USE_LPUART1 || defined UART_USE_LPUART1_REMAPPED)
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

  LL_USART_Init(UART_UARTx, &UART_InitStruct);
#else
LL_LPUART_InitTypeDef UART_InitStruct = {};

  UART_InitStruct.BaudRate = baud;
  UART_InitStruct.DataWidth = (parity != LL_USART_PARITY_NONE) ? LL_LPUART_DATAWIDTH_9B : LL_LPUART_DATAWIDTH_8B;
  UART_InitStruct.StopBits = stopbits;
  UART_InitStruct.Parity = parity;
  UART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  UART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  UART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;

  LL_LPUART_Init(UART_UARTx, &UART_InitStruct);
#endif
}


void uart_setprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
#if !(defined UART_USE_LPUART1 || defined UART_USE_LPUART1_REMAPPED)
  LL_USART_Disable(UART_UARTx); // must be disabled to configure some registers
  _uart_initprotocol(baud, parity, stopbits);
  LL_USART_Enable(UART_UARTx);
#else
  LL_LPUART_Disable(UART_UARTx); // must be disabled to configure some registers
  _uart_initprotocol(baud, parity, stopbits);
  LL_LPUART_Enable(UART_UARTx);
#endif
}


void uart_setbaudrate(uint32_t baud)
{
  uart_setprotocol(baud, XUART_PARITY_NO, UART_STOPBIT_1);
}


void uart_tx_enablepin(FunctionalState flag)
{
#ifdef UART_USE_TX
  if (flag == ENABLE) {
    gpio_change_af(UART_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UART_IO_AF, IO_SPEED_VERYFAST);
  } else {
    gpio_change(UART_TX_IO, IO_MODE_INPUT_ANALOG, IO_SPEED_VERYFAST);
  }
#endif
}


void uart_rx_enableisr(FunctionalState flag)
{
#ifdef UART_USE_RX
  if (flag == ENABLE) {
    // Enable Receive Data register not empty interrupt
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UART_UARTx);
#endif
    LL_USART_ReceiveData8(UART_UARTx); // read DR to clear RXNE and error flags
    LL_USART_EnableIT_RXNE(UART_UARTx);
  } else {
    LL_USART_DisableIT_RXNE(UART_UARTx);
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UART_UARTx);
#endif
  }
#endif
}


void uart_init_isroff(void)
{
  LL_USART_Disable(UART_UARTx);

  // Initialize clocks for USART/LPUART, GPIO, AFIO
  rcc_init_afio();
  rcc_init_uart(UART_UARTx);
#ifdef STM32F1
#if defined UART_USE_UART1_REMAPPED
  LL_GPIO_AF_EnableRemap_USART1();
#endif
#if defined UART_USE_UART2_REMAPPED
  LL_GPIO_AF_EnableRemap_USART2();
#endif
#if defined UART_USE_UART3_REMAPPED
  LL_GPIO_AF_RemapPartial_USART3();
#endif
#endif // STM32F1

  // Configure pin USART/LPUART Tx as alternative function push-pull
#ifdef UART_USE_TX
  gpio_init_af(UART_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UART_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure pin USART/LPUART Rx as input with pull up
#ifdef UART_USE_RX
  gpio_init_af(UART_RX_IO, IO_MODE_INPUT_PU, UART_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure USART/LPUART
  _uart_initprotocol(UART_BAUD, XUART_PARITY_NO, UART_STOPBIT_1);
#if !(defined UART_USE_LPUART1 || defined UART_USE_LPUART1_REMAPPED)
  LL_USART_ConfigAsyncMode(UART_UARTx);
#if defined STM32G4
  LL_USART_DisableFIFO(UART_UARTx);
  LL_USART_SetTXFIFOThreshold(UART_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(UART_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
#endif
#else
  LL_LPUART_DisableFIFO(UART_UARTx);
  LL_LPUART_SetTXFIFOThreshold(UART_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
  LL_LPUART_SetRXFIFOThreshold(UART_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
#endif

  // Configure NVIC channel
#if defined UART_USE_RX || defined UART_USE_TX_ISR
#ifdef UART_PREEMPTIONPRIORITY
  #error Legacy UART_PREEMPTIONPRIORITY used !
#endif
#ifdef UART_SUBPRIORITY
  #error Legacy UART_SUBPRIORITY used !
#endif
#ifdef UART_PRIORITY
#error Legacy UART_PRIORITY used !
#endif
#ifndef UART_IRQ_PRIORITY
  #define UART_IRQ_PRIORITY  15 // set priority to lowest
#endif
  nvic_irq_enable_w_priority(UART_IRQn, UART_IRQ_PRIORITY);
#endif

#ifdef UART_USE_TX_ISR
  // Disable Transmit Data Register empty interrupt
  LL_USART_DisableIT_TXE(UART_UARTx);
  LL_USART_DisableIT_TC(UART_UARTx);
  LL_USART_ClearFlag_TC(UART_UARTx);
  // Flush buffer
  uart_txwritepos = uart_txreadpos = 0;
#endif

#ifdef UART_USE_RX
  // Disable Receive Data register not empty interrupt
  LL_USART_DisableIT_RXNE(UART_UARTx);
#ifdef STM32F1
  LL_USART_ClearFlag_RXNE(UART_UARTx);
#endif
  // Flush buffer
  uart_rxwritepos = uart_rxreadpos = 0;
#ifdef UART_USE_RXERRORCOUNT
  uart_errorcnt_rxnoise = 0;
  uart_errorcnt_rxframe = 0;
  uart_errorcnt_rxoverrun = 0;
#endif
#endif

  // Cube HAL for f103rc
  /* In asynchronous mode, the following bits must be kept cleared:
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(UART_UARTx->CR2, USART_CR2_CLKEN);
  CLEAR_BIT(UART_UARTx->CR3, USART_CR3_HDSEL);
#if defined USART_CR2_LINEN
  CLEAR_BIT(UART_UARTx->CR2, USART_CR2_LINEN);
#endif
#if defined USART_CR3_SCEN || defined USART_CR3_IREN // assume that always both are defined
  CLEAR_BIT(UART_UARTx->CR3, (USART_CR3_SCEN | USART_CR3_IREN));
#endif

#if defined STM32F7
  // reading the F722 manual I've added this
  CLEAR_BIT(UART_UARTx->CR1, (USART_CR1_EOBIE | USART_CR1_RTOIE | USART_CR1_CMIE | USART_CR1_PEIE | USART_CR1_IDLEIE));
  CLEAR_BIT(UART_UARTx->CR2, (USART_CR2_RTOEN | USART_CR2_CPOL | USART_CR2_CPHA | USART_CR2_LBCL | USART_CR2_LBDIE | USART_CR2_LBDL));
  CLEAR_BIT(UART_UARTx->CR3, (USART_CR3_TCBGTIE | USART_CR3_CTSIE | USART_CR3_EIE));
#if !defined UART_USE_RXERRORCOUNT
  // Disable ORE in the first place
  LL_USART_DisableOverrunDetect(UART_UARTx); // disables ORE, check how F1 is behaving
#endif
#endif

#if defined STM32G4 || defined STM32WL || defined STM32F0
#if !defined UART_USE_RXERRORCOUNT
  LL_USART_DisableOverrunDetect(UART_UARTx);
#endif
#endif

  // Enable USART/LPUART
  LL_USART_Enable(UART_UARTx);

#if defined STM32G4 || defined STM32WL || defined STM32F0
  // Polling UART/LPUART initialisation
  while((!(LL_USART_IsActiveFlag_TEACK(UART_UARTx))) || (!(LL_USART_IsActiveFlag_REACK(UART_UARTx)))) {};
#endif
}


void uart_init(void)
{
  uart_init_isroff();
  uart_rx_enableisr(ENABLE);
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_UART_H
