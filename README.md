# CEC MQTT Client

A client for monitoring and controlling cec (hdmi) devices via mqtt.

This software contains a mqtt client which subscribes and publishes to a given mqtt broker.
It can be used for home automation.

It is developed, tested and maintained on **Raspberry Pi**

I am using [libCEC](https://github.com/Pulse-Eight/libcec) 


## IT IS NOT IN A USABLE STATE YET

## Features



#### This device (the device this client is running on) 

Description | Read (publish) | Write (subscribe) | Development state
------------|-------------|----------------|-------------------
| Name | [x] | [x] |  [ ]
 
 
 
#### Main device (e.g. TV) 


Description | Read (publish) | Write (subscribe) | Development state
------------|-------------|----------------|-------------------
Power state | [x] | [x]| [ ]
Active device (ID) | [x] | [x] | [ ]


#### Other connected Devices

Description | Read (publish) | Write (subscribe) | Development state
------------|-------------|----------------|-------------------
ID | [x] | [ ] | [ ]
Name | [x] | [ ] | [ ]
Power state | [x] | - [x] | [ ]




