# MotorSketch
A program for the Teensy 3 board (with an arm - it should also work for an STM32 board) to test a motor control program with quadrature rotary encoder feedback.

This isn't much use without the specific hardware, but if you're trying to read a quadrature rotary encoder output with interrupts, you might find the interrupt code here useful.

Chips like the ATXMega and other more capable microcontrollers can have built in quadrature decoders. 

The motor control code is to talk to a motor controller. It tries to control ramp up and ramp down with tables using the feedback to tell it when to do what. The ramp tables are tuned for the hardware I was building. All-in motor control solutions exist, but this is for playing with the low level motor control code.

This is on github because a friend wants to see the quadrature code. That's all.

