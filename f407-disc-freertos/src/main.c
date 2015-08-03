//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "FreeRTOS.h"
#include "task.h"

//#include "Timer.h"
#include "BlinkLed.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 led blink sample (trace via DEBUG).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (configTICK_RATE_HZ * 3 / 4)
#define BLINK_OFF_TICKS (configTICK_RATE_HZ - BLINK_ON_TICKS)

void
ledTaskFunction (void* parms __attribute__((unused)))
{
  blink_led_init ();

  uint32_t seconds = 0;

  // Infinite loop
  while (1)
    {
      blink_led_on ();
      vTaskDelay (BLINK_ON_TICKS);

      blink_led_off ();
      vTaskDelay (BLINK_OFF_TICKS);

      ++seconds;

      // Count seconds on the trace device.
      trace_printf ("Second %u\n", seconds);
    }

}
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
  // Send a greeting to the trace device (skipped on Release).
  trace_puts ("Hello ARM World!");

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf ("System clock: %u Hz\n", SystemCoreClock);

  xTaskCreate(ledTaskFunction, "led", 256, NULL, 1, NULL);

  vTaskStartScheduler ();

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
