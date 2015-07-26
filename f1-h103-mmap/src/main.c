//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "diag/Trace.h"

// ----------------------------------------------------------------------------
//
// Semihosting STM32F1 empty sample (trace via DEBUG).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the DEBUG output,
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

int
main (int argc, char* argv[])
{
  // At this stage the system clock should have already been configured
  // at high speed.
  if (argc < 2)
    {
      trace_puts ("Usage: pgm HEX");
      exit (1);
    }

  uint64_t param = strtoll(argv[1], NULL, 0);
  uint32_t addr = (uint32_t)param;
  trace_printf ("Memory read access test from 0x%08X\n", addr);

  trace_printf("Content is 0x%08X\n", *((uint32_t*)addr));

}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
