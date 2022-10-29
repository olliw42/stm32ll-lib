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
// #define UARTD_USE_UART1, UARTD_USE_UART2, UARTD_USE_UART3, UARTD_USE_UART3_REMAPPED, UARTD_USE_UART4, UARTD_USE_UART5
// #define UARTD_USE_LPUART1, UARTD_USE_LPUART1_REMAPPED
// 
// #define UARTD_BAUD
//
// #define UARTD_USE_TX
// #define UARTD_TXBUFSIZE
// #define UARTD_USE_TX_ISR
// #define UARTD_USE_TX_DMA
//
// #define UARTD_USE_RX
// #define UARTD_RXBUFSIZE
//
// #define UARTD_IRQ_PRIORITY
//
// #define UARTD_USE_RXERRORCOUNT
//
// #define UARTD_RX_CALLBACK_FULL(c)
// #define UARTD_TC_CALLBACK() // ATTENTION: when used, one never should trigger TC isr without a byte written to the tx buffer
//
//*******************************************************
#ifndef STDSTM32_UARTD_H
#define STDSTM32_UARTD_H
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

#include "stdstm32-peripherals.h"

#if (defined UARTD_USE_TX_ISR) && (!defined UARTD_USE_TX)
  #error UARTD_USE_TX_ISR used without UARTD_USE_TX!
#endif
#if (defined UARTD_USE_RXERRORCOUNT) && (!defined UARTD_USE_RX)
  #error UARTD_USE_RXERRORCOUNT used without UARTD_USE_RX!
#endif

#ifdef UARTD_USE_TX_DMA
  #error UARTD_USE_TX_DMA not supported by this library!
#endif


#ifndef UARTD_BAUD
  #define UARTD_BAUD             115200UL // 9600UL
#endif


#ifdef UARTD_USE_TX_ISR
  #ifndef UARTD_TXBUFSIZE
    #define UARTD_TXBUFSIZE      256 // MUST be 2^N
  #endif
  #if UARTD_TXBUFSIZE < 2
    #error UARTD_TXBUFSIZE must be larger than 1 !
  #elif ((UARTD_TXBUFSIZE & (UARTD_TXBUFSIZE-1)) != 0)
    #error UARTD_TXBUFSIZE must be a power of 2 !
  #endif

  #define UARTD_TXBUFSIZEMASK  (UARTD_TXBUFSIZE-1)

  volatile char uartd_txbuf[UARTD_TXBUFSIZE];
  volatile uint16_t uartd_txwritepos; // pos at which the last byte was stored
  volatile uint16_t uartd_txreadpos; // pos at which the next byte is to be fetched
#endif


#ifdef UARTD_USE_RX
  #ifndef UARTD_RXBUFSIZE
    #define UARTD_RXBUFSIZE      256 //128 //MUST be 2^N
  #endif
  #if UARTD_RXBUFSIZE < 2
    #error UARTD_RXBUFSIZE must be larger than 1 !
  #elif ((UARTD_RXBUFSIZE & (UARTD_RXBUFSIZE-1)) != 0)
    #error UARTD_RXBUFSIZE must be a power of 2 !
  #endif

  #define UARTD_RXBUFSIZEMASK  (UARTD_RXBUFSIZE-1)

  volatile char uartd_rxbuf[UARTD_RXBUFSIZE];
  volatile uint16_t uartd_rxwritepos; // pos at which the last byte was stored
  volatile uint16_t uartd_rxreadpos; // pos at which the next byte is to be fetched

  #ifdef UARTD_USE_RXERRORCOUNT
  volatile uint32_t uartd_errorcnt_rxnoise;
  volatile uint32_t uartd_errorcnt_rxframe;
  volatile uint32_t uartd_errorcnt_rxoverrun;
  #endif
#endif


#if defined UARTD_USE_UART1 || defined UARTD_USE_UART1_REMAPPED || defined UARTD_USE_UART1_REMAPPED2
  #define UARTD_UARTx             USART1
  #ifdef UARTD_USE_UART1
    #define UARTD_TX_IO           IO_PA9
    #define UARTD_RX_IO           IO_PA10
  #elif defined UARTD_USE_UART1_REMAPPED
    #define UARTD_TX_IO           IO_PB6
    #define UARTD_RX_IO           IO_PB7
  #elif defined UARTD_USE_UART1_REMAPPED2 // only G4
    #define UARTD_TX_IO           IO_PC4 // on G4
    #define UARTD_RX_IO           IO_PC5
  #endif
  #define UARTD_IO_AF             IO_AF_7
  #define UARTD_IRQn              USART1_IRQn
  #define UARTD_IRQHandler        USART1_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UARTD_USE_UART2 || defined UARTD_USE_UART2_REMAPPED || defined UARTD_USE_UART2_REMAPPED2 || defined UARTD_USE_UART2_REMAPPED3
  #define UARTD_UARTx             USART2
  #ifdef UARTD_USE_UART2
    #define UARTD_TX_IO           IO_PA2
    #define UARTD_RX_IO           IO_PA3
  #elif defined UARTD_USE_UART2_REMAPPED
    #define UARTD_TX_IO           IO_PD5
    #define UARTD_RX_IO           IO_PD6
  #elif defined UARTD_USE_UART2_REMAPPED2 // only G4
    #define UARTD_TX_IO           IO_PB3
    #define UARTD_RX_IO           IO_PB4
  #elif defined UARTD_USE_UART2_REMAPPED3 // only G4 // ATTENTION: PA14 overlaps with SWCLK
    #define UARTD_TX_IO           IO_PA14
    #define UARTD_RX_IO           IO_PA15
  #endif
  #define UARTD_IO_AF             IO_AF_7
  #define UARTD_IRQn              USART2_IRQn
  #define UARTD_IRQHandler        USART2_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
  #endif

#elif defined UARTD_USE_UART3 || defined UARTD_USE_UART3_REMAPPED
  #define UARTD_UARTx             USART3
  #if defined UARTD_USE_UART3 && defined STM32F3
    #define UARTD_TX_IO           IO_PB8
    #define UARTD_RX_IO           IO_PB9
  #elif defined UARTD_USE_UART3
    #define UARTD_TX_IO           IO_PB10
    #define UARTD_RX_IO           IO_PB11
  #else
    #define UARTD_TX_IO           IO_PC10
    #define UARTD_RX_IO           IO_PC11
  #endif
  #define UARTD_IO_AF             IO_AF_7
  #define UARTD_IRQn              USART3_IRQn
  #define UARTD_IRQHandler        USART3_IRQHandler
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32L4
  #elif defined STM32WL
    #error UART3 NOT AVAILABLE !
  #endif

#elif defined UARTD_USE_UART4 || defined UARTD_USE_UART4_REMAPPED
  #define UARTD_UARTx             UART4
  #ifdef UARTD_USE_UART4
    #define UARTD_TX_IO           IO_PC10
    #define UARTD_RX_IO           IO_PC11
  #else
    #define UARTD_TX_IO           IO_PA0 // only F7 !
    #define UARTD_RX_IO           IO_PA1
  #endif
  #define UARTD_IRQn              UART4_IRQn
  #define UARTD_IRQHandler        UART4_IRQHandler
  #if defined STM32F1
    #define UARTD_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F3
    #warning UARTD IO_AF & DMA for F3 still TODO
  #elif defined STM32F7
    #define UARTD_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UARTD_IO_AF           IO_AF_5
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART4 NOT AVAILABLE !
  #endif

#elif defined UARTD_USE_UART5
  #define UARTD_UARTx             UART5
  #define UARTD_TX_IO             IO_PC12
  #define UARTD_RX_IO             IO_PD2
  #define UARTD_IRQn              UART5_IRQn
  #define UARTD_IRQHandler        UART5_IRQHandler
  #if defined STM32F1
    #define UARTD_IO_AF           IO_AF_DEFAULT
  #elif defined STM32F3
    #warning UARTD IO_AF & DMA for F3 still TODO //TODO
  #elif defined STM32F7
    #define UARTD_IO_AF           IO_AF_8
  #elif defined STM32G4
    #define UARTD_IO_AF           IO_AF_5
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
    #error UART5 NOT AVAILABLE !
  #endif

#elif defined UARTD_USE_UART6 || defined UARTD_USE_UART6_REMAPPED
  #define UARTD_UARTx             USART6
  #ifdef UARTD_USE_UART6
    #define UARTD_TX_IO           IO_PC6
    #define UARTD_RX_IO           IO_PC7
  #else
    #define UARTD_TX_IO           IO_PG14
    #define UARTD_RX_IO           IO_PG9
  #endif
  #define UARTD_IO_AF             IO_AF_8
  #define UARTD_IRQn              USART6_IRQn
  #define UARTD_IRQHandler        USART6_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART6 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTD_USE_UART7 || defined UARTD_USE_UART7_REMAPPED
  #define UARTD_UARTx             UART7
  #ifdef UARTD_USE_UART7
    #define UARTD_TX_IO           IO_PE8
    #define UARTD_RX_IO           IO_PE7
  #else
    #define UARTD_TX_IO           IO_PF7
    #define UARTD_RX_IO           IO_PF6
  #endif
  #define UARTD_IO_AF             IO_AF_8
  #define UARTD_IRQn              UART7_IRQn
  #define UARTD_IRQHandler        UART7_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART7 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTD_USE_UART8
  #define UARTD_UARTx             UART8
  #define UARTD_TX_IO             IO_PE1
  #define UARTD_RX_IO             IO_PE0
  #define UARTD_IO_AF             IO_AF_8
  #define UARTD_IRQn              UART8_IRQn
  #define UARTD_IRQHandler        UART8_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32G4 || defined STM32WL
    #error UART8 NOT AVAILABLE !
  #elif defined STM32F7
  #elif defined STM32L4
    #error TODO ?!?
  #endif

#elif defined UARTD_USE_LPUART1 || defined UARTD_USE_LPUART1_REMAPPED
  #define UARTD_UARTx             LPUART1
  #ifdef UARTD_USE_LPUART1
    #define UARTD_TX_IO           IO_PA2
    #define UARTD_RX_IO           IO_PA3
    #define UARTD_IO_AF           IO_AF_12
  #elif defined UARTD_USE_LPUART1_REMAPPED
    #define UARTD_TX_IO           IO_PC1
    #define UARTD_RX_IO           IO_PC0
    #define UARTD_IO_AF           IO_AF_8
  #endif
  #define UARTD_IRQn              LPUART1_IRQn
  #define UARTD_IRQHandler        LPUART1_IRQHandler
  #if defined STM32F1 || defined STM32F3 || defined STM32F7
    #error LPUART1 NOT AVAILABLE !
  #elif defined STM32G4
  #elif defined STM32L4
    #error TODO ?!?
  #elif defined STM32WL
  #endif

#else
  #error No UARTD_USE_UARTx defined!
#endif


// allows us to overwrite the default pin assignment
#ifdef UARTD_USE_TX_IO
  #undef UARTD_TX_IO
  #define UARTD_TX_IO             UARTD_USE_TX_IO
#endif
#ifdef UARTD_USE_RX_IO
  #undef UARTD_RX_IO
  #define UARTD_RX_IO             UARTD_USE_RX_IO
#endif
#ifdef UARTD_USE_IO_AF
  #undef UARTD_IO_AF
  #define UARTD_IO_AF             UARTD_USE_IO_AF
#endif


//-------------------------------------------------------
// COMMENT:
// LL_USART_XX and LL_LPUART_XX flags are just redefines of USART_XX flags.
// So, we can use USART_XX here and below.
// This has also the advantage that it works for both USART and LPUART.
//
// Also, both USART and LPUART use the same structure USART_TypeDef.
// All LL_USART_xx() and LL_LPUART_xx() functions which only use it and relate to flags or 
// registers can be used for both USART and LPUART. 
// Below we simply use the LL_USART_xx() functions for both.


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
#if defined UARTD_USE_RX || defined UARTD_USE_TX_ISR

void UARTD_IRQHandler(void)
{
  // Cube HAL for f103rc is also calling this first independent on irqs
  uint16_t usart_sr = LL_USART_ReadReg(UARTD_UARTx, REG_SR); // read USART_SR register

#ifdef UARTD_USE_RX
  if (usart_sr & FLAG_SR_RXNE) {
#ifdef UARTD_USE_RXERRORCOUNT
    if (usart_sr & FLAG_SR_ORE) uartd_errorcnt_rxoverrun++;
    if (usart_sr & FLAG_SR_NE) uartd_errorcnt_rxnoise++;
    if (usart_sr & FLAG_SR_FE) uartd_errorcnt_rxframe++;
#endif
    // Cube LL driver (for f103?) says that reading SR and then writing to DR will clear error flags, so do it after error checks
    uint8_t usart_dr = LL_USART_ReceiveData8(UARTD_UARTx); // read USART_DR register, clears RXNE and RX error flags
#ifdef UARTD_RX_CALLBACK_FULL
    UARTD_RX_CALLBACK_FULL(usart_dr);
#else
    uint16_t next = (uartd_rxwritepos + 1) & UARTD_RXBUFSIZEMASK;
    if (uartd_rxreadpos != next) { // fifo not full
      uartd_rxbuf[next] = usart_dr;
      uartd_rxwritepos = next;
    }
#endif
  }
#if defined STM32F1
  // ORE is enabled along with RXNE, so we need to handle it
  // ORE bit is reset by read to USART_SR followed by read to USART_DR
  if (usart_sr & FLAG_SR_ORE) {
    LL_USART_ReceiveData8(UARTD_UARTx); // read USART_DR register, clears RXNE and RX error flags
  }
#elif defined STM32F3
  // ORE is enabled along with RXNE, but can be cleared by writing 1 to ICR
  LL_USART_WriteReg(UARTD_UARTx, ICR, (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NCF | LL_USART_ICR_FECF | LL_USART_ICR_PECF));
#elif defined STM32F7
  #warning UARTD F7 check clear flags, should be LL !?
  // these are cleared by write 1 to the ICR register, not by a read of DR register!
  LL_USART_WriteReg(UARTD_UARTx, ICR, (USART_ICR_IDLECF | USART_ICR_ORECF | USART_ICR_NCF | USART_ICR_FECF | USART_ICR_PECF));
#elif defined STM32G4 || defined STM32L4 || defined STM32WL
  LL_USART_WriteReg(UARTD_UARTx, ICR, (LL_USART_ICR_IDLECF | LL_USART_ICR_ORECF | LL_USART_ICR_NECF | LL_USART_ICR_FECF | LL_USART_ICR_PECF));
#endif
#endif

#ifdef UARTD_USE_TX_ISR
  if (usart_sr & FLAG_SR_TXE) {
    if (uartd_txwritepos != uartd_txreadpos) { // fifo not empty
      uartd_txreadpos = (uartd_txreadpos + 1) & UARTD_TXBUFSIZEMASK;
      LL_USART_TransmitData8(UARTD_UARTx, uartd_txbuf[uartd_txreadpos]); // write to USART_DR register, clears TXE flag, as well as TC
    } else {
      LL_USART_DisableIT_TXE(UARTD_UARTx); // disable interrupt when empty
#ifdef UARTD_TC_CALLBACK
      // ATTENTION: doesn't distinguish between empty fifo and last byte just send!
      LL_USART_ClearFlag_TC(UARTD_UARTx); // now enable TC interrupt
      LL_USART_EnableIT_TC(UARTD_UARTx);
#endif
    }
  }

#ifdef UARTD_TC_CALLBACK
  if (usart_sr & FLAG_SR_TC) {
    LL_USART_DisableIT_TC(UARTD_UARTx);
    LL_USART_ClearFlag_TC(UARTD_UARTx);
    UARTD_TC_CALLBACK();
  }
#endif
#endif
}

#endif // defined UARTD_USE_RX || defined UARTD_USE_TX_ISR


//-------------------------------------------------------
// TX routines
//-------------------------------------------------------
#ifdef UARTD_USE_TX

uint16_t uartd_putc(char c)
{
#ifndef UARTD_USE_TX_ISR
  while (!LL_USART_IsActiveFlag_TC(UARTD_UARTx)) {}
  LL_USART_TransmitData8(UARTD_UARTx, c);
  return 1;
#else
  uint16_t next = (uartd_txwritepos + 1) & UARTD_TXBUFSIZEMASK;
  if (uartd_txreadpos != next) { // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
    uartd_txbuf[next] = c;
    uartd_txwritepos = next;
    LL_USART_EnableIT_TXE(UARTD_UARTx); // enable interrupt // then TDR is empty, the TXE bit is set, and hence the isr is triggered
    return 1;
  }
  return 0;
#endif
}


void uartd_puts(const char *s)
{
  while (*s) {
    uartd_putc(*s);
    s++;
  }
}


void uartd_putbuf(uint8_t* buf, uint16_t len)
{
uint16_t i;

  for (i = 0; i < len; i++) uartd_putc(buf[i]);
}


#ifdef UARTD_USE_TX_ISR
uint16_t uartd_tx_notfull(void)
{
  uint16_t next = (uartd_txwritepos + 1) & UARTD_TXBUFSIZEMASK;
  return (uartd_txreadpos != next); // fifo not full // this is isr safe, works also if readpos has changed in the meanwhile
}


static inline uint16_t uartd_tx_isempty(void)
{
  if (uartd_txwritepos == uartd_txreadpos) return 1;
  return 0;
}


static inline void uartd_tx_flush(void)
{
  uartd_txwritepos = uartd_txreadpos = 0;
}
#else
static inline void uartd_tx_flush(void) {}
#endif

#endif // defined UARTD_USE_TX


//-------------------------------------------------------
// RX routines
//-------------------------------------------------------
#ifdef UARTD_USE_RX

static inline uint16_t uartd_rx_available(void)
{
  if (uartd_rxwritepos == uartd_rxreadpos) return 0; // fifo empty
  return 1;
}


uint16_t uartd_rx_bytesavailable(void)
{
int16_t d;

  d = (int16_t)uartd_rxwritepos - (int16_t)uartd_rxreadpos;
  return (d < 0) ? d + (UARTD_RXBUFSIZEMASK + 1) : d;
}


char uartd_getc(void)
{
  while (uartd_rxwritepos == uartd_rxreadpos) {};
  uartd_rxreadpos = (uartd_rxreadpos + 1) & UARTD_RXBUFSIZEMASK;
  return uartd_rxbuf[uartd_rxreadpos];
}


static inline char uartd_lookatnextc(void)
{
  return uartd_rxbuf[(uartd_rxreadpos + 1) & UARTD_RXBUFSIZEMASK];
}


static inline void uartd_rx_flush(void)
{
  uartd_rxwritepos = uartd_rxreadpos = 0;
}

#endif // defined UARTD_USE_RX


//-------------------------------------------------------
// INIT routines
//-------------------------------------------------------

#if !(defined UARTD_USE_LPUART1 || defined UARTD_USE_LPUART1_REMAPPED) || defined STM32WL
// for the moment, we don't support these functions for LPUART, except for WLE5xx

void uartd_setprotocol(uint32_t baud, UARTPARITYENUM parity, UARTSTOPBITENUM stopbits)
{
LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_USART_StructInit(&USART_InitStruct);
  USART_InitStruct.BaudRate = baud;
  USART_InitStruct.DataWidth = (parity != LL_USART_PARITY_NONE) ? LL_USART_DATAWIDTH_9B : LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = stopbits;
  USART_InitStruct.Parity = parity;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  LL_USART_Disable(UARTD_UARTx); // must be disabled to configure some registers
  LL_USART_Init(UARTD_UARTx, &USART_InitStruct);
  LL_USART_Enable(UARTD_UARTx);
}


void uartd_setbaudrate(uint32_t baud)
{
LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_USART_StructInit(&USART_InitStruct);
  USART_InitStruct.BaudRate = baud;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  LL_USART_Disable(UARTD_UARTx); // must be disabled to configure some registers
  LL_USART_Init(UARTD_UARTx, &USART_InitStruct);
  LL_USART_Enable(UARTD_UARTx);
}


void uartd_tx_enable(FunctionalState flag)
{
#ifdef UARTD_USE_TX
  if (flag == ENABLE) {
    gpio_change_af(UARTD_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UARTD_IO_AF, IO_SPEED_VERYFAST);
  } else {
    gpio_change(UARTD_TX_IO, IO_MODE_INPUT_ANALOG, IO_SPEED_VERYFAST);
  }
#endif
}

#endif


void uartd_rx_enableisr(FunctionalState flag)
{
#ifdef UARTD_USE_RX
  if (flag == ENABLE) {
    // Enable Receive Data register not empty interrupt
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UARTD_UARTx);
#endif
    LL_USART_EnableIT_RXNE(UARTD_UARTx);
  } else {
    LL_USART_DisableIT_RXNE(UARTD_UARTx);
#if defined STM32F1
    LL_USART_ClearFlag_RXNE(UARTD_UARTx);
#endif
  }
#endif
}


void uartd_init_isroff(void)
{
  LL_USART_Disable(UARTD_UARTx);

  // Initialize clocks for USART/LPUART, GPIO, AFIO
  rcc_init_afio();
  rcc_init_uart(UARTD_UARTx);
#ifdef STM32F1
#if defined UARTD_USE_UART1_REMAPPED
  LL_GPIO_AF_EnableRemap_USART1();
#endif
#if defined UARTD_USE_UART2_REMAPPED
  LL_GPIO_AF_EnableRemap_USART2();
#endif
#if defined UARTD_USE_UART3_REMAPPED
  LL_GPIO_AF_RemapPartial_USART3();
#endif
#endif // STM32F1

  // Configure pin USART/LPUART Tx as alternative function push-pull
#ifdef UARTD_USE_TX
  gpio_init_af(UARTD_TX_IO, IO_MODE_OUTPUT_ALTERNATE_PP, UARTD_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure pin USART/LPUART Rx as input with pull up
#ifdef UARTD_USE_RX
  gpio_init_af(UARTD_RX_IO, IO_MODE_INPUT_PU, UARTD_IO_AF, IO_SPEED_VERYFAST);
#endif

  // Configure USART/LPUART
#if !(defined UARTD_USE_LPUART1 || defined UARTD_USE_LPUART1_REMAPPED) || defined STM32WL
LL_USART_InitTypeDef USART_InitStruct = {0};
  USART_InitStruct.BaudRate = UARTD_BAUD;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
#if defined STM32G4 || defined STM32WL
  USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
#endif
  LL_USART_Init(UARTD_UARTx, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(UARTD_UARTx);
#if defined STM32G4 || defined STM32WL
  LL_USART_DisableFIFO(UARTD_UARTx);
  LL_USART_SetTXFIFOThreshold(UARTD_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(UARTD_UARTx, LL_USART_FIFOTHRESHOLD_1_8);
#endif
#else
LL_LPUART_InitTypeDef LPUART_InitStruct = {0};
  LPUART_InitStruct.BaudRate = UARTD_BAUD;
  LPUART_InitStruct.DataWidth = LL_LPUART_DATAWIDTH_8B;
  LPUART_InitStruct.StopBits = LL_LPUART_STOPBITS_1;
  LPUART_InitStruct.Parity = LL_LPUART_PARITY_NONE;
  LPUART_InitStruct.TransferDirection = LL_LPUART_DIRECTION_TX_RX;
  LPUART_InitStruct.HardwareFlowControl = LL_LPUART_HWCONTROL_NONE;
  LPUART_InitStruct.PrescalerValue = LL_LPUART_PRESCALER_DIV1;
  LL_LPUART_Init(UARTD_UARTx, &LPUART_InitStruct);
  LL_LPUART_DisableFIFO(UARTD_UARTx);
  LL_LPUART_SetTXFIFOThreshold(UARTD_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
  LL_LPUART_SetRXFIFOThreshold(UARTD_UARTx, LL_LPUART_FIFOTHRESHOLD_1_8);
#endif

  // Configure NVIC channel
#if defined UARTD_USE_RX || defined UARTD_USE_TX_ISR
#ifdef UARTD_PREEMPTIONPRIORITY
  #error Legacy UARTD_PREEMPTIONPRIORITY used !
#endif
#ifdef UARTD_SUBPRIORITY
  #error Legacy UARTD_SUBPRIORITY used !
#endif
#ifdef UARTD_PRIORITY
#error Legacy UARTD_PRIORITY used !
#endif
#ifndef UARTD_IRQ_PRIORITY
  #define UARTD_IRQ_PRIORITY  15 // set priority to lowest
#endif
  nvic_irq_enable_w_priority(UARTD_IRQn, UARTD_IRQ_PRIORITY);
#endif

#ifdef UARTD_USE_TX_ISR
  // Disable Transmit Data Register empty interrupt
  LL_USART_DisableIT_TXE(UARTD_UARTx);
  LL_USART_ClearFlag_TC(UARTD_UARTx);
  // Flush buffer
  uartd_txwritepos = uartd_txreadpos = 0;
#endif

#ifdef UARTD_USE_RX
  // Disable Receive Data register not empty interrupt
#ifdef STM32F1
  LL_USART_ClearFlag_RXNE(UARTD_UARTx);
#endif
  LL_USART_EnableIT_RXNE(UARTD_UARTx);
  // Flush buffer
  uartd_rxwritepos = uartd_rxreadpos = 0;
#ifdef UARTD_USE_RXERRORCOUNT
  uartd_errorcnt_rxnoise = 0;
  uartd_errorcnt_rxframe = 0;
  uartd_errorcnt_rxoverrun = 0;
#endif
#endif

  // Cube HAL for f103rc
  /* In asynchronous mode, the following bits must be kept cleared:
     - LINEN and CLKEN bits in the USART_CR2 register,
     - SCEN, HDSEL and IREN  bits in the USART_CR3 register.*/
  CLEAR_BIT(UARTD_UARTx->CR2, (USART_CR2_LINEN | USART_CR2_CLKEN));
  CLEAR_BIT(UARTD_UARTx->CR3, (USART_CR3_SCEN | USART_CR3_HDSEL | USART_CR3_IREN));

#if defined STM32F7
  // reading the F722 manual I've added this
  CLEAR_BIT(UARTD_UARTx->CR1, (USART_CR1_EOBIE | USART_CR1_RTOIE | USART_CR1_CMIE | USART_CR1_PEIE | USART_CR1_IDLEIE));
  CLEAR_BIT(UARTD_UARTx->CR2, (USART_CR2_RTOEN | USART_CR2_CPOL | USART_CR2_CPHA | USART_CR2_LBCL | USART_CR2_LBDIE | USART_CR2_LBDL));
  CLEAR_BIT(UARTD_UARTx->CR3, (USART_CR3_TCBGTIE | USART_CR3_CTSIE | USART_CR3_EIE));
#if !defined UARTD_USE_RXERRORCOUNT
  // Disable ORE in the first place
  LL_USART_DisableOverrunDetect(UARTD_UARTx); // disables ORE, check how F1 is behaving
#endif
#endif

#if defined STM32G4 || defined STM32WL
#if !defined UARTD_USE_RXERRORCOUNT
  LL_USART_DisableOverrunDetect(UARTD_UARTx);
#endif
#endif

  // Enable USART/LPUART
  LL_USART_Enable(UARTD_UARTx);

#if defined STM32G4 || defined STM32WL
  // Polling UART/LPUART initialisation
  while((!(LL_USART_IsActiveFlag_TEACK(UARTD_UARTx))) || (!(LL_USART_IsActiveFlag_REACK(UARTD_UARTx)))) {};
#endif
}


void uartd_init(void)
{
  uartd_init_isroff();
  uartd_rx_enableisr(ENABLE);
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_UARTD_H
