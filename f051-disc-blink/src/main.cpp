//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <blink_led.h>
#include <stdio.h>
#include <stdlib.h>
#include <timer_systick.h>
#include "diag/Trace.h"

// ----------------------------------------------------------------------------
//
// Semihosting STM32F0 led blink sample (trace via SH).
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
// The external clock frequency is specified as a preprocessor definition
// passed to the compiler via a command line option (see the 'C/C++ General' ->
// 'Paths and Symbols' -> the 'Symbols' tab, if you want to change it).
// The value selected during project creation was HSE_VALUE=8000000.
//
// Note: The default clock settings take the user defined HSE_VALUE and try
// to reach the maximum possible system clock. For the default 8 MHz input
// the result is guaranteed, but for other values it might not be possible,
// so please adjust the PLL settings in system/src/cmsis/system_stm32f0xx.c
//

// Definitions visible only within this translation unit.
namespace
{
  // ----- Timing definitions -------------------------------------------------

  // Keep the LED on for 2/3 of a second.
  constexpr timer_systick::ticks_t BLINK_ON_TICKS = timer_systick::FREQUENCY_HZ
      * 3 / 4;
  constexpr timer_systick::ticks_t BLINK_OFF_TICKS = timer_systick::FREQUENCY_HZ
      - BLINK_ON_TICKS;
}

// ----- LED definitions ------------------------------------------------------

// STM32F4DISCOVERY definitions

// C8 blue, C9 green, active high
#define BLINK_PORT_NUMBER               (2)
#define BLINK_PIN_NUMBER_GREEN          (9)
#define BLINK_PIN_NUMBER_BLUE		(8)
#define BLINK_ACTIVE_LOW                (false)

blink_led blink_leds[2] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_GREEN, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_BLUE, BLINK_ACTIVE_LOW },
  /**/
  };

#define BUTTON_PORT_NUMBER 		(0)
#define BUTTON_PIN_NUMBER 		(0)

#define BUTTON_GPIOx(_N)           	((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BUTTON_PIN_MASK(_N)             (1 << (_N))
#define BUTTON_RCC_MASKx(_N)         	(RCC_AHBPeriph_GPIOA << (_N))

int button_pressed = 0;

// ----- main() ---------------------------------------------------------------

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

  timer_systick timer;
  timer.start ();

  // Perform all necessary initialisations for the LEDs.
  for (size_t i = 0; i < (sizeof(blink_leds) / sizeof(blink_leds[0])); ++i)
    {
      blink_leds[i].power_up ();
    }

  uint32_t seconds = 0;

#define LOOP_COUNT (1 << (sizeof(blink_leds) / sizeof(blink_leds[0])))

  int loops = LOOP_COUNT;
  if (argc > 1)
    {
      // If defined, get the number of loops from the command line,
      // configurable via semihosting.
      loops = atoi (argv[1]);
      if (loops < LOOP_COUNT)
	{
	  loops = LOOP_COUNT;
	}
    }

  for (size_t i = 0; i < (sizeof(blink_leds) / sizeof(blink_leds[0])); ++i)
    {
      blink_leds[i].turn_on ();
    }

  timer.sleep (BLINK_ON_TICKS);

  for (size_t i = 0; i < (sizeof(blink_leds) / sizeof(blink_leds[0])); ++i)
    {
      blink_leds[i].turn_off ();
    }

  timer.sleep (BLINK_OFF_TICKS);

  ++seconds;
  trace_printf ("Second %u\n", seconds);

  // --------------------------------------------------------------------------

  RCC_AHBPeriphClockCmd (BUTTON_RCC_MASKx(BUTTON_PORT_NUMBER), ENABLE);

  GPIO_InitTypeDef button_gpio_init;

  // Configure pin in input mode
  button_gpio_init.GPIO_Pin = BUTTON_PIN_MASK(BUTTON_PIN_NUMBER);
  button_gpio_init.GPIO_Mode = GPIO_Mode_IN;
  button_gpio_init.GPIO_Speed = GPIO_Speed_2MHz;
  button_gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init (BUTTON_GPIOx(BUTTON_PORT_NUMBER), &button_gpio_init);

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  SYSCFG_EXTILineConfig ((uint8_t) BUTTON_PORT_NUMBER, EXTI_PinSource0);

  EXTI->IMR |= BUTTON_PIN_MASK(BUTTON_PIN_NUMBER);
  EXTI->RTSR |= BUTTON_PIN_MASK(BUTTON_PIN_NUMBER);
  EXTI->FTSR |= BUTTON_PIN_MASK(BUTTON_PIN_NUMBER);

  NVIC_EnableIRQ (EXTI0_1_IRQn);

  // --------------------------------------------------------------------------

  // Blink individual leds.
  for (size_t i = 0;
      (i < (sizeof(blink_leds) / sizeof(blink_leds[0]))) && (!button_pressed);
      ++i)
    {
      blink_leds[i].turn_on ();
      timer.sleep (BLINK_ON_TICKS);

      if (button_pressed)
	break;

      blink_leds[i].turn_off ();
      timer.sleep (BLINK_OFF_TICKS);

      if (button_pressed)
	break;

      ++seconds;
      trace_printf ("Second %u\n", seconds);
    }

  // Blink binary.
  for (int i = 0; (i < loops) && (!button_pressed); i++)
    {
      for (size_t l = 0; l < (sizeof(blink_leds) / sizeof(blink_leds[0])); ++l)
	{
	  blink_leds[l].toggle ();
	  if (blink_leds[l].is_on ())
	    {
	      break;
	    }
	}

      if (button_pressed)
	break;

      timer.sleep (timer_systick::FREQUENCY_HZ);

      ++seconds;
      trace_printf ("Second %u\n", seconds);
    }

  for (size_t i = 0;
      (i < (sizeof(blink_leds) / sizeof(blink_leds[0]))) && (!button_pressed);
      ++i)
    {
      blink_leds[i].turn_on ();
    }

  do
    {
      timer.sleep (timer_systick::FREQUENCY_HZ);
      ++seconds;
      trace_printf ("Second %u\n", seconds);
    }
  while (button_pressed);

  return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

int led_no = 0;
int old_val = 0;

static void
GPIO_EXTI_Callback (uint16_t mask)
{
  int val = ((BUTTON_GPIOx(BUTTON_PORT_NUMBER)->IDR & mask) != 0);

  if (val != old_val)
    {
      if (button_pressed == 0)
	{
	  button_pressed = 1;

	  for (size_t i = 0; i < (sizeof(blink_leds) / sizeof(blink_leds[0]));
	      ++i)
	    {
	      blink_leds[i].turn_off ();
	    }
	}

      if (val)
	{
	  blink_leds[led_no].turn_on ();
	}
      else
	{
	  blink_leds[led_no].turn_off ();
#if 1
	  led_no = (led_no + 1) % (sizeof(blink_leds) / sizeof(blink_leds[0]));
	  // trace_printf ("Led %d\n", led_no);
#endif
	}
    }
  old_val = val;
}

extern "C" void
EXTI0_1_IRQHandler (void);

void
EXTI0_1_IRQHandler (void)
{
  trace_printf ("EXTI\n");
  if ((EXTI->PR & BUTTON_PIN_MASK(BUTTON_PIN_NUMBER)) != 0)
    {
      EXTI->PR = BUTTON_PIN_MASK(BUTTON_PIN_NUMBER);

      GPIO_EXTI_Callback (BUTTON_PIN_MASK(BUTTON_PIN_NUMBER));
    }
}

// ----------------------------------------------------------------------------
