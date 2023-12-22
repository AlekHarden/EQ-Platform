# Tracking Calibration Program

This program is used to calibrate the stepper profile, or mapping of steps to degrees.

The mapping is non linear and difficult to simulate or calculate, therefore expirimental data can be gathered and applied instead.

It works by detecting stars through a usb camera, and also detecting their movement which the EQ-platform is designed to minimize.

If movement is detected, feedback to the stepper controller is sent via usb in order to calibrate the stepper profile and reduce movment.

Ideally, the stepper profile should only need to be calibrated once, then saved to the controllers EEPROM since the rotation of the earth is constant.


## Keyboard Controls:
### General:
- `ESC` - Close program
- `P` - Reopen camera properties window (if it's closed).
- `R` - Start/Stop recording
### Image Calibration:
#### Dark Frame
- `D` - Toggle Dark Frame Calibration (Default: Off)
- `C` - Start/Stop dark frame acquisition.
- `X` - Stack aquired dark frames, and apply stack as new master dark.
- `Z` - Show master dark offset (Signed 16 bit) as 8 bit two channel image (Green + , Red -)




