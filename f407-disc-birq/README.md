# f407-disc-birq

This project can be used to test the STM32F4DISCOVERY user button.

## Expected behaviour

After reset, the four coloured LEDs will lit for a second, then turn off.

Click the blue button; the LEDs will lit one by one. The LED will tun on when the mouse button is down and will turn off when the mouse button is up.

Click the black button; the board will reset. 

## Details

The button is connected to GPIOA0; the application enables EXTI0 and installs a callback that will turn on and off each button in sequence.
