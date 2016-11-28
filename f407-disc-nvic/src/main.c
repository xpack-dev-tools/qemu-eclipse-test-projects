//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"

#include "cmsis_device.h"

// ----------------------------------------------------------------------------
//
// Semihosting STM32F4 empty sample (trace via ITM).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the ITM output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

void
WWDG_IRQHandler (void);

void
WWDG_IRQHandler (void)
{
  printf ("$0\n");
}

void
PVD_IRQHandler (void);

void
PVD_IRQHandler (void)
{
  printf ("$1\n");
}


void
TAMP_STAMP_IRQHandler(void);

void
TAMP_STAMP_IRQHandler(void)
{
  printf ("$2\n");
}

int
main (int argc, char* argv[])
{
  // At this stage the system clock should have already been configured
  // at high speed.

  NVIC_SetPriority (2, 4);

  NVIC_EnableIRQ (2);

  printf("disable\n");
  __disable_irq();
  printf("pend\n");
  NVIC_SetPendingIRQ (2);
  printf("enable\n");
  __enable_irq();

  __NOP ();
  __NOP ();
  __NOP ();

  uint32_t st = __get_BASEPRI();

  printf("basepri 4\n");
  __set_BASEPRI_MAX(4 << __NVIC_PRIO_BITS);

  printf("pend\n");
  NVIC_SetPendingIRQ (2);

  __NOP ();
  __NOP ();
  __NOP ();

  printf("basepri 3\n");
  __set_BASEPRI_MAX(3 << __NVIC_PRIO_BITS);

  printf("pend\n");
  NVIC_SetPendingIRQ (2);

  __NOP ();
  __NOP ();
  __NOP ();

  printf("basepri restore\n");
  __set_BASEPRI(st);

  __NOP ();
  __NOP ();
  __NOP ();

  return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
