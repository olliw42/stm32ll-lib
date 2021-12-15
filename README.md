# STM32 LL Library
Library for STM32 chips.

This is a header only library. The UART and SPI libraries do not use handlers like it is often done. This results in more cumbersome-looking code, but is significantly more performant, especially of interrupt routines.
