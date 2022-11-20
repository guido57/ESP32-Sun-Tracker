# ESP32-Sun-Tracker

## Overview

The hardware prototype is described on hackaday.io: [Sun Tracker prototype](https://hackaday.io/project/188318-sun-tracker-prototype)

The hardware schematic is the following:

![](https://github.com/guido57/ESP32-Sun-Tracker/blob/master/docs/schematic.jpg)

## Software for ESP32 is in this repository.

### main.cpp

In the loop():
* change LED blinking according to WIFI status
* read the settings (max_error, sensitivity, update_period, start_time, stop_time) and apply them to the SunTracker object
* call LED_loop, SunTracker.loop and AutoConnect loop

### AutoConnect

Create a captive portal
Let to connect to WIFI
Let to change settings








