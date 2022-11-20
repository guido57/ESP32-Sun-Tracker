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

See also 
<a href="https://hieromon.github.io/AutoConnect/index.html" target="_blank">AutoConnect by Hieromon</a>

* Automatically start a WIFI hostpot (ESP32 softAP) with a captive portal at first run of if there's no connection to a WIFi hotspot
* Let to connect to a WIFI hotspot
* Reconnect in case of reboot or connection lost
* Let to change settings with a web interface

<img src="https://github.com/guido57/ESP32-Sun-Tracker/blob/master/docs/AutoConnect-settings.jpg" alt="drawing" width="400"/>







