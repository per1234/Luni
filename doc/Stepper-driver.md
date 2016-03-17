##Device Driver Stepper

Implemented as part of the Device Driver Feature proposed for Firmata 2.6

##Introduction

This Stepper Device Driver provides control methods for Stepper motors attached to an Arduino-compatible board.  The methods can be accessed directly by other code on the Arduino or remotely by code on another host communicating via the Firmata protocol with the Arduino.


###Method Prototypes

The method prototypes shown below are the primary interface for all Device Drivers.  The type identifier `int` is used to indicate a signed integer value of at least 16 bits.  Only the two low order bytes are transmitted for these values by Firmata.

See device-driver.md for more discussion of how the queries and responses are encoded by Firmata and where the parameter values are placed in each message type.

##Stepper Device Driver Methods

###Stepper - Open

By default, this device driver recognizes names beginning with "Stepper", but other prefixes can be specified when the device driver is initialized.

    int open(char* name)
    int open(char* name, int flags)

**param** (in) `name` Name of the logical unit to open.  UTF-8 encoded, null terminated.

**param** (in) `flags` Flags associated with the open.  Default: 0.

**return** *Success*: The newly assigned handle value.  The handle is used in future calls to indicate the device driver and specific device being addressed.  *Error*: error code.

---
###Stepper - Status

Read information from a register (or virtual register) in the device or device driver.  

The method and its parameters are as follows.

    int status(int handle, int reg, int count, byte *buf)

**param** (in) `handle` The device driver selector value returned by Open in a previous call.

**param** (in) `reg` The register address at which to start reading.

**param** (in) `count` The number of bytes to read.

**param** (out) `buf` Pointer to the buffer to receive the data read.  Must be large enough to hold `count` bytes.

**return**  *Success*: The number of bytes actually read.  A short count does not in itself cause an error, since the caller can determine that not everything requested was read which may not actually be an error.  *Error*: error code.

---
###Stepper - Control

    int control(int handle, int reg, int count, byte *buf)

**param** (in) `handle` The device driver selector value returned by Open in a previous call.

**param** (in) `reg` The control register address at which to start writing.   This value does not necessarily name a real register, but is used by the device driver to identify the desired control function.  

**param** (in) `count` The number of bytes to write.

**param** (in) `buf` Pointer to the buffer containing the data to write.

**return**  *Success*: The number of bytes actually written.  Ordinarily, this will be equal to the requested number of bytes to write.  If it is short due to some device error (eg, physical write failure), then the driver will return an error code (eg, `EIO`).  However, under some unique circumstances for some drivers, it may be reasonable for a short count to occur in which case the driver will return the short count and no error code.  *Error*: error code.

---
###Stepper - Read

    int read(int handle, int count, byte *buf)

**param** (in) `handle` The device driver selector value returned by Open in a previous call.

**param** (in) `count` The number of bytes to read.

**param** (out) `buf` Pointer to the buffer to receive the data read.  Must be large enough to hold `count` bytes.

**return**  *Success*: The number of bytes actually read.  A short count does not in itself cause an error, since the caller can determine that not everything requested was read which may not actually be an error.  *Error*: error code.

---
###Stepper - Write

    int write(int handle, int count, byte *buf)

**param** (in) `handle` The device driver selector value returned by Open in a previous call.

**param** (in) `count` The number of bytes to write.

**param** (in) `buf` Pointer to the buffer containing the data to write.  Must contain at least `count` bytes.

**return**  *Success*: The number of bytes actually written.  Ordinarily, this will be equal to the requested number of bytes to write.  If it is short due to some device error (eg, physical write failure), then the driver will return an error code (eg, `EIO`).  However, under some unique circumstances for some drivers, it may be reasonable for a short count to occur in which case the driver will return the short count and no error code.  *Error*: error code.

---
###Stepper - Close

    int close(int handle)

**param** (in) `handle` The device driver selector value returned by Open in a previous call.  The selected device driver is responsible for deciding what actions if any are needed to "close" the connection.  After a close, the only valid action on the device is another open.

**return**  Success: 0.  *Error*: error code.

