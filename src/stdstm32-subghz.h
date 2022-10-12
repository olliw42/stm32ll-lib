//*******************************************************
// Copyright (c) OlliW, OlliW42, www.olliw.eu
// GPL3
// https://www.gnu.org/licenses/gpl-3.0.de.html
//*******************************************************
// SUBGHZ standard library
// defines some helpers for use with STM32's SUBGHZ peripheral
// the SPI functionality is implemented in the stdstm32-spi libraries
//*******************************************************
#ifndef STDSTM32_SUBGHZ_H
#define STDSTM32_SUBGHZ_H
#ifdef __cplusplus
extern "C" {
#endif


//-------------------------------------------------------
// SUBGHZ specific helpers
//-------------------------------------------------------

// from stm32wlexx_hal_subghz.c
//  void SUBGHZSPI_Init(uint32_t BaudratePrescaler) does essentially exactly what
//    LL_SPI_Init(SPI_SPIx, &SPI_InitStruct);
//    LL_SPI_Enable(SPI_SPIx);
//  does. So we can use the default spi code with just the adaption for
//    ClockPolarity, ClockPhase, and BaudRate
//  The subghz_init() does what
//    HAL_StatusTypeDef HAL_SUBGHZ_Init(SUBGHZ_HandleTypeDef *hsubghz)
//  does. With some few additions to more closely follow datasheet.


// from stm32wlxx_hal_subghz.h
#define SUBGHZSPI_BAUDRATEPRESCALER_2       (0x00000000U)
#define SUBGHZSPI_BAUDRATEPRESCALER_4       (SPI_CR1_BR_0)
#define SUBGHZSPI_BAUDRATEPRESCALER_8       (SPI_CR1_BR_1)
#define SUBGHZSPI_BAUDRATEPRESCALER_16      (SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define SUBGHZSPI_BAUDRATEPRESCALER_32      (SPI_CR1_BR_2)
#define SUBGHZSPI_BAUDRATEPRESCALER_64      (SPI_CR1_BR_2 | SPI_CR1_BR_0)
#define SUBGHZSPI_BAUDRATEPRESCALER_128     (SPI_CR1_BR_2 | SPI_CR1_BR_1)
#define SUBGHZSPI_BAUDRATEPRESCALER_256     (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)


uint8_t subghz_is_busy(void)
{
  uint32_t mask = LL_PWR_IsActiveFlag_RFBUSYMS();
  return ((LL_PWR_IsActiveFlag_RFBUSYS() & mask) == 1) ? 1 : 0;
}


void subghz_waitonbusy(void)
{
/*
  // that's what HAL_StatusTypeDef SUBGHZ_WaitOnBusy(SUBGHZ_HandleTypeDef *hsubghz) does:
  uint32_t mask;
  do {
    mask = LL_PWR_IsActiveFlag_RFBUSYMS();
  } while ((LL_PWR_IsActiveFlag_RFBUSYS() & mask) == 1UL);
*/
  while (LL_PWR_IsActiveFlag_RFBUSYS()) {};
}


// Follows HAL_StatusTypeDef HAL_SUBGHZ_Init(SUBGHZ_HandleTypeDef *hsubghz),
// with some few additions to better follow the datasheet.
void subghz_init()
{
  // we do this additionally
  LL_RCC_RF_EnableReset();
  for (uint16_t i = 0; i < 1000; i++) { __NOP(); }

  // disable radio reset signal
  LL_RCC_RF_DisableReset();

  // wait for radio reset signal to have disabled
  while (LL_RCC_IsRFUnderReset()) {};

  // we do this in addition, datasheet says there should be a falling edge of >20 us for entering standby
  LL_PWR_SelectSUBGHZSPI_NSS();
  for (uint16_t i = 0; i < 1000; i++) { __NOP(); }

  // set radio SPI NSS to logical level high
  LL_PWR_UnselectSUBGHZSPI_NSS();

  // clear radio busy flag
  LL_PWR_ClearFlag_RFBUSY();

  // Enable EXTI 44 : Radio IRQ ITs for CPU1
  // no, we don't like this // LL_EXTI_EnableIT_32_63(LL_EXTI_LINE_44);

  // Enable wakeup signal of the Radio peripheral */
  // no, we don't like this // LL_PWR_SetRadioBusyTrigger(LL_PWR_RADIO_BUSY_TRIGGER_WU_IT);
}


//-------------------------------------------------------
#ifdef __cplusplus
}
#endif
#endif // STDSTM32_SUBGHZ_H
