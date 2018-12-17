# CEC MQTT Client

A client for monitoring and controlling cec (hdmi) devices via mqtt.

This software contains a mqtt client which subscribes and publishes to a given mqtt broker.
It can be used for home automation.

It is developed, tested and maintained on **Raspberry Pi**

I am using [libCEC](https://github.com/Pulse-Eight/libcec) 


## IT IS NOT IN A USABLE STATE YET

## Features



#### This device (the device this client is running on) 

Description | Read (publish) | Write (subscribe) | Implemented
------------|-------------|----------------|-------------------
Name | YES | YES | NO
 
 
 
#### Main device (e.g. TV) 


Description | Read (publish) | Write (subscribe) | Implemented
------------|-------------|----------------|-------------------
Power state | YES | YES | NO
Active device (ID) | YES | YES | NO


#### Other connected Devices

Description | Read (publish) | Write (subscribe) | Implemented
------------|-------------|----------------|-------------------
ID | YES | NO | NO
Name | YES | NO | NO
Power state | YES | YES | NO




