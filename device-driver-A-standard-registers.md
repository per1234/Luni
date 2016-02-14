

----------
*tbd* 2016 V 0.2.0 beta.  Doug Johnson (finson@whidbey.com) 

----------

## DeviceFeature and Device Drivers - Appendix A: Standard Virtual Registers

The document device-driver.md describes the overall device feature proposal and most parts of the detailed design.

One aspect of the design is that device drivers can define virtual register numbers to identify actions that the device driver supports beyond those that can be implemented simply with access to a single hardware register.

This document defines the virtual registers that are available for every device.  Addition virtual registers may be defined by each device driver according to its need.

####Device Status and Control Registers

The status and control methods operate based on register numbers.  On an actual device, physical register numbers usually start at 0 and max out at a relatively low value like 16 or 32, depending on the device.  This DeviceDriver API uses a full 16-bit signed integer to identify the register of interest, so virtual quantities and actions can be implemented in addition to the actual physical device capabilities.

###Standard Virtual Registers

The `reg` parameter is always a signed 16-bit integer value.  Physical device register numbers and device-specific virtual register numbers are positive.  The standard virtual register numbers defined below are negative.

---
###Open

    int open(const char* name)
    int open(const char* name, int flags)

The open method does not use any registers.

---
###Status

    int status(int handle, int reg, int count, byte *buf)

Read information from a register (or virtual register) in the device or device driver.  

Standard virtual registers are:

- VR_Version
- VR_DeviceName
- VR_UnitName
- VR_UnitCount
- VR_Debug


---
###Control

    int control(int handle, int reg, int count, byte *buf)

Write information to a register (or virtual register) in the device or device driver.  

Standard virtual registers are:

- VR_Reset

---
###Read

    int read(int handle, int count, byte *buf)

The read method does not use any registers, it always returns the bytes available from the input stream (currently) associated with the device.  This stream can be real data from a physical device, or a composed stream created by the driver.

---
###Write

    int write(int handle, int count, byte *buf)

The write method does not use any registers, it always writes the bytes provided to the output stream (currently) associated with the device.  This stream can be real data to a physical device, or a virtual stream used by the driver.

---
###Close

    int close(int handle)

The close method does not use any registers.

