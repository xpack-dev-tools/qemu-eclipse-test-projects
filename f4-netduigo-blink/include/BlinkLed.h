//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

#ifndef BLINKLED_H_
#define BLINKLED_H_

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#define BLINK_GPIOx(_N)                 ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BLINK_PIN_MASK(_N)              (1 << (_N))
#define BLINK_RCC_MASKx(_N)             (RCC_AHB1ENR_GPIOAEN << (_N))

// ----------------------------------------------------------------------------

class BlinkLed
{
public:
  BlinkLed (int port, int bit, bool active_low);

  void
  powerUp ();

  void
  turnOn ();

  void
  turnOff ();

private:
  int port_number;
  int bit_number;
  bool is_active_low;
};

// ----------------------------------------------------------------------------

#endif // BLINKLED_H_
