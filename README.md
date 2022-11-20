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
* call:
  * LED_loop
  * SunTracker.loop (only if now time is inside start_time and stop_time)
  * AutoConnect loop

### AutoConnect

See also 
<a href="https://hieromon.github.io/AutoConnect/index.html" target="_blank">AutoConnect by Hieromon</a>

* Automatically start a WIFI hostpot (ESP32 softAP) with a captive portal at first run of if there's no connection to a WIFi hotspot
* Let to connect to a WIFI hotspot
* Reconnect in case of reboot or connection lost
* Let to change settings with a web interface

<img src="https://github.com/guido57/ESP32-Sun-Tracker/blob/master/docs/AutoConnect-settings.jpg" alt="drawing" width="400"/>

### SunTracker

In its constructor receives the pins where:
* PDR1, PDR2, RELE1, RELE2, RUNEND1, RUNEND2 
are connected 

* Read the two photo resistors: PDR1 and PDR2
* Check if the average of PDR1 and PDR2 values is above the minimum required (sensitivity)
* Compare their values with max_error
* Activate the two relays to rotate the DC motor to a new position in which abs(pdr2-pdr1) < avg(PDR1,PDR2) * max_error/100
* Stop both relays if one of the runend is reached (closed)



