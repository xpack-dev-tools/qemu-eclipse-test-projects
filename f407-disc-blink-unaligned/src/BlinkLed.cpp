//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#include "BlinkLed.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

typedef struct
{
  __IO uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
  __IO uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
  __IO uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
  __IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
  __IO uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
  __IO uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
  __IO uint16_t BSRRL;     /*!< GPIO port bit set register,      Address offset: 0x16      */
  __IO uint16_t BSRRH;     /*!< GPIO port bit reset register,      Address offset: 0x18      */
  __IO uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
  __IO uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
} GPIO_TypeDef2;

#define BLINK_GPIOx(_N)       ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BLINK_GPIOx2(_N)       ((GPIO_TypeDef2 *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BLINK_PIN_MASK(_N)    (1 << (_N))
#define BLINK_RCC_MASKx(_N)   (RCC_AHB1ENR_GPIOAEN << (_N))

// ----------------------------------------------------------------------------

BlinkLed::BlinkLed (unsigned int port, unsigned int bit, bool activeLow)
{
  fPortNumber = port;
  fBitNumber = bit;
  fIsActiveLow = activeLow;
  fBitMask = BLINK_PIN_MASK(fBitNumber);
}

void
BlinkLed::powerUp ()
{
  // Enable GPIO Peripheral clock
  RCC->AHB1ENR |= BLINK_RCC_MASKx(fPortNumber);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.Pin = fBitMask;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init (BLINK_GPIOx(fPortNumber), &GPIO_InitStructure);

  // Start with led turned off
  turnOff ();
}

void
BlinkLed::turnOn ()
{
  if (fIsActiveLow)
    {
      BLINK_GPIOx2(fPortNumber)->BSRRH = fBitMask;
    }
  else
    {
      BLINK_GPIOx2(fPortNumber)->BSRRL = fBitMask;
    }
}

void
BlinkLed::turnOff ()
{
  if (fIsActiveLow)
    {
      BLINK_GPIOx2(fPortNumber)->BSRRL = fBitMask;
    }
  else
    {
      BLINK_GPIOx2(fPortNumber)->BSRRH = fBitMask;
    }
}

void
BlinkLed::toggle ()
{
  if (BLINK_GPIOx(fPortNumber)->IDR & fBitMask)
    {
      BLINK_GPIOx(fPortNumber)->ODR &= ~fBitMask;
    }
  else
    {
      BLINK_GPIOx(fPortNumber)->ODR |= fBitMask;
    }
}

bool
BlinkLed::isOn ()
{
  if (fIsActiveLow)
    {
      return (BLINK_GPIOx(fPortNumber)->IDR & fBitMask) == 0;
    }
  else
    {
      return (BLINK_GPIOx(fPortNumber)->IDR & fBitMask) != 0;
    }
}

// ----------------------------------------------------------------------------
