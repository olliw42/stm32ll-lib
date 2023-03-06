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
// #define UART$_USE_UART1, UART$_USE_UART2, UART$_USE_UART3, UART$_USE_UART3_REMAPPED, UART$_USE_UART4, UART$_USE_UART5
// #define UART$_USE_LPUART1, UART$_USE_LPUART1_REMAPPED
//
// #define UART$_BAUD
//
// #define UART$_USE_TX
// #define UART$_TXBUFSIZE
// #define UART$_USE_TX_ISR
// #define UART$_USE_TX_DMA
//
// #define UART$_USE_RX
// #define UART$_RXBUFSIZE
//
// #define UART$_IRQ_PRIORITY
//
// #define UART$_USE_RXERRORCOUNT
//
// #define UART$_RX_CALLBACK_FULL(c)
// #define UART$_TC_CALLBACK() // ATTENTION: when used, one never should trigger TC isr without a byte written to the tx buffer
//
//*******************************************************
#ifndef STDSTM32_UART$_H
#define STDSTM32_UART$_H
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

#if (defined UART$_USE_TX_ISR) && (!defined UART$_USE_TX)
  #error UART$_USE_TX_ISR used without UART$_USE_TX!
#endif
#if (defined UART$_USE_RXERRORCOUNT) && (!defined UART$_USE_RX)
  #error UART$_USE_RXERRORCOUNT used without UART$_USE_RX!
#endif

#ifdef UART$_USE_TX_DMA
  #error UART$_USE_TX_DMA not supported by this library!
#endif


#ifndef UART$_BAUD
  #define UART$_BAUD             115200UL // 9600UL
#endif


#ifdef UART$_USE_TX_ISR
  #ifndef UART$_TXBUFSIZE
    #define UART$_TXBUFSIZE      256 // MUST be 2^N
  #endif
  #if UART$_TXBUFSIZE < 2
    #error UART$_TXBUFSIZE must be larger than 1 !
  #elif ((UART$_TXBUFSIZE & (UART$_TXBUFSIZE-1)) != 0)
    #error UART$_TXBUFSIZE must be a power of 2 !
  #endif

  #define UART$_TXBUFSIZEMASK  (UART$_TXBUFSIZE-1)

  volatile char uart$_txbuf[UART$_TXBUFSIZE];
  volatile uint16_t uart$_txwritepos; // pos at which the last byte was stored
  volatile uint16_t uart$_txreadpos; // pos at which the next byte is to be fetched
#endif


#ifdef UART$_USE_RX
  #ifndef UART$_RXBUFSIZE
    #define UART$_RXBUFSIZE      256 //128 //MUST be 2^N
  #endif
  #if UART$_RXBUFSIZE < 2
    #error UART$_RXBUFSIZE must be larger than 1 !
  #elif ((UART$_RXBUFSIZE & (UART$_RXBUFSIZE-1)) != 0)
    #error UART$_RXBUFSIZE must be a power of 2 !
  #endif

  #define UART$_RXBUFSIZEMASK  (UART$_RXBUFSIZE-1)

  volatile char uart$_rxbuf[UART$_RXBUFSIZE];
  volatile uint16_t uart$_rxwritepos; // pos at which the last byte was stored
  volatile uint16_t uart$_rxreadpos; // pos at which the next byte is to be fetched

  #ifdef UART$_USE_RXERRORCOUNT
  volatile uint32_t uart$_errorcnt_rxnoise;
  volatile uint32_t uart$_errorcnt_rxframe;
  volatile uint32_t uart$_errorcnt_rxoverrun;
  #endif
#endif


#if defined UART$_USE_UART1 || defined UART$_USE_UART1_REMAPPED || defined UART$_USE_UART1_REMAPPED2
  #define UART$_UARTx             USART1
  #ifdef UART$_USE_UART1
    #define UART$_TX_IO           IO_PA9
    #define UART$_RX_IO           IO_PA10
  #elif defined UART$_USE_UART1_REMAPPED
    #define UART$_TX_IO           IO_PB6
    #define UART$_RX_IO           IO_PB7
  #elif defined UART$_USE_UART1_REMAPPED2 // only G4
    #define UART$_TX_IO           IO_PC4 // on G4
    #define UART$_RX_IO           IO_PC5
  #endif
  #define UART$_IO_AF             IO_AF_7
  #define UART$_IRQn              USART1_IRQn
  #define UART$_IRQHandler        USART1_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UART$_USE_UART2 || defined UART$_USE_UART2_REMAPPED || defined UART$_USE_UART2_REMAPPED2 || defined UART$_USE_UART2_REMAPPED3
  #define UART$_UARTx             USART2
  #ifdef UART$_USE_UART2
    #define UART$_TX_IO           IO_PA2
    #define UART$_RX_IO           IO_PA3
  #elif defined UART$_USE_UART2_REMAPPED
    #define UART$_TX_IO           IO_PD5
    #define UART$_RX_IO           IO_PD6
  #elif defined UART$_USE_UART2_REMAPPED2 // only G4
    #define UART$_TX_IO           IO_PB3
    #define UART$_RX_IO           IO_PB4
  #elif defined UART$_USE_UART2_REMAPPED3 // only G4 // ATTENTION: PA14 overlaps with SWCLK
    #define UART$_TX_IO           IO_PA14
    #define UART$_RX_IO           IO_PA15
  #endif
  #define UART$_IO_AF             IO_AF_7
  #define UART$_IRQn              USART2_IRQn
  #define UART$_IRQHandler        USART2_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UART$_USE_UART3 || defined UART$_USE_UART3_REMAPPED
  #define UART$_UARTx             USART3
  #if defined UART$_USE_UART3 && defined STM32F3
    #define UART$_TX_IO           IO_PB8
    #define UART$_RX_IO           IO_PB9
  #elif defined UART$_USE_UART3
    #define UART$_TX_IO           IO_PB10
    #define UART$_RX_IO           IO_PB11
  #else
    #define UART$_TX_IO           IO_PC10
    #define UART$_RX_IO           IO_PC11
  #endif
  #define UART$_IO_AF             IO_AF_7
  #define UART$_IRQn              USART3_IRQn
  #define UART$_IRQHandler        USART3_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
    #error UART3 NOT AVAILABLE !
  #endif

#elif defined UART$_USE_UART4 || defined UART$_USE_UART4_REMAPPED
  #define UART$_UARTx             UART4
  #ifdef UART$_USE_UART4
    #define UART$_TX_IO           IO_PC10
    #define UART$_RX_IO           IO_PC11
  #else
    #define UART$_TX_IO           IO_PA0 // only F7 !
    #define UART$_RX_IO           IO_PA1
  #endif
  #define UART$_IRQn              UART4_IRQn
  #define UART$_IRQHandler        UART4_IRQHandler
  #if defined STM32F1
    #define UART$_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F3
    #warning UART$ IO_AF & DMA for F3 still TODO
  #elif defined STM32F7
    #define UART$_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UART$_IO_AF           IO_AF_5
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART4 NOT AVAILABLE !
  #endif

#elif defined UART$_USE_UART5
  #define UART$_UARTx             UART5
  #define UART$_TX_IO             IO_PC12
  #define UART$_RX_IO             IO_PD2
  #define UART$_IRQn              UART5_IRQn
  #define UART$_IRQHandler        UART5_IRQHandler
  #if defined STM32F1
    #define UART$_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F3
    #warning UART$ IO_AF & DMA for F3 still TODO //TODO
  #elif defined STM32F7
    #define UART$_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UART$_IO_AF           IO_AF_5
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART5 NOT AVAILABLE !
  #endif

#elif defined UART$_USE_UART6 || defined UART$_USE_UART6_REMAPPED
  #define UART$_UARTx             USART6
  #ifdef UART$_USE_UART6
    #define UART$_TX_IO           IO_PC6
    #define UART$_RX_IO           IO_PC7
  #else
    #define UART$_TX_IO           IO_PG14
    #define UART$_RX_IO           IO_PG9
  #endif
  #define UART$_IO_AF             IO_AF_8
  #define UART$_IRQn              USART6_IRQn
  #define UART$_IRQHandler        USART6_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART6 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UART$_USE_UART7 || defined UART$_USE_UART7_REMAPPED
  #define UART$_UARTx             UART7
  #ifdef UART$_USE_UART7
    #define UART$_TX_IO           IO_PE8
    #define UART$_RX_IO           IO_PE7
  #else
    #define UART$_TX_IO           IO_PF7
    #define UART$_RX_IO           IO_PF6
  #endif
  #define UART$_IO_AF             IO_AF_8
  #define UART$_IRQn              UART7_IRQn
  #define UART$_IRQHandler        UART7_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART7 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UART$_USE_UART8
  #define UART$_UARTx             UART8
  #define UART$_TX_IO             IO_PE1
  #define UART$_RX_IO             IO_PE0
  #define UART$_IO_AF             IO_AF_8
  #define UART$_IRQn              UART8_IRQn
  #define UART$_IRQHandler        UART8_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART8 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UART$_USE_LPUART1 || defined UART$_USE_LPUART1_REMAPPED
  #define UART$_UARTx             LPUART1
  #ifdef UART$_USE_LPUART1
    #define UART$_TX_IO           IO_PA2
    #define UART$_RX_IO           IO_PA3
    #ifdef STM32G4
      #define UART$_IO_AF         IO_AF_12 // G4
    #else
      #define UART$_IO_AF         IO_AF_8 // WL
    #endif
  #elif defined UART$_USE_LPUART1_REMAPPED
    #define UART$_TX_IO           IO_PC1
    #define UART$_RX_IO           IO_PC0
    #define UART$_IO_AF           IO_AF_8
  #endif
  #define UART$_IRQn              LPUART1_IRQn
  #define UART$_IRQHandler        LPUART1_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32F7
    #error LPUART1 NOT AVAILABLE !
  #elif defined STM32G4
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
  #endif

#else
  #error No UART$_USE_UARTx defined!
#endif


// allows us to overwrite the default pin assignment
#ifdef UART$_USE_TX_IO
  #undef UART$_TX_IO
  #define UART$_TX_IO             UART$_USE_TX_IO
#endif
#ifdef UART$_USE_RX_IO
  #undef UART$_RX_IO
  #define UART$_RX_IO             UART$_USE_RX_IO
#endif
#ifdef UART$_USE_IO_AF
  #undef UART$_IO_AF
  #define UART$_IO_AF             UART$_USE_IO_AF
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
#if defined UART$_USE_RX || defined UART$_USE_TX_ISR

void UART$_IRQHandler(void)
{
  // Cube HAL for f103rc is also calling this first independent on irqs
  uint16_t usart_sr = LL_USART_ReadReg(UART$_UARTx, REG_SR); // read USART_SR register

#ifdef UART$_USE_RX
  if (usart_sr & FLAG_SR_RXNE) {
#ifdef UART$_USE_RXERRORCOUNT
    if (usart_sr & FLAG_SR_ORE) uart$_errorcnt_rxoverrun++;
    if (usart_sr & FLAG_SR_NE) uart$_errorcnt_rxnoise++;
    if (usart_sr & FLAG_SR_FE) uart$_errorcnt_rxframe++;
#endif
    // Cube LL driver (for f103?) says that reading SR and then writing to DR will clear error flags, so do it after error checks
    uint8_t usart_dr = LL_USART_ReceiveData8(UART$_UARTx); // read USART_DR register, clears RXNE and RX error flags
#ifdef UART$_RX_CALLBACK_FULL
    UART$_RX_CALLBACK_FULL(usart_dr);
#else
    uint16_t next = (uart$_rxwritepos + 1) & UART$_RXBUFSIZEMASK;
    if (uart$_rxreadpos != next) { // fifo not full
      uart$_rxbuf[next] = usart_dr;
      uart$_rxwritepos = next;
    }
#endif
  }
#if defined STM32F1
  // ORE is enabled along with RXNE, so we need to handle it
  // ORE bit is reset by read to USART_SR followed by read to USART_DR
  if (usart_sr & FLAG_SR_ORE) {
    LL_USART_ReceiveData8(UART$_UARTx); // read USART_DR register, clears RXNE and RX error flags
  }
#elif defined STM32F3
  // ORE is enabled along with RXNE, but can be cleared by writing 1 to ICR
  LL_USART_WriteReg(UART$_UARTx, ICR, (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NCF | LL_USART_ICR_FECF | LL_USART_ICR_PECF));
#elif defined STM32F7
  #warning UART$ F7 check clear flags, should be LL !?
  // these are cleared by write 1 to the ICR register, not by a read of DR register!
  LL_USART_WriteReg(UART$_UARTx, ICR, (USART_ICR_IDLECF | USART_ICR_ORECF | USART_ICR_NCF | USART_ICR_FECF | USART_ICR_PECF));
#elif defined STM32G4 || defined STM32L4 || defined STM32WL
  LL_USART_WriteReg(UART$_UARTx, ICR, (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NECF | LL_USART_ICR_FECF | LL_USART_ICR_PECF));
#endif
#endif

#ifdef UART$_USE_TX_ISR
  if (usart_sr & FLAG_SR_TXE) {
    if (uart$_txwritepos != uart$_txreadpos) { // fifo not empty
      uart$_txreadpos = (uart$_txreadpos + 1) & UART$_TXBUFSIZEMASK;
      LL_USART_TransmitData8(UART$_UARTx, uart$_txbuf[uart$_txreadpos]); // write to USART_DR register, clears TXE flag, as well as TC
    } else {
      LL_USART_DisableIT_TXE(UART$_UARTx); // disable interrupt when empty
#ifdef UART$_TC_CALLBACK
      // ATTENTION: doesn't distinguish between empty fifo and last byte just send!
      LL_USART_ClearFlag_TC(UART$_UARTx); // now enable TC interrupt
      LL_USART_EnableIT_TC(UART$_UARTx);
#endif
    }
  }

#ifdef UART$_TC_CALLBACK
  if (usart_sr & FLAG_SR_TC) {
    LL_USART_DisableIT_TC(UART$_UARTx);
    LL_USART_ClearFlag_TC(UART$_UARTx);
    UART$_TC_CALLBACK();
  }
#endif
#endif
}

#endif // defined UART$_USE_RX || defined UART$_USE_TX_ISR


//-------------------------------------------------------
// TX routines
//-------------------------------------------------------
#ifdef UART$_USE_TX

uint16_t uart$_putc(char c)
{
#ifndef UART$_USE_TX_ISR
  while (!LL_USART_IsActiveFlag_TC(UART$_UARTx)) {}
  LL_USART_TransmitData8(UART$_UARTx, c);
  return 1;
#else
  uint16_t next = (uart$_txwritepos + 1) & UART$_TXBUFSIZEMASK;
  if (uart$_txreadpos != next) { // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
    uart$_txbuf[next] = c;
    uart$_txwritepos = next;
    LL_USART_EnableIT_TXE(UART$_UARTx); // enable interrupt // then TDR is empty, the TXE bit is set, and hence the isr is triggered
    return 1;
  }
  return 0;
#endif
}


void uart$_puts(const char *s)
{
  while (*s) {
    uart$_putc(*s);
    s++;
  }
}


void uart$_putbuf(uint8_t* buf, uint16_t len)
{
uint16_t i;

  for (i = 0; i < len; i++) uart$_putc(buf[i]);
}


#ifdef UART$_USE_TX_ISR
uint16_t uart$_tx_notfull(void)
{
  uint16_t next = (uart$_txwritepos + 1) & UART$_TXBUFSIZEMASK;
  return (uart$_txreadpos != next); // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
}


static inline uint16_t uart$_tx_isempty(void)
{
  if (uart$_txwritepos == uart$_txreadpos) return 1;
  return 0;
}


static inline void uart$_tx_flush(void)
{
  uart$_txwritepos = uart$_txreadpos = 0;
}
#else
static inline void uart$_tx_flush(void) {}
#endif

#endif // defined UART$_USE_TX


//-------------------------------------------------------
// RX routines
//-------------------------------------------------------
#ifdef UART$_USE_RX

static inline uint16_t uart$_rx_available(void)
{
  if (uart$_rxwritepos == uart$_rxreadpos) return 0; // fifo empty
  return 1;
}


uint16_t uart$_rx_bytesavailable(void)
{
int16_t d;

  d = (int16_t)uart$_rxwritepos - (int16_t)uart$_rxreadpos;
  return (d < 0) ? d + (UART$_RXBUFSIZEMASK + 1) : d;
}


char uart$_getc(void)
{
  while (uart$_rxwritepos == uart$_rxreadpos) {};
  uart$_rxreadpos = (uart$_rxreadpos + 1) & UART$_RXBUFSIZEMASK;
  return uart$_rxbuf[uart$_rxreadpos];
}


static inline char uart$_lookatnextc(void)
{
  return uart$_rxbuf[(uart$_rxreadpos + 1) & UART$_RXBUFSIZEMASK];
}


static inline void uart$_rx_flush(void)
{
  uart$_rxwritepos = uart$_rxreadpos = 0;
}

#endif // defined UART$_USE_RX


//-------------------------------------------------------
// INIT routines
//-------------------------------------------------------
// ATTENTION:
// LPUART does not support 0.5 or 1.5 stop bits !!

// helper, usually should not be called itself
void _uart$_initprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
#if !(defined UART$_USE_LPUART1 || defined UART$_USE_LPUART1_REMAPPED)
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

  LL_USART_Init(UART$_UARTx, &UART_InitStruct);
#else
LL_LPUART_InitTypeDef UART_InitStruct = {0};

  UART_InitStruct.BaudRate = baud;
  UART_InitStruct.DataWidth = (parity != LL_USART_PARITY_NONE) ? LL_LPUART_DATAWIDTH_9B : LL_LPUART_DATAWIDTH_8B;
  UART_InitStruct.StopBits = stopbits;
  UART_InitStruct.Parity = parity;
  UART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  UART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  UART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;

  LL_LPUART_Init(UART$_UARTx, &UART_InitStruct);
#endif
}


void uart$_setprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
#if !(defined UART$_USE_LPUART1 || defined UART$_USE_LPUART1_REMAPPED)
  LL_USART_Disable(UART$_UARTx); // must be disabled to configure some registers
  _uart$_initprotocol(baud, parity, stopbits);
  LL_USART_Enable(UART$_UARTx);
#else
  LL_LPUART_Disable(UART$_UARTx); // must be disabled to configure some registers
  _uart$_initprotocol(baud, parity, stopbits);
  LL_LPUART_Enable(UART$_UARTx);
#endif
}


void uart$_setbaudrate(uint32_t baud)
{
  uart$_setprotocol(baud, XUART_PARITY_NO, UART_STOPBIT_1);
}


void uart$_tx_enable(FunctionalState flag)
{
#ifdef UART$_USE_TX
  if (flag == ENABLE) {
    gpio_change_af(UART$_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UART$_IO_AF, IO_SPEED_VERYFAST);
  } else {
    gpio_change(UART$_TX_IO, IO_MODE_INPUT_ANALOG, IO_SPEED_VERYFAST);
  }
#endif
}


void uart$_rx_enableisr(FunctionalState flag)
{
#ifdef UART$_USE_RX
  if (flag == ENABLE) {
    // Enable Receive Data register not empty interrupt
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UART$_UARTx);
#endif
    LL_USART_EnableIT_RXNE(UART$_UARTx);
  } else {
    LL_USART_DisableIT_RXNE(UART$_UARTx);
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UART$_UARTx);
#endif
  }
#endif
}


void uart$_init_isroff(void)
{
  LL_USART_Disable(UART$_UARTx);

  // Initialize clocks for USART/LPUART, GPIO, AFIO
  rcc_init_afio();
  rcc_init_uart(UART$_UARTx);
#ifdef STM32F1
#if defined UART$_USE_UART1_REMAPPED
  LL_GPIO_AF_EnableRemap_USART1();
#endif
#if defined UART$_USE_UART2_REMAPPED
  LL_GPIO_AF_EnableRemap_USART2();
#endif
#if defined UART$_USE_UART3_REMAPPED
  LL_GPIO_AF_RemapPartial_USART3();
#endif
#endif // STM32F1

  // Configure pin USART/LPUART Tx as alternative function push-pull
#ifdef UART$_USE_TX
  gpio_init_af(UART$_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UART$_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure pin USART/LPUART Rx as input with pull up
#ifdef UART$_USE_RX
  gpio_init_af(UART$_RX_IO, IO_MODE_INPUT_PU, UART$_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure USART/LPUART
  _uart$_initprotocol(UART$_BAUD, XUART_PARITY_NO, UART_STOPBIT_1);
#if !(defined UART$_USE_LPUART1 || defined UART$_USE_LPUART1_REMAPPED)
  LL_USART_ConfigAsyncMode(UART$_UARTx);
#if defined STM32G4
  LL_USART_DisableFIFO(UART$_UARTx);
  LL_USART_SetTXFIFOThreshold(UART$_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(UART$_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
#endif
#else
  LL_LPUART_DisableFIFO(UART$_UARTx);
  LL_LPUART_SetTXFIFOThreshold(UART$_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
  LL_LPUART_SetRXFIFOThreshold(UART$_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
#endif

  // Configure NVIC channel
#if defined UART$_USE_RX || defined UART$_USE_TX_ISR
#ifdef UART$_PREEMPTIONPRIORITY
  #error Legacy UART$_PREEMPTIONPRIORITY used !
#endif
#ifdef UART$_SUBPRIORITY
  #error Legacy UART$_SUBPRIORITY used !
#endif
#ifdef UART$_PRIORITY
#error Legacy UART$_PRIORITY used !
#endif
#ifndef UART$_IRQ_PRIORITY
  #define UART$_IRQ_PRIORITY  15 // set priority to lowest
#endif
  nvic_irq_enable_w_priority(UART$_IRQn, UART$_IRQ_PRIORITY);
#endif

#ifdef UART$_USE_TX_ISR
  // Disable Transmit Data Register empty interrupt
  LL_USART_DisableIT_TXE(UART$_UARTx);
  LL_USART_ClearFlag_TC(UART$_UARTx);
  // Flush buffer
  uart$_txwritepos = uart$_txreadpos = 0;
#endif

#ifdef UART$_USE_RX
  // Disable Receive Data register not empty interrupt
#ifdef STM32F1
  LL_USART_ClearFlag_RXNE(UART$_UARTx);
#endif
  LL_USART_EnableIT_RXNE(UART$_UARTx);
  // Flush buffer
  uart$_rxwritepos = uart$_rxreadpos = 0;
#ifdef UART$_USE_RXERRORCOUNT
  uart$_errorcnt_rxnoise = 0;
  uart$_errorcnt_rxframe = 0;
  uart$_errorcnt_rxoverrun = 0;
#endif
#endif

  // Cube HAL for f103rc
  /* In asynchronous mode, the following bits must be kept cleared:
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(UART$_UARTx->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(UART$_UARTx->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

#if defined STM32F7
  // reading the F722 manual I've added this
  CLEAR_BIT(UART$_UARTx->CR1, (USART_CR1_EOBIE | USART_CR1_RTOIE | USART_CR1_CMIE | USART_CR1_PEIE | USART_CR1_IDLEIE));
  CLEAR_BIT(UART$_UARTx->CR2, (USART_CR2_RTOEN | USART_CR2_CPOL | USART_CR2_CPHA | USART_CR2_LBCL | USART_CR2_LBDIE | USART_CR2_LBDL));
  CLEAR_BIT(UART$_UARTx->CR3, (USART_CR3_TCBGTIE | USART_CR3_CTSIE | USART_CR3_EIE));
#if !defined UART$_USE_RXERRORCOUNT
  // Disable ORE in the first place
  LL_USART_DisableOverrunDetect(UART$_UARTx); // disables ORE, check how F1 is behaving
#endif
#endif

#if defined STM32G4 || defined STM32WL
#if !defined UART$_USE_RXERRORCOUNT
  LL_USART_DisableOverrunDetect(UART$_UARTx);
#endif
#endif

  // Enable USART/LPUART
  LL_USART_Enable(UART$_UARTx);

#if defined STM32G4 || defined STM32WL
  // Polling UART/LPUART initialisation
  while((!(LL_USART_IsActiveFlag_TEACK(UART$_UARTx))) || (!(LL_USART_IsActiveFlag_REACK(UART$_UARTx)))) {};
#endif
}


void uart$_init(void)
{
  uart$_init_isroff();
  uart$_rx_enableisr(ENABLE);
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_UART$_H
