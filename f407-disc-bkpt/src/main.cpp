//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "assert.h"

#include "timer_systick.h"
#include "BlinkLed.h"

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
  printf("bkpt.\n");

  // assert(0);

  asm volatile ("bkpt");
  asm volatile ("bkpt #1");
  asm volatile ("bkpt #2");

  printf("done.\n");
  return 0;
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
