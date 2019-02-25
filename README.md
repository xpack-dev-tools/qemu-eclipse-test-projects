Eclipse projects used to test QEMU.

(workspace for Eclipse-46-cdt-test.app)

# STM32F4-Discovery

## f407-disc-blink-tutorial

This is the same project as described in the 
[blinky tutorial](https://gnu-mcu-eclipse.github.io/tutorials/blinky-arm/).

It is a semihosted project, blinking the 4 LEDs.

The VSC launcher refer to this project; for convenience,
the Debug elf is included.

## f407-disco-birq

A button test using interrupts.

## f407-disco-bpoll

A button test using polling.

## f407-disc-blink

The more elaborate blink test, it blinks all 4 leds, in various ways (all, each, binary count).

## nvic

The NVIC test, used to develop the BASEPRI patch.

## bkpt

Issue a BKPT, should halt in the debugger.
