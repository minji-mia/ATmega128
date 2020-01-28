# ATmega128

Digital clock operation using ATmega128, an AVR Processor based microcontroller unit

You can use a digital clock, stopwatch, alarm, down count timer, and calculator.

• Designed the Atmega128 Board and wrote the software programs in C language
• The Board had Atmega128 Micro Controller, 7-Segment, SN74LS47, Resistor, Diode, 7805 Regulator, Capacitor, LED, Button, and Buzzer
• Mode 0 (Digital Clock): The most basic purpose is a digital clock using a timer. 7-segment is 10min, min, 10s, 1s, 10ms. It is arranged in order of 1ms, and the time is counted
• Mode1 (StopWatch): This function records the time and includes the Lap_time. The stopwatch function turns on the LED and counts from 1s. Use switch2 and 3 to count the numbers
• Mode2 (Alarm): Sets the alarm of the digital clock. After setting the time using switch2 and 3 as the alarm function, the buzzer sounds when the timer reaches mode 0
• Mode3 (Down Count Timer): Timer to count down from the set time to 0. If you set the time using switch2 and 3 as the DownCounter, the buzzer will sound when the count goes down to 00
• Mode4 (Calculator): A calculator performs arithmetic operations. Enter two digits using switch2 and 3 to achieve the desired arithmetic operation with switch6

### 1.Circuit
![circuit](https://user-images.githubusercontent.com/52568892/73293052-c9047a80-41c8-11ea-9f3c-a13189181150.jpg)

### 2.Front
![front](https://user-images.githubusercontent.com/52568892/73293360-5f38a080-41c9-11ea-9135-426a4f5d95c9.jpg)

### 3.Back
![back](https://user-images.githubusercontent.com/52568892/73293379-6a8bcc00-41c9-11ea-9a64-9ef17bc4f633.jpg)
