# HB0057_DC28_Badge
This is contains some programs written to run on the Hacker Box #0057 DEFCON 28 badge based on the ESP32 board. Most of this should port easily to other ESP32 boards with an LCD screen.

Be sure to setup the arduino enviroment first for ESP32 and the LCD library. I have written up a guide here
[https://miswired.github.io/esp32/2020/08/16/HB0057DC28/](https://miswired.github.io/esp32/2020/08/16/HB0057DC28/)


## BetterMario
This project contains a better example of Mario sounds that I ported to the badge. I felt like that Mario example that came from the HackerBox instructable didn't quite do it for me. See the project for references and credits. 

## GetMAC
A simple sketch that displays the ESP32's MAC address. Handy for pairing boards. See project for credits.

## Comms
This sketch demonstrates an example of wireless comms between two ESP32 boards using the ESP-NOW library. This is the main sketch that should be put on the HB0057 badge. There is LOTS to improve here, but it does work and maybe someone will find it helpful. ¯\_(ツ)_/¯

## Comms_Peer
This is a simple sketch that waits for a ping request and responds to the board running the Comms sketch. This can be run on a standalone ESP32 without a screen. 
