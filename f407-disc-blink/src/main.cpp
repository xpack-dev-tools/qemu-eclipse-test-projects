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
  constexpr Timer::ticks_t BLINK_ON_TICKS = Timer::FREQUENCY_HZ * 3 / 4;
  constexpr Timer::ticks_t BLINK_OFF_TICKS = Timer::FREQUENCY_HZ
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

BlinkLed blinkLed[4] =
  {
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_GREEN, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_ORANGE, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_RED, BLINK_ACTIVE_LOW },
    { BLINK_PORT_NUMBER, BLINK_PIN_NUMBER_BLUE, BLINK_ACTIVE_LOW }, };

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

  Timer timer;
  timer.start ();

  // Perform all necessary initialisations for the LEDs.
  for (size_t i = 0; i < (sizeof(blinkLed) / sizeof(blinkLed[0])); ++i)
    {
      blinkLed[i].powerUp ();
    }

  uint32_t seconds = 0;

#define LOOP_COUNT (1 << (sizeof(blinkLed) / sizeof(blinkLed[0])))

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

  for (size_t i = 0; i < (sizeof(blinkLed) / sizeof(blinkLed[0])); ++i)
    {
      blinkLed[i].turnOn ();
    }

  timer.sleep (BLINK_ON_TICKS);

  for (size_t i = 0; i < (sizeof(blinkLed) / sizeof(blinkLed[0])); ++i)
    {
      blinkLed[i].turnOff ();
    }

  timer.sleep (BLINK_OFF_TICKS);

  ++seconds;
  trace_printf ("Second %u\n", seconds);

  // Blink individual leds.
  for (size_t i = 0; i < (sizeof(blinkLed) / sizeof(blinkLed[0])); ++i)
    {
      blinkLed[i].turnOn ();
      timer.sleep (BLINK_ON_TICKS);

      blinkLed[i].turnOff ();
      timer.sleep (BLINK_OFF_TICKS);

      ++seconds;
      trace_printf ("Second %u\n", seconds);
    }

  // Blink binary.
  for (int i = 0; i < loops; i++)
    {
      for (size_t l = 0; l < (sizeof(blinkLed) / sizeof(blinkLed[0])); ++l)
        {
          blinkLed[l].toggle ();
          if (blinkLed[l].isOn ())
            {
              break;
            }
        }
      timer.sleep (Timer::FREQUENCY_HZ);

      ++seconds;
      trace_printf ("Second %u\n", seconds);
    }

  for (size_t i = 0; i < (sizeof(blinkLed) / sizeof(blinkLed[0])); ++i)
    {
      blinkLed[i].turnOn ();
    }

  timer.sleep (Timer::FREQUENCY_HZ);

  return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
