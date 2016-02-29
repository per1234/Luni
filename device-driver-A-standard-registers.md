

----------
*tbd* 2016 V 0.6.0 beta.  Doug Johnson (finson@whidbey.com) 

----------

## DeviceFeature and Device Drivers - Appendix A: Common Registers

The document device-driver.md describes the overall device feature proposal and most parts of the detailed design.

One aspect of the design is that device drivers can define virtual register numbers to identify actions that the device driver supports beyond those that can be implemented simply with access to a single hardware register.

This document defines the virtual registers that are available for every device.  Addition virtual registers may be defined by each device driver according to its need.

####Device Status and Control Registers

The status and control methods operate based on register numbers.  On an actual device, physical register numbers usually start at 0 and max out at a relatively low value like 16 or 32, depending on the device.  This DeviceDriver API uses a full 16-bit signed integer to identify the register of interest, so virtual quantities and actions can be implemented in addition to the actual physical device capabilities.

###Common Virtual Register Numbers

The `reg` parameter is always a signed 16-bit integer value.  Physical device register numbers and device-specific virtual register numbers are non-negative.  The common register numbers defined below are negative.

---
##Open

    int open(const char* name)
    int open(const char* name, int flags)

The open method does not use any registers.

---
##Status (CSR::)

    int status(int handle, int reg, int count, byte *buf)

Read information from a register (or virtual register) in the device or device driver.  

Common status registers are in the CSR namespace.

---
###`CSR::DriverVersion`

####Get version number and name of the DeviceDriver.

###`CSR::LibraryVersion`

####Get version number and name of the supporting library.

Version numbers are in the Semantic Versioning format x.y.z-a.b.c  See [semver.org](http://semver.org) for more info.  

The first byte of the return data buffer is the size in bytes of the version identifier packet that follows.  Name strings use UTF-8 encoding and are null-terminated.

In the initial implementation, the size of the version identifier packet is 6 bytes.  The name string immediately follows the version identifier packet and is limited to 128 bytes maximum, including the null terminator.

The size of the receiving buffer should be large enough to hold the 1-byte packet size, a version identifier packet, and a name string (including the null terminator). If the buffer size is not large enough, an error will be returned (`EMSGSIZE`).

*Method signature*

`int status(int handle, CDR_DriverVersion, int bufSize, byte *buf)`
`int status(int handle, CDR_LibraryVersion, int bufSize, byte *buf)`

*Return data buffer*

     0  version descriptor packet size (6, in this example)
     1  major version (x)
     2  minor version (y)
     3  patch version (z)
     4  pre-release (a)
     5  pre-release (b)
     6  pre-release (c)
     7..n  name string (UTF-8, null terminated)



     0  version descriptor packet size (3, in this example)
     1  major version (x)
     2  minor version (y)
     3  patch version (z)
     4..n  name string (UTF-8, null terminated)

---
###`CSR::Intervals`

####Get settings of the microsecond and millisecond timer intervals for the device driver.

There are two timer intervals for each device driver, one set in microseconds and the other set in milliseconds.  If the microsecond value is non-zero the processTimerEvent(...) method of the driver is called each time the interval elapses.  Similarly, each time a non-zero millisecond interval elapses, the same processTimerEvent(...) method is called.  A `timerIndex` value (0 or 1) is provided as an argument to distinguish the two cases.

*Method signature*

`int status(int handle, CSR::Intervals, 8, byte *buf)`


*Return data buffer*

     0  microsecond interval (LSB) (ulong32)
     1  microsecond interval
     2  microsecond interval
     3  microsecond interval (MSB)
     4  millisecond interval (LSB) (ulong32)
     5  millisecond interval
     6  millisecond interval
     7  millisecond interval (MSB)

---
##Control

    int control(int handle, int reg, int count, byte *buf)

Write information to a register (or virtual register) in the device or device driver.  

Common control registers are in the CCR namespace.

---
###`CCR::Reset`

####Reset the device driver state.

###`CCR::Configure`

####Configure a logical unit instance.


---
##Read

    int read(int handle, int count, byte *buf)

The read method does not use any registers, it always returns the bytes available from the input stream (currently) associated with the device.  This stream can be real data from a physical device, or a composed stream created by the driver.

---
##Write

    int write(int handle, int count, byte *buf)

The write method does not use any registers, it always writes the bytes provided to the output stream (currently) associated with the device.  This stream can be real data to a physical device, or a virtual stream used by the driver.

---
##Close

    int close(int handle)

The close method does not use any registers.

