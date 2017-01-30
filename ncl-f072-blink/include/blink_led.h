//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#ifndef BLINKLED_H_
#define BLINKLED_H_

#include "stm32f0xx.h"

#define BLINK_GPIOx(_N)                 ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BLINK_PIN_MASK(_N)              (1 << (_N))
#define BLINK_RCC_MASKx(_N)             (RCC_AHBPeriph_GPIOA << (_N))

// ----------------------------------------------------------------------------

class blink_led
{
public:

  blink_led (unsigned int port, unsigned int bit, bool active_low);

  void
  power_up ();

  void
  turn_on ();

  void
  turn_off ();

  void
  toggle ();

  bool
  is_on ();

private:
  GPIO_TypeDef *portAddress;
  unsigned int portNumber;
  unsigned int bitNumber;
  unsigned int bitMask;
  bool isActiveLow;

};

// ----------------------------------------------------------------------------

#endif // BLINKLED_H_
