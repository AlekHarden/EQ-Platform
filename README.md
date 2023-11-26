## EQ-Platform
Equatorial Platform

The purpose of this project is to build an equatorial platform to set any telescope and/or camera on top and actively track the motion of the night sky.
This enables long exposure photography of very faint objects like galaxies and nebulae, which would otherwise be impossible to capture in high detail with a manually tracked telescope or camera.

The design was inspired by Andy Fell's Video: https://www.youtube.com/watch?v=fGdvxtIfSmU

This project uses a spare Creality3D Melzi 1.1.3 mainboard from an Ender3 3D printer, along with stepper motors and a lead screw scrapped from a 5th Generation MakerBot Replicator that I tore apart. 
(The MakerBot was so bad at 3D printing, that the parts have more utility doing other things, like controlling my projects.)

My original plan was to buy a separate stepper driver board to use with an Arduino, but in the interest of time, 
I decided on reviving my old 3D printer mainboard since it has stepper drivers built in! All I had to do was learn how to program it...

To my surprise, the Ender3 mainboard is based on Arduino and uses an ATMEGA1284P processor!
The open-source community has generously created a bootloader for this chip, which allows users to create programs in C using the Arduino IDE. 
Third party stepper libraries work too!

# Current Capability

Since the project uses a stepper motor and lead screw as the main actuator for rotating the platform, there is some funky math required to map a linear motion to an angular one.
To obtain this mapping, I simulated the movement within cad software and created a discrete table of linear distance vs angular position data points.
Then I plotted the data in Excel and fitted a 4th order polynomial curve. This curve is used to interpolate the data and allow smooth movement of the stepper motor.
Also, I implemented overall scaling of the curve with external buttons to adjust the tracking speed manually if it seems to be going too fast or slow.

However, this simulated + manual scaling approach has resulted in sub-par tracking (probably due to imprecise real-world limitations of wood construction). The result is bad tracking that only allows short exposures of less than 10 seconds before star trailing occurs and blurs the fine details of the image. (But still WAY better than manual tracking)

For astrophotography, ideally exposures of around 1 or 2 minutes with pixel perfect tracking are required.

# WIP Plans

To improve the tracking, I plan on calibrating the actuation curve with real world data by creating a tracking feedback loop, in which a camera (Or "Guide Scope" as known by astrophotographers) detects star movement and adjusts the speed of the stepper motor on the fly to counter the movement. 
Over the course of the full actuation, I plan to save the adjustments to a new curve which will be the new default for the next session. 
Ideally, the actuation curve will have been calibrated, and the feedback system can be removed. This way the mainboard can then return to working independently of the feedback system.
(Or if the user really wants, they can reattach the guide scope to guarantee tracking).

To accomplish these goals the current WIP Plan is:
1. Attach USB webcam to finder scope or steam video from the telescope camera.
2. Obtain data with Raspberry Pi running OpenCV in C++ and use optical flow algorithm to determine unwanted motion of FOV.
3. Get the component of the motion that is controlled by the Polar Axis. (Hopefully all of it) (The component that cannot be controlled would be due to incorrect polar alignment which is adjusted using the feet on the platform.)
4. Send speed adjustments via serial to the Melzi mainboard.
5. Save adjustments as default.

Extra features I would like to implement:
- Bahtinov Mask Diffraction Spike Analyzer / Manual Focusing Assist.

# Important links which helped me:

Programming the Ender3 Mainboard:
https://youtu.be/FJ9al1n0-pY?si=JfbQ9YBzFDdZIu13

Installing a Bootloader 
https://www.youtube.com/watch?v=fIl5X2ffdyo

ATMEGA 1248P Documentation  
http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega164A_PA-324A_PA-644A_PA-1284_P_Data-Sheet-40002070B.pdf

Melzi Pin Assignments
https://github.com/MarlinFirmware/Marlin/blob/2.0.x/Marlin/src/pins/sanguino/pins_MELZI_CREALITY.h

FlexyStepper Library Documentation  
https://github.com/Stan-Reifel/FlexyStepper/blob/master/Documentation.pdf
