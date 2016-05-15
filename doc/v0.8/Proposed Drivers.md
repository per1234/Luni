
----------
March 2016 V 0.8  Doug Johnson (finson@whidbey.com) 

----------

## DeviceFeature and Device Drivers - Work in Progress

The document device-driver.md describes the overall device feature proposal and most parts of the detailed design.  

This document is a catch all for draft additions to the appendices.  It is a place to put new device driver specifications as they are developed, detailed descriptions of new features, etc.

**Nothing** in this document is guaranteed to ever be released!

---
##*Stepper Motor (DDStepper)*

###`CDR::Configure`
####Set stepper motor interface configuration

Configure the stepper motor attached to the given handle and its associated software object.

###`CDR::Configure`
####Get stepper motor interface configuration

Get the configuration of the stepper motor attached to the given handle and its associated software object.

*Method signatures*

`int status(int handle, CDR::Configure, int count, byte *buf)`
`int control(int handle, CDR::Configure, int count, byte *buf)`

*Parameter block buffer*

####2-wire interface (EasyDriver: step and direction)

     0  interface type (0)
     1  steps per revolution (LSB)
     2  steps per revolution (MSB)
     3  direction pin (LSB)
     4  direction pin (MSB)
     5  step pin (LSB)
     6  step pin (MSB)

####2-wire interface (unipolar motor)

     0  interface type (1)
     1  steps per revolution (LSB)
     2  steps per revolution (MSB)
     3  pin1 (LSB)
     4  pin1 (MSB)
     5  pin2 (LSB)
     6  pin2 (MSB)

####4-wire (bipolar motor)

     0  interface type (2)
     1  steps per revolution (LSB)
     2  steps per revolution (MSB)
     3  pin1 (LSB)
     4  pin1 (MSB)
     5  pin2 (LSB)
     6  pin2 (MSB)
     7  pin3 (LSB)
     8  pin3 (MSB)
     9  pin4 (LSB)
    10  pin4 (MSB)

---
###`REG::MoveR`
####Move to new relative position

*Method signature*

`int control(int handle, Stepper::MoveR, 5, byte *buf);`

*Parameter block buffer*

     0  delta position (steps) (LSB)
     1  delta position (steps)
     2  delta position (steps)
     3  delta position (steps) (MSB)
     4  block? (0 or 1)

---
###`REG::PositionEvent`
####Stopped at most recently commanded position? (synchronous or asynchronous)

*Method signature*

`int status(int handle, Stepper::PositionEvent, 5, byte *buf);`

*Parameter block buffer*

     0  At position? 0 -> no (running), 1 -> yes (stopped)
     1  Position provided?  0 -> no, 1 -> yes
     2  (opt) absolute position (steps) (LSB)
     3  (opt) absolute position (steps)
     4  (opt) absolute position (steps)
     5  (opt) absolute position (steps) (MSB)

---
###`REG::RPMSpeeds`
####Set stepping speed using 16-bit Revolutions Per Minute values

###`REG::RPMSpeeds`
####Get stepping speed using 16-bit Revolutions Per Minute values

*Method signature*

`int control(int handle, Stepper::RPMSpeeds, 8, byte *buf);`
`int status(int handle, Stepper::RPMSpeeds, 8, byte *buf);`

*Parameter block buffer*

     0  target speed RPM (LSB)
     1  target speed RPM (MSB)
     2  max speed RPM (LSB)
     3  max speed RPM (MSB)
     4  accel (LSB)
     5  accel (MSB)
     6  decel (LSB)
     7  decel (MSB)


---

##[Proposed Driver]
##*Signal I/O (DDSignal)*  

Basic digital and analog signal input and output.

`CDR::DriverVersion`

`CDR::Intervals`

See Appendix A for information about the operation of the common device registers.


- modes are j5 modes - INPUT: 0, OUTPUT: 1, ANALOG (or ADC): 2, PWM: 3, SERVO: 4 
- actions are digital read, digital write, analog read (ANALOG or ADC, PWM), analog  write (DAC, PWM)

- define pin group (r or w) all group pins are same direction (read or write) but not necessarily the same type of pin, each pin group has an access handle that can actually do the work (possibly using a port expander)
- delete pin group
- read group, write group (once, intervals) 
- read group and average each channel
- set analog input reference
- set analog input or output resolution

Debugger
<a href="#debugger_debugger">Debugger</a>