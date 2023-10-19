//*******************************************************
// Copyright (c) OlliW, OlliW42, www.olliw.eu
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// my stripped down SPI standard library
// no ISR, no DMA, no DATASIZEWORD, no bit convenience functions
//*******************************************************
// Interface:
//
// #define SPI$_USE_SPI1, SPI$_USE_SPI2, SPI$_USE_SPI3, SPI$_USE_SUBGHZSPI
// #define SPI$_USE_DMA
// #define SPI$_DMA_PRIORITY
//
// #define SPI$_CS_IO
//
// #define SPI$_USE_CLK_LOW_1EDGE
// #define SPI$_USE_CLK_LOW_2EDGE
// #define SPI$_USE_CLK_HIGH_1EDGE
// #define SPI$_USE_CLK_HIGH_2EDGE
//
// #defined SPI$_USE_CLOCKSPEED_36MHZ
// #defined SPI$_USE_CLOCKSPEED_18MHZ
// #defined SPI$_USE_CLOCKSPEED_9MHZ
// #defined SPI$_USE_CLOCKSPEED_4500KHZ
// #defined SPI$_USE_CLOCKSPEED_2250KHZ
// #defined SPI$_USE_CLOCKSPEED_1125KHZ
// #defined SPI$_USE_CLOCKSPEED_562KHZ
// #defined SPI$_USE_CLOCKSPEED_281KHZ
//
// #defined SPI$_USE_DATASIZEWORD // uses word transfer instead of byte transfer
//
//*******************************************************
#ifndef STDSTM32_SPI$_H
#define STDSTM32_SPI$_H
#ifdef __cplusplus
extern "C" {
#endif


//-------------------------------------------------------
//  SPI Enums
//-------------------------------------------------------
// in order to not introduce hardware dependency upstream
// we define our own enums

#ifndef SPI_ENUMS
#define SPI_ENUMS

typedef enum {
  SPI_36MHZ = 0,
  SPI_18MHZ,
  SPI_9MHZ,
  SPI_4p5MHZ,
  SPI_2p25MHZ,
  SPI_1p125MHZ,
  SPI_562p5KHZ,
  SPI_281p25KHZ,
} SPICLOCKSPEEDENUM;

typedef enum {
  SPI_MODE_LOW_1EDGE = 0, // mode 0 <=> cpol = 0, cpha = 0
  SPI_MODE_LOW_2EDGE,     // mode 1 <=> cpol = 0, cpha = 1
  SPI_MODE_HIGH_1EDGE,    // mode 2 <=> cpol = 1, cpha = 0
  SPI_MODE_HIGH_2EDGE,    // mode 3 <=> cpol = 1, cpha = 1
} SPIMODEENUM;

typedef enum {
  SPI_FAIL = 0,
  SPI_PASS,
} SPIRESOPONSEENUM;

typedef enum {
  SPI_ISNOTREADY = 0,
  SPI_ISREADY,
} SPIREADYENUM;

#endif


//-------------------------------------------------------
// Defines
//-------------------------------------------------------

#include "stdstm32-peripherals.h"
#if defined SPI$_USE_SUBGHZSPI && defined STM32WL
#include "stdstm32-subghz.h"
#endif

#if defined SPI$_USE_SPI1 || defined SPI$_USE_SPI1_PB3PB4PB5
  #define SPI$_SPIx                SPI1
  #if defined SPI$_USE_SPI1_PB3PB4PB5 || defined STM32F3
    #define SPI$_SCK_IO            IO_PB3
    #define SPI$_MISO_IO           IO_PB4
    #define SPI$_MOSI_IO           IO_PB5
  #else
    #define SPI$_SCK_IO            IO_PA5
    #define SPI$_MISO_IO           IO_PA6
    #define SPI$_MOSI_IO           IO_PA7
  #endif
  #ifndef STM32F0
    #define SPI$_IO_AF             IO_AF_5
  #else
    #define SPI$_IO_AF             IO_AF_0
  #endif
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32WL
  #elif defined STM32F0
  #endif
#endif
#ifdef SPI$_USE_SPI2
  #define SPI$_SPIx                SPI2
  #define SPI$_SCK_IO              IO_PB13 // IO_PB10 ?
  #define SPI$_MISO_IO             IO_PB14 // IO_PC1 ?
  #define SPI$_MOSI_IO             IO_PB15 // IO_PC2 ?
  #ifndef STM32F0
    #define SPI$_IO_AF             IO_AF_5
  #else
    #define SPI$_IO_AF             IO_AF_0
  #endif
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32WL
  #elif defined STM32F0
  #endif
#endif
#ifdef SPI$_USE_SPI3
  #define SPI$_SPIx                SPI3
  #define SPI$_SCK_IO              IO_PB3
  #define SPI$_MISO_IO             IO_PB4
  #define SPI$_MOSI_IO             IO_PB5
  #ifndef STM32G4
    #define SPI$_IO_AF             IO_AF_5
  #else
    #define SPI$_IO_AF             IO_AF_6
  #endif
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32WL || defined STM32F0
    #error SPI3 NOT AVAILABLE !
  #endif
#endif
#ifdef SPI$_USE_SUBGHZSPI
  #define SPI$_SPIx                SUBGHZSPI
  #if !defined STM32WL
    #error SUBGHZSPI NOT AVAILABLE !
  #endif
#endif
#ifndef SPI$_SPIx
  #error No SPI defined !
#endif


// allows us to overwrite the default pin assignment
#ifdef SPI$_USE_SCK_IO
  #undef SPI$_SCK_IO
  #define SPI$_SCK_IO              SPI$_USE_SCK_IO
#endif
#ifdef SPI$_USE_MISO_IO
  #undef SPI$_MISO_IO
  #define SPI$_MISO_IO             SPI$_USE_MISO_IO
#endif
#ifdef SPI$_USE_MOSI_IO
  #undef SPI$_MOSI_IO
  #define SPI$_MOSI_IO             SPI$_USE_MOSI_IO
#endif
#ifdef SPI$_USE_IO_AF
  #undef SPI$_IO_AF
  #define SPI$_IO_AF               SPI$_USE_IO_AF
#endif


// allows us to set delays in CS activities
#ifndef SPI$_SELECT_PRE_DELAY
  #define SPI$_SELECT_PRE_DELAY
#endif
#ifndef SPI$_SELECT_POST_DELAY
  #define SPI$_SELECT_POST_DELAY
#endif
#ifndef SPI$_DESELECT_PRE_DELAY
  #define SPI$_DESELECT_PRE_DELAY
#endif
#ifndef SPI$_DESELECT_POST_DELAY
  #define SPI$_DESELECT_POST_DELAY
#endif


//-------------------------------------------------------
//  SPI low level routines
//-------------------------------------------------------

void spi$_disable(void)
{
  // according to reference manual
  while (!LL_SPI_IsActiveFlag_RXNE(SPI$_SPIx)) {};
  while (!LL_SPI_IsActiveFlag_TXE(SPI$_SPIx)) {};
  while (LL_SPI_IsActiveFlag_BSY(SPI$_SPIx)) {};
  LL_SPI_Disable(SPI$_SPIx);
}


//-- select functions

#ifdef SPI$_CS_IO

static inline void spi$_select(void)
{
  SPI$_SELECT_PRE_DELAY;
  gpio_low(SPI$_CS_IO); // CS = low
  SPI$_SELECT_POST_DELAY;
}


static inline void spi$_deselect(void)
{
  SPI$_DESELECT_PRE_DELAY;
  gpio_high(SPI$_CS_IO); // CS = high
  SPI$_DESELECT_POST_DELAY;
}

#elif defined SPI$_USE_SUBGHZSPI
    
static inline void spi$_select(void)
{
  SPI$_SELECT_PRE_DELAY;
  LL_PWR_SelectSUBGHZSPI_NSS();
  SPI$_SELECT_POST_DELAY;
}


static inline void spi$_deselect(void)
{
  SPI$_DESELECT_PRE_DELAY;
  LL_PWR_UnselectSUBGHZSPI_NSS();
  SPI$_DESELECT_POST_DELAY;
}

#endif


//-- transmit, transfer, read, write functions

// is blocking
uint8_t spi$_transmitchar(uint8_t c)
{
#ifdef SPI$_USE_SUBGHZSPI
  while (!LL_SPI_IsActiveFlag_TXE(SPI$_SPIx)) {}; // we don't do that originally, but it's suggested by cubemx    
#endif  
  LL_SPI_TransmitData8(SPI$_SPIx, c);
  while (!LL_SPI_IsActiveFlag_RXNE(SPI$_SPIx)) {};
  return LL_SPI_ReceiveData8(SPI$_SPIx);
}


// is blocking
void spi$_transfer(uint8_t* dataout, uint8_t* datain, uint16_t len)
{
  while (len) {
    *datain = spi$_transmitchar(*dataout);
    dataout++;
    datain++;
    len--;
  }
}


// sends a dummy char, returns the received byte
// is blocking
static inline uint8_t spi$_readchar(void)
{
  return spi$_transmitchar(0xFF);
}


// sends a char, ignores the received byte
// is blocking
static inline void spi$_writechar(uint8_t c)
{
  spi$_transmitchar(c);
}


// sends a word, returns the received word
// is blocking
static inline uint16_t spi$_transmitword(uint16_t w)
{
  return (((uint16_t)spi$_transmitchar((uint8_t)(w >> 8))) << 8) + spi$_transmitchar((uint8_t)(w));
}


// sends a dummy word, returns the received word
// is blocking
static inline uint16_t spi$_readword(void)
{
  return (((uint16_t)spi$_transmitchar(0xFF)) << 8) + spi$_transmitchar(0xFF);
}


// sends a word, ignores the received word
// is blocking
static inline void spi$_writeword(uint16_t w)
{
  spi$_transmitchar((uint8_t)(w >> 8));
  spi$_transmitchar((uint8_t)(w));
}


// is blocking
void spi$_read(uint8_t* data, uint16_t len)
{
  while (len) {
    *data = spi$_readchar();
    data++;
    len--;
  }
}


// is blocking
void spi$_write(uint8_t* data, uint16_t len)
{
  while (len) {
    spi$_writechar(*data);
    data++;
    len--;
  }
}


// is blocking
void spi$_writecandread(uint8_t c, uint8_t* data, uint16_t datalen)
{
  spi$_writechar(c);
  while (datalen) {
    *data = spi$_readchar();
    data++;
    datalen--;
  }
}


//-------------------------------------------------------
// INIT routines
//-------------------------------------------------------

#if !defined SPI$_USE_SUBGHZSPI

uint32_t _spi$_baudrate(SPICLOCKSPEEDENUM speed)
{
#if defined STM32F1 // SPI1 is on 72 MHz, SPI2 & SPI3 are on 36 MHz
  switch (speed) {
    // case SPI_36MHZ: not possible !
#ifdef SPI$_USE_SPI1
    case SPI_18MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV4;
    case SPI_9MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV8; // 72 MHz / 8 = 9 MHz
    case SPI_4p5MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV16;
    case SPI_2p25MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV32;
    case SPI_1p125MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV64;
    case SPI_562p5KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV128;
    case SPI_281p25KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV256;
#else
    case SPI_18MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV2;
    case SPI_9MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV4; // 36 MHz / 4 = 9 MHz
    case SPI_4p5MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV8;
    case SPI_2p25MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV16;
    case SPI_1p125MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV32;
    case SPI_562p5KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV64;
    case SPI_281p25KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV128;
#endif
    default:
      return LL_SPI_BAUDRATEPRESCALER_DIV256;
  }

#elif defined STM32F3 // SPI1 & SPI2 & SPI3 are on 36 MHz
  switch (speed) {
    // case SPI_36MHZ: not possible !
    case SPI_18MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV2;
    case SPI_9MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV4;
    case SPI_4p5MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV8;
    case SPI_2p25MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV16;
    case SPI_1p125MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV32;
    case SPI_562p5KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV64;
    case SPI_281p25KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV128;
    default:
      return LL_SPI_BAUDRATEPRESCALER_DIV256;
  }

#elif defined STM32F7
#ifdef SPI$_USE_SPI1
    switch (speed) {
      case SPI_36MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV4; // 27 Mhz
      case SPI_18MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV8;
      case SPI_9MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV16; // 6.75 Mbit/s
      case SPI_4p5MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV32;
      case SPI_2p25MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV64;
      case SPI_1p125MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV128;
      case SPI_562p5KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV256;
      // case SPI_281p25KHZ: not possible !
      default:
        return LL_SPI_BAUDRATEPRESCALER_DIV256;
    }
#else
    switch (speed) {
      case SPI_36MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV2; // 27 Mhz
      case SPI_18MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV4;
      case SPI_9MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV8; // 6.75 Mbit/s
      case SPI_4p5MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV16;
      case SPI_2p25MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV32;
      case SPI_1p125MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV64;
      case SPI_562p5KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV128;
      case SPI_281p25KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV256;
      default:
        return LL_SPI_BAUDRATEPRESCALER_DIV256;
    }
#endif

#elif defined STM32G4
  switch (speed) {
    case SPI_36MHZ: // not possible ! DIV4 would give 42.5 MHz, which is > 40 MHz, so use DIV8
    case SPI_18MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV8; // 21.25 MHz
    case SPI_9MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV16; // 10.625 Mbit/s
    case SPI_4p5MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV32;
    case SPI_2p25MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV64;
    case SPI_1p125MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV128;
    case SPI_562p5KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV256;
    // case SPI_281p25KHZ: not possible !
    default:
      return LL_SPI_BAUDRATEPRESCALER_DIV256;
  }

#elif defined STM32L4
  switch (speed) {
    case SPI_36MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV2; // 40 MHz
    case SPI_18MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV4;
    case SPI_9MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV8; // 10 Mbit/s
    case SPI_4p5MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV16;
    case SPI_2p25MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV32;
    case SPI_1p125MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV64;
    case SPI_562p5KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV128;
    case SPI_281p25KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV256;
    // case SPI_281p25KHZ: not possible !
    default:
      return LL_SPI_BAUDRATEPRESCALER_DIV256;
  }
  
#elif defined STM32WL || defined STM32F0 // all SPI are on 48 MHz
  switch (speed) {
    case SPI_36MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV2; // 24 MHz
    case SPI_18MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV4; // 12 MHz
    case SPI_9MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV8; // 6 MHz
    case SPI_4p5MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV16;
    case SPI_2p25MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV32;
    case SPI_1p125MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV64;
    case SPI_562p5KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV128;
    case SPI_281p25KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV256;
    default:
      return LL_SPI_BAUDRATEPRESCALER_DIV256;
  }
 
#endif
}


// datasheet: this bit should not be changed when communication is ongoing
void spi$_setmode(SPIMODEENUM mode)
{
  LL_SPI_Disable(SPI$_SPIx); // is important to do that

  switch (mode) {
    case SPI_MODE_LOW_1EDGE:
      LL_SPI_SetClockPolarity(SPI$_SPIx, LL_SPI_POLARITY_LOW);
      LL_SPI_SetClockPhase(SPI$_SPIx, LL_SPI_PHASE_1EDGE);
      break;
    case SPI_MODE_LOW_2EDGE:
      LL_SPI_SetClockPolarity(SPI$_SPIx, LL_SPI_POLARITY_LOW);
      LL_SPI_SetClockPhase(SPI$_SPIx, LL_SPI_PHASE_2EDGE);
      break;
    case SPI_MODE_HIGH_1EDGE:
      LL_SPI_SetClockPolarity(SPI$_SPIx, LL_SPI_POLARITY_HIGH);
      LL_SPI_SetClockPhase(SPI$_SPIx, LL_SPI_PHASE_1EDGE);
      break;
    case SPI_MODE_HIGH_2EDGE:
      LL_SPI_SetClockPolarity(SPI$_SPIx, LL_SPI_POLARITY_HIGH);
      LL_SPI_SetClockPhase(SPI$_SPIx, LL_SPI_PHASE_2EDGE);
      break;
    default:
      while (1) {};
  }

  LL_SPI_Enable(SPI$_SPIx);
  while (!LL_SPI_IsActiveFlag_TXE(SPI$_SPIx)) {}
  (void)LL_SPI_ReceiveData8(SPI$_SPIx);
}

#endif // !SPI$_USE_SUBGHZSPI


void spi$_init(void)
{
LL_SPI_InitTypeDef SPI_InitStruct = {};

  // Initialize clocks for SPI, AFIO
  rcc_init_afio();
  rcc_init_spi(SPI$_SPIx);
#ifdef SPI$_USE_SUBGHZSPI
  subghz_init();
#endif

#if defined SPI$_USE_SPI3 && defined STM32F1
  LL_GPIO_AF_Remap_SWJ_NOJTAG();
#endif

#ifndef SPI$_USE_SUBGHZSPI

  // Configure pin CS
#ifdef SPI$_CS_IO
  gpio_init(SPI$_CS_IO, IO_MODE_OUTPUT_PP_HIGH, IO_SPEED_VERYFAST);
  spi$_deselect(); // deselect SPI: CS high
#endif

  // Configure pin SCK MOSI as alternative function push-pull, pin MISO as input with pull up
  gpio_init_af(SPI$_SCK_IO, IO_MODE_OUTPUT_ALTERNATE_PP, SPI$_IO_AF, IO_SPEED_VERYFAST);
  gpio_init_af(SPI$_MOSI_IO, IO_MODE_OUTPUT_ALTERNATE_PP, SPI$_IO_AF, IO_SPEED_VERYFAST);
  gpio_init_af(SPI$_MISO_IO, IO_MODE_INPUT_PU, SPI$_IO_AF, IO_SPEED_VERYFAST);

#endif // !SPI$_USE_SUBGHZSPI

  // Configure SPIx
#ifndef SPI$_USE_SUBGHZSPI

#if defined SPI$_USE_CLK_LOW_1EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
#elif defined SPI$_USE_CLK_LOW_2EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
#elif defined SPI$_USE_CLK_HIGH_1EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
#elif defined SPI$_USE_CLK_HIGH_2EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
#else
  #error SPI$ clk polarity and edge not specified!
#endif

#if defined SPI$_USE_CLOCKSPEED_36MHZ
  SPI_InitStruct.BaudRate = _spi$_baudrate(SPI_36MHZ);
#elif defined SPI$_USE_CLOCKSPEED_18MHZ
  SPI_InitStruct.BaudRate = _spi$_baudrate(SPI_18MHZ);
#elif defined SPI$_USE_CLOCKSPEED_9MHZ
  SPI_InitStruct.BaudRate = _spi$_baudrate(SPI_9MHZ);
#elif defined SPI$_USE_CLOCKSPEED_4500KHZ
  SPI_InitStruct.BaudRate = _spi$_baudrate(SPI_4p5MHZ);
#elif defined SPI$_USE_CLOCKSPEED_2250KHZ
  SPI_InitStruct.BaudRate = _spi$_baudrate(SPI_2p25MHZ);
#elif defined SPI$_USE_CLOCKSPEED_1125KHZ
  SPI_InitStruct.BaudRate = _spi$_baudrate(SPI_1p125MHZ);
#elif defined SPI$_USE_CLOCKSPEED_562KHZ
  SPI_InitStruct.BaudRate = _spi$_baudrate(SPI_562p5KHZ);
#else
  #warning SPI$: no clockspeed defined, 280 kHz selected!
  SPI_InitStruct.BaudRate = _spi$_baudrate(SPI_281p25KHZ);
#endif

#else // SUBGHZSPI
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
#if defined SPI$_USE_CLOCKSPEED_24MHZ
  SPI_InitStruct.BaudRate = SUBGHZSPI_BAUDRATEPRESCALER_2;
#elif defined SPI$_USE_CLOCKSPEED_12MHZ
  SPI_InitStruct.BaudRate = SUBGHZSPI_BAUDRATEPRESCALER_4;
#elif defined SPI$_USE_CLOCKSPEED_6MHZ
  SPI_InitStruct.BaudRate = SUBGHZSPI_BAUDRATEPRESCALER_8;
#else
  #warning SPI$: no clockspeed defined, 6 MHz selected!
  SPI_InitStruct.BaudRate = SUBGHZSPI_BAUDRATEPRESCALER_8;
#endif
#endif

  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI$_SPIx, &SPI_InitStruct);

#if defined STM32F7 || defined STM32G4 || defined STM32F3 || defined STM32WL || defined STM32F0 //TODO is this correct for F3 ???
  LL_SPI_SetStandard(SPI$_SPIx, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_DisableNSSPulseMgt(SPI$_SPIx);
#endif

  // Enable SPIx
  LL_SPI_Enable(SPI$_SPIx);

  // Empty SPIx
  while (!LL_SPI_IsActiveFlag_TXE(SPI$_SPIx)) {}
  (void)LL_SPI_ReceiveData8(SPI$_SPIx);
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_SPI$_H
