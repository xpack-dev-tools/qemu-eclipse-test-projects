//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "timer_systick.h"
#include "blink_led.h"

// ----------------------------------------------------------------------------
//
// Semihosting STM32F4 led blink sample (trace via DEBUG).
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
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
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

#define BLINK_PORT_NUMBER               (3)
#define BLINK_PIN_NUMBER_GREEN          (12)
#define BLINK_PIN_NUMBER_ORANGE		(13)
#define BLINK_PIN_NUMBER_RED		(14)
#define BLINK_PIN_NUMBER_BLUE		(15)
#define BLINK_ACTIVE_LOW                (false)

blink_led blink_leds[4] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_GREEN, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_ORANGE, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_RED, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_BLUE, BLINK_ACTIVE_LOW },
  /**/
  };

// ----- Button definitions ---------------------------------------------------

#define BUTTON_PORT_NUMBER 		(0)
#define BUTTON_PIN_NUMBER 		(0)

#define BUTTON_GPIOx(_N)           	((GPIO_TypeDef *)(GPIOA_BASE + (GPIOB_BASE-GPIOA_BASE)*(_N)))
#define BUTTON_PIN_MASK(_N)             (1 << (_N))
#define BUTTON_RCC_MASKx(_N)         	(RCC_AHB1ENR_GPIOAEN << (_N))

void
SYSCFG_EXTILineConfig (uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex);

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

  // --------------------------------------------------------------------------

  RCC->AHB1ENR |= BUTTON_RCC_MASKx(BUTTON_PORT_NUMBER);

  GPIO_InitTypeDef button_gpio_init;

  // Configure pin in input mode
  button_gpio_init.Pin = BUTTON_PIN_MASK(BUTTON_PIN_NUMBER);
  button_gpio_init.Mode = GPIO_MODE_IT_RISING_FALLING;
  button_gpio_init.Speed = GPIO_SPEED_LOW;
  button_gpio_init.Pull = GPIO_NOPULL;
  HAL_GPIO_Init (BUTTON_GPIOx(BUTTON_PORT_NUMBER), &button_gpio_init);

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  SYSCFG_EXTILineConfig ((uint8_t) BUTTON_PORT_NUMBER, 0);

  EXTI->IMR |= BUTTON_PIN_MASK(BUTTON_PIN_NUMBER);
  EXTI->RTSR |= BUTTON_PIN_MASK(BUTTON_PIN_NUMBER);
  EXTI->FTSR |= BUTTON_PIN_MASK(BUTTON_PIN_NUMBER);

  NVIC_EnableIRQ (EXTI0_IRQn);

  // --------------------------------------------------------------------------

  uint32_t seconds = 0;

  // Perform all necessary initialisations for the LEDs.
  for (size_t i = 0; i < (sizeof(blink_leds) / sizeof(blink_leds[0])); ++i)
    {
      blink_leds[i].power_up ();
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

void
SYSCFG_EXTILineConfig (uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex)
{
  uint32_t tmp = 0x00;

  tmp = ((uint32_t) 0x0F) << (0x04 * (EXTI_PinSourcex & (uint8_t) 0x03));
  SYSCFG->EXTICR[EXTI_PinSourcex >> 0x02] &= ~tmp;
  SYSCFG->EXTICR[EXTI_PinSourcex >> 0x02] |= (((uint32_t) EXTI_PortSourceGPIOx)
      << (0x04 * (EXTI_PinSourcex & (uint8_t) 0x03)));
}

int led_no = 0;
int old_val = 0;

void
HAL_GPIO_EXTI_Callback (uint16_t mask)
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
EXTI0_IRQHandler (void);

void
EXTI0_IRQHandler (void)
{
  trace_printf ("EXTI\n");
  HAL_GPIO_EXTI_IRQHandler (BUTTON_PIN_MASK(BUTTON_PIN_NUMBER));
}

// ----------------------------------------------------------------------------
