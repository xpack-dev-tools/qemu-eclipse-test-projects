//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#include <blink_led.h>

// ----------------------------------------------------------------------------

blink_led::blink_led (unsigned int port, unsigned int bit, bool activeLow)
{
  portAddress = BLINK_GPIOx(port);
  portNumber = port;
  bitNumber = bit;
  isActiveLow = activeLow;
  bitMask = BLINK_PIN_MASK(bitNumber);
}

void
blink_led::power_up ()
{
  // Enable GPIO Peripheral clock
  RCC->AHB1ENR |= BLINK_RCC_MASKx(portNumber);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.Pin = bitMask;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init (portAddress, &GPIO_InitStructure);

  // Start with led turned off
  turn_off ();
}

void
blink_led::turn_on ()
{
  if (isActiveLow)
    {
      HAL_GPIO_WritePin (portAddress, bitMask, GPIO_PIN_RESET);
    }
  else
    {
      HAL_GPIO_WritePin (portAddress, bitMask, GPIO_PIN_SET);
    }
}

void
blink_led::turn_off ()
{
  if (isActiveLow)
    {
      HAL_GPIO_WritePin (portAddress, bitMask, GPIO_PIN_SET);
    }
  else
    {
      HAL_GPIO_WritePin (portAddress, bitMask, GPIO_PIN_RESET);
    }
}

void
blink_led::toggle ()
{
  HAL_GPIO_TogglePin (portAddress, bitMask);
}

bool
blink_led::is_on ()
{
  if (isActiveLow)
    {
      return (portAddress->IDR & bitMask) == 0;
    }
  else
    {
      return (portAddress->IDR & bitMask) != 0;
    }
}

// ----------------------------------------------------------------------------
