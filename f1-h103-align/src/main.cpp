//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include <stdint.h>

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

typedef union {
  uint32_t l;
  uint8_t b[4];
  uint16_t w[2];
} ul_t;

int
main(int argc, char* argv[])
{
  // At this stage the system clock should have already been configured
  // at high speed.

  volatile ul_t *p = (ul_t *)0x40022004;

  trace_printf("Address %08X\n", p);
  trace_printf("%08X (before)\n", p->l);
  p->l = 0x12345678;
  trace_printf("%08X\n", p->l);
  trace_printf("%04X %04X\n", p->w[0], p->w[1]);
  trace_printf("%02X %02X %02X %02X\n", p->b[0], p->b[1], p->b[2], p->b[3]);

  volatile uint16_t *p2 = (uint16_t *)(0x40022004+1);
  trace_printf("%04X (unaligned %08X)\n", *p2, p2);

  volatile uint32_t *p4 = (uint32_t *)(0x40022004+1);
  trace_printf("%08X (unaligned %08X)\n", *p4, p4);

  return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
