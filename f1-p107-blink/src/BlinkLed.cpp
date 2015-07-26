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
  RCC_APB2PeriphClockCmd (BLINK_RCC_MASKx(port_number), ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.GPIO_Pin = BLINK_PIN_MASK(bit_number);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init (BLINK_GPIOx(port_number), &GPIO_InitStructure);

  // Start with led turned off
  turnOff ();
}

void
BlinkLed::turnOn ()
{
  if (is_active_low)
    {
      GPIO_ResetBits (BLINK_GPIOx(port_number), BLINK_PIN_MASK(bit_number));
    }
  else
    {
      GPIO_SetBits (BLINK_GPIOx(port_number), BLINK_PIN_MASK(bit_number));
    }
}

void
BlinkLed::turnOff ()
{
  if (is_active_low)
    {
      GPIO_SetBits (BLINK_GPIOx(port_number), BLINK_PIN_MASK(bit_number));
    }
  else
    {
      GPIO_ResetBits (BLINK_GPIOx(port_number), BLINK_PIN_MASK(bit_number));
    }
}

// ----------------------------------------------------------------------------
