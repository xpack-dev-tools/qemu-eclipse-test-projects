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
  RCC_APB2PeriphClockCmd (BLINK_RCC_MASKx(portNumber), ENABLE);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.GPIO_Pin = bitMask;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init (portAddress, &GPIO_InitStructure);

  // Start with led turned off
  turn_off ();
}

void
blink_led::turn_on ()
{
  if (isActiveLow)
    {
      GPIO_ResetBits (portAddress, bitMask);
    }
  else
    {
      GPIO_SetBits (portAddress, bitMask);
    }
}

void
blink_led::turn_off ()
{
  if (isActiveLow)
    {
      GPIO_SetBits (portAddress, bitMask);
    }
  else
    {
      GPIO_ResetBits (portAddress, bitMask);
    }
}

void
blink_led::toggle ()
{
  if (GPIO_ReadInputData (portAddress) & bitMask)
    {
      GPIO_ResetBits (portAddress, bitMask);
    }
  else
    {
      GPIO_SetBits (portAddress, bitMask);
    }
}

bool
blink_led::is_on ()
{
  if (isActiveLow)
    {
      return (GPIO_ReadInputData (portAddress) & bitMask) == 0;
    }
  else
    {
      return (GPIO_ReadInputData (portAddress) & bitMask) != 0;
    }
}

// ----------------------------------------------------------------------------
