# ac_voltage_current
Microntroller Atmega32.
AC RMS Voltage, Current and Frequency measurement using scheduler method using timer.

Using Timer0, I have triggered ADC for reading ADC values of Voltage and Current.

Using Timer 2, I have scheduled flag setting for task of 10ms, 500 ms and 1000ms.

flag_10ms task, calculataing values of RMS Voltage and Current.

flag_500ms tasl. transmit values over terminal using UART.

flag_1000ms task, calculate frequency using Timer1 input capture on falling edge.

While calculating frequency I have disabled Timer0 and Timer2 Overflow interrupt by setting TIMSK = 0x00,
Once I have done gettin frequency, enabling back Timer0 and Timer2 Overflow interrupt by setting TIMSK = 0x41.

Below is the Proteus Schematic and Video.

https://drive.google.com/file/d/1UQ8jmw-vFK8OMxbGe88OrKq7GnHCb_R1/view?usp=sharing

![ac_voltage_current2](https://user-images.githubusercontent.com/111571035/187989607-8c8c72ba-c6be-413e-b8f2-4d56c63ad7be.SVG)

https://drive.google.com/file/d/1UQ8jmw-vFK8OMxbGe88OrKq7GnHCb_R1/view?usp=sharing
