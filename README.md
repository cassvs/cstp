# Control System Test Platform
A simple hardware and software platform for playing around with control theory.

Check out the [wiki](https://github.com/cassvs/cstp/wiki) for more about the hardware.

The software provides the folowing features:
- PWM output, for controlling a motor
- Quadrature encoder input, for position and velocity feedback
- System tick function
- UART IO, featuring a simple protocol for reading and writing 20 one-byte registers and issuing commands to up to 26 devices on the same bus
