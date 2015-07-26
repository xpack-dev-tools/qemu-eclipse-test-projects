//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#include "BlinkLed.h"

// ----------------------------------------------------------------------------

BlinkLed::BlinkLed (int port, int bit, bool active_low)
{
  port_number = port;
  bit_number = bit;
  is_active_low = active_low;
}

void
BlinkLed::powerUp ()
{
  // Enable GPIO Peripheral clock
  RCC->AHB1ENR |= BLINK_RCC_MASKx(port_number);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.Pin = BLINK_PIN_MASK(bit_number);
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init (BLINK_GPIOx(port_number), &GPIO_InitStructure);

  // Start with led turned off
  turnOff ();
}

void
BlinkLed::turnOn ()
{
  if (is_active_low)
    {
      HAL_GPIO_WritePin (BLINK_GPIOx(port_number), BLINK_PIN_MASK(bit_number),
			 GPIO_PIN_RESET);
    }
  else
    {
      HAL_GPIO_WritePin (BLINK_GPIOx(port_number), BLINK_PIN_MASK(bit_number),
			 GPIO_PIN_SET);
    }
}

void
BlinkLed::turnOff ()
{
  if (is_active_low)
    {
      HAL_GPIO_WritePin (BLINK_GPIOx(port_number), BLINK_PIN_MASK(bit_number),
			 GPIO_PIN_SET);
    }
  else
    {
      HAL_GPIO_WritePin (BLINK_GPIOx(port_number), BLINK_PIN_MASK(bit_number),
			 GPIO_PIN_RESET);
    }
}

// ----------------------------------------------------------------------------
