# Gaia's Gampad (GGP)
This is most of what is required to make this gamepad from scratch. This gamepad was made with an arduino micro, 3d printed casing, and handmade circuit boards. Maybe at some point i will do a better writeup of this with detailed instructions on how to make your own. But hey... technically everything is here that you need.

## Overview
This controller supports 16 buttons for inputs and one button from swapping mode, current supports mouse input mode and standard HID gamepad input. We are using the gampad library for the micro that can be found in the arduino ide library manager. The buttons themselves are setup using a button matrix scanning through the inputs over and over again every cycle. Also included are the wiring schematics for this gamepad in case you would like to reproduce it or improove it. Please note its only populated with a schematic and not a pcb layout or the like but kiCad auto generates the file for you.

## Parts
- Arduino Micro
- pla casing
- 17 push buttons
- 17 diodes (one per button to prevent ghosting)
- 2 analog sticks with pcb module for them to mount with
- 5 pefboards cut to size
- arduino socket
- m3 screws
- a lot of wire

### more coming eventually
i want a full instruction but dont have the effort right now