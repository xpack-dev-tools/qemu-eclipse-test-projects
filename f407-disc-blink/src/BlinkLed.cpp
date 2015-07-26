//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#include "BlinkLed.h"

// ----------------------------------------------------------------------------

BlinkLed::BlinkLed (unsigned int port, unsigned int bit, bool activeLow)
{
  portNumber = port;
  bitNumber = bit;
  isActiveLow = activeLow;
  bitMask = BLINK_PIN_MASK(bitNumber);
}

void
BlinkLed::powerUp ()
{
  // Enable GPIO Peripheral clock
  RCC->AHB1ENR |= BLINK_RCC_MASKx(portNumber);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.Pin = bitMask;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init (BLINK_GPIOx(portNumber), &GPIO_InitStructure);

  // Start with led turned off
  turnOff ();
}

void
BlinkLed::turnOn ()
{
  if (isActiveLow)
    {
      BLINK_GPIOx(portNumber)->BSRR = BLINK_PIN_MASK(bitNumber + 16);
    }
  else
    {
      BLINK_GPIOx(portNumber)->BSRR = bitMask;
    }
}

void
BlinkLed::turnOff ()
{
  if (isActiveLow)
    {
      BLINK_GPIOx(portNumber)->BSRR = bitMask;
    }
  else
    {
      BLINK_GPIOx(portNumber)->BSRR = BLINK_PIN_MASK(bitNumber + 16);
    }
}

void
BlinkLed::toggle ()
{
  if (BLINK_GPIOx(portNumber)->IDR & bitMask)
    {
      BLINK_GPIOx(portNumber)->ODR &= ~bitMask;
    }
  else
    {
      BLINK_GPIOx(portNumber)->ODR |= bitMask;
    }
}

bool
BlinkLed::isOn ()
{
  if (isActiveLow)
    {
      return (BLINK_GPIOx(portNumber)->IDR & bitMask) == 0;
    }
  else
    {
      return (BLINK_GPIOx(portNumber)->IDR & bitMask) != 0;
    }
}

// ----------------------------------------------------------------------------
