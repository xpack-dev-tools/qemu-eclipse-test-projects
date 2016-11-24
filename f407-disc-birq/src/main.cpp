//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "Timer.h"
#include "BlinkLed.h"

// ----------------------------------------------------------------------------
//
// Semihosting STM32F4 led blink sample (trace via ITM).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// To demonstrate semihosting, display a message on the standard output
// and another message on the standard error.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// On DEBUG, the uptime in seconds is also displayed on the trace device.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the ITM output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// Definitions visible only within this translation unit.
namespace
{
  // ----- Timing definitions -------------------------------------------------

  // Keep the LED on for 2/3 of a second.
  constexpr Timer::ticks_t BLINK_ON_TICKS = Timer::FREQUENCY_HZ * 3 / 4;
  constexpr Timer::ticks_t BLINK_OFF_TICKS = Timer::FREQUENCY_HZ
      - BLINK_ON_TICKS;
}

// ----- LED definitions ------------------------------------------------------

#if defined(STM32F401xE)

#warning "Assume a NUCLEO-F401RE board, PA5, active high."

// PA5
#define BLINK_PORT_NUMBER         (0)
#define BLINK_PIN_NUMBER          (5)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[1] =
  {
      { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER, BLINK_ACTIVE_LOW},
  };

#elif defined(STM32F407xx)

#warning "Assume a STM32F4-Discovery board, PC12-PC15, active high."

#define BLINK_PORT_NUMBER         (3)
#define BLINK_PIN_NUMBER_GREEN    (12)
#define BLINK_PIN_NUMBER_ORANGE   (13)
#define BLINK_PIN_NUMBER_RED      (14)
#define BLINK_PIN_NUMBER_BLUE     (15)
#define BLINK_ACTIVE_LOW          (false)

#define BUTTON_PORT_NUMBER        (0)
#define BUTTON_PIN_NUMBER         (0)

BlinkLed blinkLeds[4] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_GREEN, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_ORANGE, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_RED, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_BLUE, BLINK_ACTIVE_LOW }, };

#elif defined(STM32F411xE)

#warning "Assume a NUCLEO-F411RE board, PA5, active high."

#define BLINK_PORT_NUMBER         (0)
#define BLINK_PIN_NUMBER          (5)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[1] =
  {
      { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER, BLINK_ACTIVE_LOW},
  };

#elif defined(STM32F429xx)

#warning "Assume a STM32F429I-Discovery board, PG13-PG14, active high."

#define BLINK_PORT_NUMBER         (6)
#define BLINK_PIN_NUMBER_GREEN    (13)
#define BLINK_PIN_NUMBER_RED      (14)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[2] =
  {
      { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_GREEN, BLINK_ACTIVE_LOW},
      { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_RED, BLINK_ACTIVE_LOW},
  };

#else

#warning "Unknown board, assume PA5, active high."

#define BLINK_PORT_NUMBER         (0)
#define BLINK_PIN_NUMBER          (5)
#define BLINK_ACTIVE_LOW          (false)

BlinkLed blinkLeds[1] =
  {
      { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER, BLINK_ACTIVE_LOW},
  };

#endif

// ----- main() ---------------------------------------------------------------

#define BLINK_GPIOx(_N)       ((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BLINK_PIN_MASK(_N)    (1 << (_N))
#define BLINK_RCC_MASKx(_N)   (RCC_AHB1ENR_GPIOAEN << (_N))

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

int
main (int argc, char* argv[])
{
  // Show the program parameters (passed via semihosting).
  // Output is via the semihosting output channel.
  trace_dump_args (argc, argv);

  // Send a greeting to the trace device (skipped on Release).
  trace_puts ("Hello ARM World!");

  // Send a message to the standard output.
  puts ("Standard output message.");

  // Send a message to the standard error.
  fprintf (stderr, "Standard error message.\n");

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf ("System clock: %u Hz\n", SystemCoreClock);

  Timer timer;
  timer.start ();

  // Perform all necessary initialisations for the LEDs.
  for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
    {
      blinkLeds[i].powerUp ();
    }

  for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
    {
      blinkLeds[i].turnOn ();
    }

  timer.sleep (BLINK_ON_TICKS);

  for (size_t i = 0; i < (sizeof(blinkLeds) / sizeof(blinkLeds[0])); ++i)
    {
      blinkLeds[i].turnOff ();
    }

  timer.sleep (BLINK_OFF_TICKS);

  RCC->AHB1ENR |= BLINK_RCC_MASKx(BUTTON_PORT_NUMBER);

  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure pin in output push/pull mode
  GPIO_InitStructure.Pin = BLINK_PIN_MASK(BUTTON_PIN_NUMBER);
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init (BLINK_GPIOx(BUTTON_PORT_NUMBER), &GPIO_InitStructure);

  NVIC_EnableIRQ(EXTI0_IRQn);

  while (1)
    {
      timer.sleep (1000);
    }

  return 0;
}

#pragma GCC diagnostic pop

extern "C" void
EXTI0_IRQHandler(void);

void
EXTI0_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

int led_no = 0;
int old_val = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  int val = ((GPIOA->IDR & BLINK_PIN_MASK(BUTTON_PIN_NUMBER)) != 0);

  if (val != old_val)
	{
	  if (val)
	    {
	      blinkLeds[led_no].turnOn ();
	    }
	  else
	    {
	      blinkLeds[led_no].turnOff ();
#if 1
	      led_no = (led_no + 1)
		  % (sizeof(blinkLeds) / sizeof(blinkLeds[0]));
	      // trace_printf ("Led %d\n", led_no);
#endif
	    }
	}
  old_val = val;
}

// ----------------------------------------------------------------------------
