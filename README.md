# EQ-Platform
 Since my dobsonian telescope has no motorized tracking, this platform will rotate to keep the subject in view which enables long exposure photograpy.  
The main file, EQ-Platform.ino holds the code which runs on an old ender3 mainboard.   
The ender3 mainboard has the same processor as an arduino, and has stepper drivers built into the board, which allows easy programming
with the arduino ide and an external stepper library.

 Important links which helped me:
 
 Programming the ender3 mainboard  
 https://youtu.be/FJ9al1n0-pY?si=JfbQ9YBzFDdZIu13

 FlexyStepper Library Documentaion  
 https://github.com/Stan-Reifel/FlexyStepper/blob/master/Documentation.pdf

 ATMEGA 1248P Documentation  
http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega164A_PA-324A_PA-644A_PA-1284_P_Data-Sheet-40002070B.pdf  
https://github.com/MarlinFirmware/Marlin/blob/2.0.x/Marlin/src/pins/sanguino/pins_MELZI_CREALITY.h
