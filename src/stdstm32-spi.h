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
// #define SPI_USE_SPI1, SPI_USE_SPI2, SPI_USE_SPI3, SPI_USE_SUBGHZSPI
// #define SPI_USE_DMAMODE
// #define SPI_DMAMODE_PRIORITY
//
// #define SPI_CS_IO
//
// #define SPI_USE_CLK_LOW_1EDGE
// #define SPI_USE_CLK_LOW_2EDGE
// #define SPI_USE_CLK_HIGH_1EDGE
// #define SPI_USE_CLK_HIGH_2EDGE
//
// #defined SPI_USE_CLOCKSPEED_36MHZ
// #defined SPI_USE_CLOCKSPEED_18MHZ
// #defined SPI_USE_CLOCKSPEED_9MHZ
// #defined SPI_USE_CLOCKSPEED_4500KHZ
// #defined SPI_USE_CLOCKSPEED_2250KHZ
// #defined SPI_USE_CLOCKSPEED_1125KHZ
// #defined SPI_USE_CLOCKSPEED_562KHZ
// #defined SPI_USE_CLOCKSPEED_281KHZ
//
// #defined SPI_USE_DATASIZEWORD // uses word transfer instead of byte transfer
//
//*******************************************************
#ifndef STDSTM32_SPI_H
#define STDSTM32_SPI_H
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
#if defined SPI_USE_SUBGHZSPI && defined STM32WL
#include "stdstm32-subghz.h"
#endif

#ifdef SPI_USE_SPI1
  #define SPI_SPIx                SPI1
  #if defined STM32F3
    #define SPI_SCK_IO            IO_PB3
    #define SPI_MISO_IO           IO_PB4
    #define SPI_MOSI_IO           IO_PB5
  #else
    #define SPI_SCK_IO            IO_PA5
    #define SPI_MISO_IO           IO_PA6
    #define SPI_MOSI_IO           IO_PA7
  #endif
  #define SPI_IO_AF               IO_AF_5
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32WL
  #endif
#endif
#ifdef SPI_USE_SPI2
  #define SPI_SPIx                SPI2
  #define SPI_SCK_IO              IO_PB13 // IO_PB10 ?
  #define SPI_MISO_IO             IO_PB14 // IO_PC1 ?
  #define SPI_MOSI_IO             IO_PB15 // IO_PC2 ?
  #define SPI_IO_AF               IO_AF_5
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32WL
  #endif
#endif
#ifdef SPI_USE_SPI3
  #define SPI_SPIx                SPI3
  #define SPI_SCK_IO              IO_PB3
  #define SPI_MISO_IO             IO_PB4
  #define SPI_MOSI_IO             IO_PB5
  #define SPI_IO_AF               IO_AF_5
  #if defined STM32F1
  #elif defined STM32F3
  #elif defined STM32F7
  #elif defined STM32G4
  #elif defined STM32WL
    #error SPI3 NOT AVAILABLE !
  #endif
#endif
#ifdef SPI_USE_SUBGHZSPI
  #define SPI_SPIx                SUBGHZSPI
  #if !defined STM32WL
    #error SUBGHZSPI NOT AVAILABLE !
  #endif
#endif
#ifndef SPI_SPIx
  #error No SPI defined !
#endif


// allows us to overwrite the default pin assignment
#ifdef SPI_USE_SCK_IO
  #undef SPI_SCK_IO
  #define SPI_SCK_IO              SPI_USE_SCK_IO
#endif
#ifdef SPI_USE_MISO_IO
  #undef SPI_MISO_IO
  #define SPI_MISO_IO             SPI_USE_MISO_IO
#endif
#ifdef SPI_USE_MOSI_IO
  #undef SPI_MOSI_IO
  #define SPI_MOSI_IO             SPI_USE_MOSI_IO
#endif
#ifdef SPI_USE_IO_AF
  #undef SPI_IO_AF
  #define SPI_IO_AF               SPI_USE_IO_AF
#endif


#ifndef SPI_SELECT_PRE_DELAY
  #define SPI_SELECT_PRE_DELAY
#endif
#ifndef SPI_SELECT_POST_DELAY
  #define SPI_SELECT_POST_DELAY
#endif
#ifndef SPI_DESELECT_PRE_DELAY
  #define SPI_DESELECT_PRE_DELAY
#endif
#ifndef SPI_DESELECT_POST_DELAY
  #define SPI_DESELECT_POST_DELAY
#endif


//-------------------------------------------------------
//  SPI low level routines
//-------------------------------------------------------

void spi_disable(void)
{
  // according to reference manual
  while (!LL_SPI_IsActiveFlag_RXNE(SPI_SPIx)) {};
  while (!LL_SPI_IsActiveFlag_TXE(SPI_SPIx)) {};
  while (LL_SPI_IsActiveFlag_BSY(SPI_SPIx)) {};
  LL_SPI_Disable(SPI_SPIx);
}


#ifdef SPI_CS_IO

static inline void spi_select(void)
{
  SPI_SELECT_PRE_DELAY;
  gpio_low(SPI_CS_IO); // CS = low
  SPI_SELECT_POST_DELAY;
}


static inline void spi_deselect(void)
{
  SPI_DESELECT_PRE_DELAY;
  gpio_high(SPI_CS_IO); // CS = high
  SPI_DESELECT_POST_DELAY;
}

#elif defined SPI_USE_SUBGHZSPI
    
static inline void spi_select(void)
{
  SPI_SELECT_PRE_DELAY;
  LL_PWR_SelectSUBGHZSPI_NSS();
  SPI_SELECT_POST_DELAY;
}


static inline void spi_deselect(void)
{
  SPI_DESELECT_PRE_DELAY;
  LL_PWR_UnselectSUBGHZSPI_NSS();
  SPI_DESELECT_POST_DELAY;
}

#endif


// is blocking
uint8_t spi_transmitchar(uint8_t c)
{
#ifdef SPI_USE_SUBGHZSPI
  while (!LL_SPI_IsActiveFlag_TXE(SPI_SPIx)) {}; // we don't do that originally, but it' suggested by cubemx    
#endif  
  LL_SPI_TransmitData8(SPI_SPIx, c);
  while (!LL_SPI_IsActiveFlag_RXNE(SPI_SPIx)) {};
  return LL_SPI_ReceiveData8(SPI_SPIx);
}


// is blocking
void spi_transfer(uint8_t* dataout, uint8_t* datain, uint16_t len)
{
  while (len) {
    *datain = spi_transmitchar(*dataout);
    dataout++;
    datain++;
    len--;
  }
}


// sends a dummy char, returns the received byte
// is blocking
static inline uint8_t spi_readchar(void)
{
  return spi_transmitchar(0xFF);
}


// sends a char, ignores the received byte
// is blocking
static inline void spi_writechar(uint8_t c)
{
  spi_transmitchar(c);
}


// sends a word, returns the received word
// is blocking
static inline uint16_t spi_transmitword(uint16_t w)
{
  return (((uint16_t)spi_transmitchar((uint8_t)(w >> 8))) << 8) + spi_transmitchar((uint8_t)(w));
}


// sends a dummy word, returns the received word
// is blocking
static inline uint16_t spi_readword(void)
{
  return (((uint16_t)spi_transmitchar(0xFF)) << 8) + spi_transmitchar(0xFF);
}


// sends a word, ignores the received word
// is blocking
static inline void spi_writeword(uint16_t w)
{
  spi_transmitchar((uint8_t)(w >> 8));
  spi_transmitchar((uint8_t)(w));
}


// is blocking
void spi_read(uint8_t* data, uint16_t len)
{
  while (len) {
    *data = spi_readchar();
    data++;
    len--;
  }
}


// is blocking
void spi_write(uint8_t* data, uint16_t len)
{
  while (len) {
    spi_writechar(*data);
    data++;
    len--;
  }
}

// is blocking
void spi_writecandread(uint8_t c, uint8_t* data, uint16_t datalen)
{
  spi_writechar(c);
  while (datalen) {
    *data = spi_readchar();
    data++;
    datalen--;
  }
}


//-------------------------------------------------------
// INIT routines
//-------------------------------------------------------

#if !defined SPI_USE_SUBGHZSPI
#ifndef SPI_BAUDRATE_FUNC
#define SPI_BAUDRATE_FUNC

uint32_t _spi_baudrate(SPICLOCKSPEEDENUM speed)
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
  
#elif defined STM32WL // all SPI are on 48 MHz
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

#endif // !SPI_BAUDRATE_FUNC


// datasheet: this bit should not be changed when communication is ongoing
void spi_setmode(SPIMODEENUM mode)
{
  LL_SPI_Disable(SPI_SPIx); // is important to do that

  switch (mode) {
    case SPI_MODE_LOW_1EDGE:
      LL_SPI_SetClockPhase(SPI_SPIx, LL_SPI_PHASE_1EDGE);
      LL_SPI_SetClockPolarity(SPI_SPIx, LL_SPI_POLARITY_LOW);
      break;
    case SPI_MODE_LOW_2EDGE:
      LL_SPI_SetClockPhase(SPI_SPIx, LL_SPI_PHASE_2EDGE);
      LL_SPI_SetClockPolarity(SPI_SPIx, LL_SPI_POLARITY_LOW);
      break;
    case SPI_MODE_HIGH_1EDGE:
      LL_SPI_SetClockPhase(SPI_SPIx, LL_SPI_PHASE_1EDGE);
      LL_SPI_SetClockPolarity(SPI_SPIx, LL_SPI_POLARITY_HIGH);
      break;
    case SPI_MODE_HIGH_2EDGE:
      LL_SPI_SetClockPhase(SPI_SPIx, LL_SPI_PHASE_2EDGE);
      LL_SPI_SetClockPolarity(SPI_SPIx, LL_SPI_POLARITY_HIGH);
      break;
    default:
      while (1) {};
  }

  LL_SPI_Enable(SPI_SPIx);
  while (!LL_SPI_IsActiveFlag_TXE(SPI_SPIx)) {}
  (void)LL_SPI_ReceiveData8(SPI_SPIx);
}

#endif // !SPI_USE_SUBGHZSPI


void spi_init(void)
{
LL_SPI_InitTypeDef SPI_InitStruct = {0};

  // Initialize clocks for SPI, AFIO
  rcc_init_afio();
  rcc_init_spi(SPI_SPIx);
#ifdef SPI_USE_SUBGHZSPI
  subghz_init();
#endif

#if defined SPI_USE_SPI3 && defined STM32F1
  LL_GPIO_AF_Remap_SWJ_NOJTAG();
#endif

#ifndef SPI_USE_SUBGHZSPI

  // Configure pin CS
#ifdef SPI_CS_IO
  gpio_init(SPI_CS_IO, IO_MODE_OUTPUT_PP_HIGH, IO_SPEED_VERYFAST);
  spi_deselect(); // deselect SPI: CS high
#endif

  // Configure pin SCK MOSI as alternative function push-pull, pin MISO as input with pull up
  gpio_init_af(SPI_SCK_IO, IO_MODE_OUTPUT_ALTERNATE_PP, SPI_IO_AF, IO_SPEED_VERYFAST);
  gpio_init_af(SPI_MOSI_IO, IO_MODE_OUTPUT_ALTERNATE_PP, SPI_IO_AF, IO_SPEED_VERYFAST);
  gpio_init_af(SPI_MISO_IO, IO_MODE_INPUT_PU, SPI_IO_AF, IO_SPEED_VERYFAST);

#endif // !SPI_USE_SUBGHZSPI

  // Configure SPIx
#ifndef SPI_USE_SUBGHZSPI

#if defined SPI_USE_CLK_LOW_1EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
#elif defined SPI_USE_CLK_LOW_2EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
#elif defined SPI_USE_CLK_HIGH_1EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
#elif defined SPI_USE_CLK_HIGH_2EDGE
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
#else
  #error SPI clk polarity and edge not specified!
#endif

#if defined SPI_USE_CLOCKSPEED_36MHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_36MHZ);
#elif defined SPI_USE_CLOCKSPEED_18MHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_18MHZ);
#elif defined SPI_USE_CLOCKSPEED_9MHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_9MHZ);
#elif defined SPI_USE_CLOCKSPEED_4500KHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_4p5MHZ);
#elif defined SPI_USE_CLOCKSPEED_2250KHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_2p25MHZ);
#elif defined SPI_USE_CLOCKSPEED_1125KHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_1p125MHZ);
#elif defined SPI_USE_CLOCKSPEED_562KHZ
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_562p5KHZ);
#else
  #warning SPI: no clockspeed defined, 280 kHz selected!
  SPI_InitStruct.BaudRate = _spi_baudrate(SPI_281p25KHZ);
#endif

#else
  SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStruct.BaudRate = SUBGHZSPI_BAUDRATEPRESCALER_8;
#endif

  SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
  SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  SPI_InitStruct.CRCPoly = 7;
  LL_SPI_Init(SPI_SPIx, &SPI_InitStruct);

#if defined STM32F7 || defined STM32G4 || defined STM32F3 || defined STM32WL //TODO is this correct for F3 ???
  LL_SPI_SetStandard(SPI_SPIx, LL_SPI_PROTOCOL_MOTOROLA);
  LL_SPI_DisableNSSPulseMgt(SPI_SPIx);
#endif

  // Enable SPIx
  LL_SPI_Enable(SPI_SPIx);

  // Empty SPIx
  while (!LL_SPI_IsActiveFlag_TXE(SPI_SPIx)) {}
  (void)LL_SPI_ReceiveData8(SPI_SPIx);
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_SPI_H
