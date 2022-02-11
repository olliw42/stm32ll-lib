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
// #define SPIB_USE_SPI1, SPIB_USE_SPI2, SPIB_USE_SPI3
// #define SPIB_USE_DMAMODE
// #define SPIB_DMAMODE_PRIORITY
//
// #define SPIB_CS_IO
//
// #define SPIB_USE_CLK_LOW_1EDGE
// #define SPIB_USE_CLK_LOW_2EDGE
// #define SPIB_USE_CLK_HIGH_1EDGE
// #define SPIB_USE_CLK_HIGH_2EDGE
//
// #defined SPIB_USE_CLOCKSPEED_36MHZ
// #defined SPIB_USE_CLOCKSPEED_18MHZ
// #defined SPIB_USE_CLOCKSPEED_9MHZ
// #defined SPIB_USE_CLOCKSPEED_4500KHZ
// #defined SPIB_USE_CLOCKSPEED_2250KHZ
// #defined SPIB_USE_CLOCKSPEED_1125KHZ
// #defined SPIB_USE_CLOCKSPEED_562KHZ
// #defined SPIB_USE_CLOCKSPEED_281KHZ
//
// #defined SPIB_USE_DATASIZEWORD // uses word transfer instead of byte transfer
//
//*******************************************************
#ifndef STDSTM32_SPIB_H
#define STDSTM32_SPIB_H
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
  SPI_MODE_LOW_1EDGE = 0,
  SPI_MODE_LOW_2EDGE,
  SPI_MODE_HIGH_1EDGE,
  SPI_MODE_HIGH_2EDGE,
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

#ifdef SPIB_USE_SPI1
  #define SPIB_SPIx                SPI1
  #if defined STM32F3
    #define SPIB_SCK_IO            IO_PB3
    #define SPIB_MISO_IO           IO_PB4
    #define SPIB_MOSI_IO           IO_PB5
  #else
    #define SPIB_SCK_IO            IO_PA5
    #define SPIB_MISO_IO           IO_PA6
    #define SPIB_MOSI_IO           IO_PA7
  #endif
  #define SPIB_IO_AF               IO_AF_5
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #endif
#endif
#ifdef SPIB_USE_SPI2
  #define SPIB_SPIx                SPI2
  #define SPIB_SCK_IO              IO_PB13 // IO_PB10 ?
  #define SPIB_MISO_IO             IO_PB14 // IO_PC1 ?
  #define SPIB_MOSI_IO             IO_PB15 // IO_PC2 ?
  #define SPIB_IO_AF               IO_AF_5
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #endif
#endif
#ifdef SPIB_USE_SPI3
  #define SPIB_SPIx                SPI3
  #define SPIB_SCK_IO              IO_PB3
  #define SPIB_MISO_IO             IO_PB4
  #define SPIB_MOSI_IO             IO_PB5
  #define SPIB_IO_AF               IO_AF_5
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #endif
#endif
#ifndef SPIB_SPIx
  #error No SPI defined !
#endif


#ifndef SPIB_SELECT_PRE_DELAY
  #define SPIB_SELECT_PRE_DELAY
#endif
#ifndef SPIB_SELECT_POST_DELAY
  #define SPIB_SELECT_POST_DELAY
#endif
#ifndef SPIB_DESELECT_PRE_DELAY
  #define SPIB_DESELECT_PRE_DELAY
#endif
#ifndef SPIB_DESELECT_POST_DELAY
  #define SPIB_DESELECT_POST_DELAY
#endif


//-------------------------------------------------------
//  SPI low level routines
//-------------------------------------------------------

void spib_disable(void)
{
  // according to reference manual
  while (!LL_SPI_IsActiveFlag_RXNE(SPIB_SPIx)) {};
  while (!LL_SPI_IsActiveFlag_TXE(SPIB_SPIx)) {};
  while (LL_SPI_IsActiveFlag_BSY(SPIB_SPIx)) {};
  LL_SPI_Disable(SPIB_SPIx);
}


#ifdef SPIB_CS_IO

static inline void spib_select(void)
{
  SPIB_SELECT_PRE_DELAY;
  gpio_low(SPIB_CS_IO); // CS = low
  SPIB_SELECT_POST_DELAY;
}


static inline void spib_deselect(void)
{
  SPIB_DESELECT_PRE_DELAY;
  gpio_high(SPIB_CS_IO); // CS = high
  SPIB_DESELECT_POST_DELAY;
}

#endif


// is blocking
uint8_t spib_transmitchar(uint8_t c)
{
  LL_SPI_TransmitData8(SPIB_SPIx, c);
  while (!LL_SPI_IsActiveFlag_RXNE(SPIB_SPIx)) {};
  return LL_SPI_ReceiveData8(SPIB_SPIx);
}


// is blocking
void spib_transfer(uint8_t* dataout, uint8_t* datain, uint16_t len)
{
  while (len) {
    *datain = spib_transmitchar(*dataout);
    dataout++;
    datain++;
    len--;
  }
}


// sends a dummy char, returns the received byte
// is blocking
static inline uint8_t spib_readchar(void)
{
  return spib_transmitchar(0xFF);
}


// sends a char, ignores the received byte
// is blocking
static inline void spib_writechar(uint8_t c)
{
  spib_transmitchar(c);
}


// sends a word, returns the received word
// is blocking
static inline uint16_t spib_transmitword(uint16_t w)
{
  return (((uint16_t)spib_transmitchar((uint8_t)(w >> 8))) << 8) + spib_transmitchar((uint8_t)(w));
}


// sends a dummy word, returns the received word
// is blocking
static inline uint16_t spib_readword(void)
{
  return (((uint16_t)spib_transmitchar(0xFF)) << 8) + spib_transmitchar(0xFF);
}


// sends a word, ignores the received word
// is blocking
static inline void spib_writeword(uint16_t w)
{
  spib_transmitchar((uint8_t)(w >> 8));
  spib_transmitchar((uint8_t)(w));
}


// is blocking
void spib_read(uint8_t* data, uint16_t len)
{
  while (len) {
    *data = spib_readchar();
    data++;
    len--;
  }
}


// is blocking
void spib_write(uint8_t* data, uint16_t len)
{
  while (len) {
    spib_writechar(*data);
    data++;
    len--;
  }
}

// is blocking
void spib_writecandread(uint8_t c, uint8_t* data, uint16_t datalen)
{
  spib_writechar(c);
  while (datalen) {
    *data = spib_readchar();
    data++;
    datalen--;
  }
}


//-------------------------------------------------------
// INIT routines
//-------------------------------------------------------

#ifndef SPI_BAUDRATE_FUNC
#define SPI_BAUDRATE_FUNC

uint32_t _spib_baudrate(SPICLOCKSPEEDENUM speed)
{
#if defined STM32F1 // SPI1 is on 72 MHz, SPI2 & SPI3 are on 36 MHz
  switch (speed) {
    // case SPI_36MHZ: not possible !
#ifdef SPI_USE_SPI1
    case SPI_18MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV4;
    case SPI_9MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV8; // 72 MHz / 8 = 9 MHz
    case SPI_4p5MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV16;
    case SPI_2p25MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV32;
    case SPI_1p125MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV64;
    case SPI_562p5KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV128;
    case SPI_281p25KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV256;
#else
#warning ATTENTION For F1 SPI2 & SPI3: Baudrate has changed !!!!! Is now only half of it !!!!
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
#ifdef SPI_USE_SPI1
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
    case SPI_36MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV4; // 42.5 MHz
    case SPI_18MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV8;
    case SPI_9MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV16; // 10.625 Mbit/s
    case SPI_4p5MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV32;
    case SPI_2p25MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV64;
    case SPI_1p125MHZ: return LL_SPI_BAUDRATEPRESCALER_DIV128;
    case SPI_562p5KHZ: return LL_SPI_BAUDRATEPRESCALER_DIV256;
    // case SPI_281p25KHZ: not possible !
    default:
      return LL_SPI_BAUDRATEPRESCALER_DIV256;
  }
#endif
}

#endif


// datasheet: this bit should not be changed when communication is ongoing
void spib_setmode(SPIMODEENUM mode)
{
volatile uint16_t dummy;

  LL_SPI_Disable(SPIB_SPIx); // is important to do that

  switch (mode) {
    case SPI_MODE_LOW_1EDGE:
      LL_SPI_SetClockPhase(SPIB_SPIx, LL_SPI_PHASE_1EDGE);
      LL_SPI_SetClockPolarity(SPIB_SPIx, LL_SPI_POLARITY_LOW);
      break;
    case SPI_MODE_LOW_2EDGE:
      LL_SPI_SetClockPhase(SPIB_SPIx, LL_SPI_PHASE_2EDGE);
      LL_SPI_SetClockPolarity(SPIB_SPIx, LL_SPI_POLARITY_LOW);
      break;
    case SPI_MODE_HIGH_1EDGE:
      LL_SPI_SetClockPhase(SPIB_SPIx, LL_SPI_PHASE_1EDGE);
      LL_SPI_SetClockPolarity(SPIB_SPIx, LL_SPI_POLARITY_HIGH);
      break;
    case SPI_MODE_HIGH_2EDGE:
      LL_SPI_SetClockPhase(SPIB_SPIx, LL_SPI_PHASE_2EDGE);
      LL_SPI_SetClockPolarity(SPIB_SPIx, LL_SPI_POLARITY_HIGH);
      break;
    default:
      while (1) {};
  }

  LL_SPI_Enable(SPIB_SPIx);
  while (!LL_SPI_IsActiveFlag_TXE(SPIB_SPIx)) {}
  dummy = LL_SPI_ReceiveData8(SPIB_SPIx);
  dummy = dummy; // to make the compiler happy
}


void spib_init(void)
{
LL_SPI_InitTypeDef SPI_InitStruct = {0};
volatile uint16_t dummy;

  // Initialize clocks for SPI, AFIO
  rcc_init_afio();
  rcc_init_spi(SPIB_SPIx);

#if defined SPIB_USE_SPI3 && defined STM32F1
  LL_GPIO_AF_Remap_SWJ_NOJTAG();
#endif

  // Configure pin CS
#ifdef SPIB_CS_IO
  gpio_init(SPIB_CS_IO, IO_MODE_OUTPUT_PP_HIGH, IO_SPEED_VERYFAST);
  spi_deselect(); // deselect SPI: CS high
#endif

  // Configure pin SCK MOSI as alternative function push-pull, pin MISO as input with pull up
  gpio_init_af(SPIB_SCK_IO, IO_MODE_OUTPUT_ALTERNATE_PP, SPIB_IO_AF, IO_SPEED_VERYFAST);
  gpio_init_af(SPIB_MOSI_IO, IO_MODE_OUTPUT_ALTERNATE_PP, SPIB_IO_AF, IO_SPEED_VERYFAST);
  gpio_init_af(SPIB_MISO_IO, IO_MODE_INPUT_PU, SPIB_IO_AF, IO_SPEED_VERYFAST);

  // Configure SPIx
  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;

  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;

#if defined SPIB_USE_CLK_LOW_1EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
#elif defined SPIB_USE_CLK_LOW_2EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
#elif defined SPIB_USE_CLK_HIGH_1EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
#elif defined SPIB_USE_CLK_HIGH_2EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
#else
  #error SPIB clk polarity and edge not specified!
#endif

  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;

#if defined SPIB_USE_CLOCKSPEED_36MHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_36MHZ);
#elif defined SPIB_USE_CLOCKSPEED_18MHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_18MHZ);
#elif defined SPIB_USE_CLOCKSPEED_9MHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_9MHZ);
#elif defined SPIB_USE_CLOCKSPEED_4500KHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_4p5MHZ);
#elif defined SPIB_USE_CLOCKSPEED_2250KHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_2p25MHZ);
#elif defined SPIB_USE_CLOCKSPEED_1125KHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_1p125MHZ);
#elif defined SPIB_USE_CLOCKSPEED_562KHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_562p5KHZ);
#else
  #warning SPIB: no clockspeed defined, 280 kHz selected!
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_281p25KHZ);
#endif

  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPIB_SPIx, &SPI_InitStruct);

#if defined STM32F7 || defined STM32G4 || defined STM32F3 //TODO is this correct for F3 ???
  LL_SPI_SetStandard(SPIB_SPIx, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_DisableNSSPulseMgt(SPIB_SPIx);
#endif

  // Enable SPIx
  LL_SPI_Enable(SPIB_SPIx);

  // Empty SPIx
  while (!LL_SPI_IsActiveFlag_TXE(SPIB_SPIx)) {}
  dummy = LL_SPI_ReceiveData8(SPIB_SPIx);
  dummy = dummy; // to make the compiler happy
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_SPIB_H
