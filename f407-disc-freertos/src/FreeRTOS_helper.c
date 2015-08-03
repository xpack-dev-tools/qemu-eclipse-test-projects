//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include "FreeRTOS.h"
#include "task.h"

#if defined(USE_HAL_DRIVER)
#if defined(STM32F407xx)
#include "stm32f4xx_hal.h"
#else
extern void HAL_IncTick(void);
#endif
#endif

// ----------------------------------------------------------------------------

// FreeRTOS helper functions.

// ----------------------------------------------------------------------------

/**
 * This handler has dual use, first by HAL which uses it for
 * timings, and then by FreeRTOS, for scheduling. The extra test
 * for schedule state is mandatory, otherwise the TickHandler
 * hang if called before starting the scheduler.
 */
void __attribute__ ((section(".after_vectors")))
SysTick_Handler (void)
{
#if defined(USE_HAL_DRIVER)
  HAL_IncTick ();
#endif

  if (xTaskGetSchedulerState () != taskSCHEDULER_NOT_STARTED)
    {
      xPortSysTickHandler ();
    }
}

/**
 *  This function will get called if a task overflows its stack.   If the
 * parameters are corrupt then inspect pxCurrentTCB to find which was the
 * offending task.
 */
void
vApplicationStackOverflowHook (TaskHandle_t pxTask, char *pcTaskName)
{

  (void) pxTask;
  (void) pcTaskName;

  for (;;)
    ;
}

#if defined(USE_FULL_ASSERT)

/**
 * This function is called when a FreeRTOS assert failed.
 */
void
vAssertCalled (char *pucFile, unsigned long ulLine)
{
  (void) pucFile;
  (void) ulLine;

  for (;;)
    ;
}

#endif

// ----------------------------------------------------------------------------

uint32_t ulRunTimeStatsClock;

// ----------------------------------------------------------------------------

