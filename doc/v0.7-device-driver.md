

----------
March 2016 V 0.7.0 beta.  Doug Johnson (finson@whidbey.com) 

----------

##Firmata Feature: DeviceFeature and Device Drivers

Proposed for addition in Firmata 2.6 or later.

The purpose of this feature is to facilitate arbitrary additions to Firmata capabilities without requiring a central registration or causing frequent command code conflicts.  The feature is implemented with a new FirmataFeature module `DeviceFeature`, a pair of new Sysex commands (`DEVICE_QUERY` and `DEVICE_RESPONSE`), and the concept of a DeviceDriver abstract class with well defined method signatures.  

In effect, `DeviceFeature` uses Firmata as a remote procedure call mechanism.  

The `DeviceFeature` module receives, decodes, and dispatches incoming `DEVICE_QUERY` messages to the appropriate device driver.  The concrete sub-classes of DeviceDriver implement the various capabilities and make them available to callers through the API documented below.  After a request has been processed by the device driver, the `DeviceFeature` module captures the result, encodes, and sends the outgoing `DEVICE_RESPONSE` messages back to the host.

Also note that any other module on the server can use the device driver API directly to access any device capabilities it might require.  In this case, there is no reformatting, encoding, transmission, or other involvement by Firmata, it's just one module calling another directly.

##Introduction

Device Drivers are designed to allow a client-side application to control remote devices attached to a Firmata server.  This is similar to the function of the existing Firmata command set, but at a somewhat higher level of abstraction.

###Terminology

Some terms with specific meanings for this feature are *device*, *logical unit* and *handle*.

- Device.  A device driver can control one or more instances of a specific *device* type.  The capabilities of the driver are determined by the device type, and are equally available to all the instances of the device. Each driver is given a simple name to identify the device type.  For example, MCP9808 is the name of a device driver for the MCP9808 temperature sensor.  Hello is the name of a virtual device driver that deals with "Hello World" type messaging.
- Logical Unit.  Each instance of a device is a *logical unit*.  A device driver can handle one or more logical units, depending on device capabilities and device driver implementation. Each logical unit is given a name based on the device name.  For example, if there are two MCP9808 boards connected to a single server, then they are referred to as MCP9808:0 and MCP9808:1.
- Handle.  When a logical unit is first opened, a *handle* that uniquely identifies the device and logical unit of interest is returned to the caller for use in future operations.  The handle is valid until the logical unit is closed.

###Device Driver API

The Device Driver API includes four methods documented below.  The API is intended to be implemented by a device driver module on the server side (Firmata micro) exactly as written.  On the client side (client host), the same API calls should be implemented, but there will be small changes dictated by the syntax of the language used for the client.  Client-side proxy device drivers and server-side device drivers always use this API and never compose Firmata messages themselves, instead they rely on the  Firmata library to do that.

In the most common architecture, the device driver implements the main device control code on the server and provides access using the specified API.  A proxy on the client also implements the API signatures, and acts as a bridge to the actual device driver and uses the Device Driver Sysex messages DEVICE\_QUERY and DEVICE\_RESPONSE to control the server side device driver via Firmata, which in turn controls the component(s) using local capabilities.  In this scenario, the server side device driver receives the same calls and parameters as were provided to the proxy on the client.

On the other hand, it is also possible for a device driver author to write the main control code for the client and provide access there using the same API. In this case the client device driver uses existing Firmata Features and commands as necessary to control the remote component(s) directly and according to the data sheet.  In this scenario, the server side Firmata responds to standard Firmata commands as received and there is no specific device driver needed on the server.

####Device Read and Write Registers

The read and write methods operate based on register numbers.  On an actual device, physical register numbers usually start at 0 and max out at a relatively low value like 16 or 32, depending on the device.  This DeviceDriver API uses a 16-bit signed integer to identify the register of interest, so virtual quantities and actions can be implemented in addition to the actual physical device capabilities.

There is a set of common register numbers defined that all device drivers can implement as applicable. All the common register numbers are negative.  Also, each individual device driver can define its own set of register numbers as it needs in order to match hardware registers or define unique virtual registers and actions.  These device specific register numbers should all be non-negative to avoid conflict with the common register numbers.

####Status Return from Methods

Each of the device driver methods returns a signed `int` value to the caller.  If the value is negative, then the call failed and the value is an error code.  If the value is greater than or equal to 0, then the call succeeded.  The meaning of the non-negative value depends on the call.  For example, the `open` method returns a handle for future use and the `read` method returns the number of bytes read.  The details for each method are documented below.

####Firmata Messages

Two Sysex sub-commands are used by this feature: `DEVICE_QUERY` and `DEVICE_RESPONSE`.

There is a small set of action codes that specify what the driver is to do after it receives the message.  

The first action is always `OPEN`.  The caller supplies a logical unit name that can be recognized by a device driver, and upon success, a handle is returned for use in future calls. After the handle has been received, the caller can read status and data (`READ`) and write control and data (`WRITE`).  Once the caller has completed its operations on a device, it can use `CLOSE` to make the logical unit available for another client.

The detailed message formats for each action are provided at the end of this document.

##Method Prototypes

The method prototypes shown below are the primary interface to each Device Driver on the server and, with suitable modifications for language syntax, on the client.

The type identifier `int` is used to indicate a signed integer value of at least 16 bits.  Only the low order 16 bits (the two low order bytes) are significant.  The type identifier `byte` is used to indicate an integer value of at least 8 bits.  Only the low order 8 bits (one byte) are significant.

To the extent practical, the error code values and meanings are taken directly from the Linux/C error codes documented in errno.h and errno-base.h, except that the actual values are negated for use in this application.

All data values visible to the device drivers are encoded to base-64 immediately before transmission by Firmata and decoded to their true values immediately after receipt.  Thus, as far at the device drivers and the clients are concerned, all values are full-width values with no constraints on their magnitude or use of the high order sign bit due to transmission protocols.

---
###Open

    int open(const char* name)
    int open(const char* name, int flags)

**param** (in) `name` Name of the logical unit to open.  UTF-8 encoded, null terminated.

**param** (in) `flags` Flags associated with the open.  Default: 0.

**return** *Success*: The newly assigned handle value.  The handle is used in future calls to indicate the device driver and specific device being addressed.  *Error*: error code.

---
###Read

Read information from a register (or virtual register) in the device or device driver.

The method and its parameters are as follows.

    int read(int handle, int reg, int count, byte *buf)

**param** (in) `handle` The device driver selector value returned by Open in a previous call.

**param** (in) `reg` The register address at which to start reading.

**param** (in) `count` The desired (or maximum) number of bytes to read.

**param** (out) `buf` Pointer to the buffer to receive the data read.  Must be large enough to hold `count` bytes.

**return**  *Success*: The number of bytes actually read.  A short count does not in itself cause an error, since the caller can determine for itself that not everything requested was read which may not actually be an error.  *Error*: error code.

---
###Write

    int control(int handle, int reg, int count, byte *buf)

**param** (in) `handle` The device driver selector value returned by Open in a previous call.

**param** (in) `reg` The register address at which to start writing.

**param** (in) `count` The desired number of bytes to write.

**param** (in) `buf` Pointer to the buffer containing the data to write.

**return**  *Success*: The number of bytes actually written.  Ordinarily, this will be equal to the requested number of bytes to write.  If it is short due to some device error (eg, physical write failure), then the driver will return an error code (eg, `EIO`).  However, under some unique circumstances for some drivers, it may be reasonable for a short count to occur in which case the driver will return the short count and no error code.  *Error*: error code.

---
###Close

    int close(int handle)

**param** (in) `handle` The device driver selector value returned by Open in a previous call.  The selected device driver is responsible for deciding what actions if any are needed to "close" the connection.  After a close, the only valid action on the device is another open.

**return**  Success: 0.  *Error*: error code.

---
##Firmata Message Formats

The arguments provided by the caller of an API method are organized into a `DEVICE_QUERY` message on the client side by the proxy device driver, encoded in base-64, then transmitted to the server.  Firmata dispatches the Sysex message to the DeviceFeature module, which decodes it and dispatches the API call to the proper device driver.  After processing by the device driver, DeviceFeature captures the results and formats them as a `DEVICE_RESPONSE` message, then encodes and transmits the message back to the client host where the proxy device driver decodes the message and returns the result to the original caller.

There are no restrictions on the values seen by the device drivers due to Firmata because those values are all encoded in Base-64 before transmission.

###Message Header

The `DEVICE_QUERY` and `DEVICE_RESPONSE` messages are Firmata Sysex messages.  The first message byte is always START_SYSEX (0xF0).  The second byte is the Sysex command byte, in this case either DEVICE_QUERY (0x30) or DEVICE_RESPONSE (0x31).  This 2-byte header is transmitted as given with no encoding. The body of the message follows the header and is entirely encoded in base-64. The final byte of the message is always an unencoded END_SYSEX (0xF7) command byte.

####DEVICE\_QUERY header

    0 START_SYSEX byte (0xF0).
    1 Sysex command byte DEVICE_QUERY (0x30).

####DEVICE\_RESPONSE header

    0 START_SYSEX byte (0xF0).
    1 Sysex command byte DEVICE_RESPONSE (0x31).

###Message Body

The body of the message includes all the values needed to reconstitute the method call and response.  These values are marshalled into a single buffer by the sending Firmata method immediately before transmission.  The values are unmarshalled by the receiving Firmata method immediately after receipt.  Thus the following layouts only exist for a brief moment after marshall and before encode and send, or after receipt and decode and before unmarshall.  

Character strings are stored on the server in UTF-8.  All eight bits in a UTF-8 byte are significant.  A '0' in the high order bit indicates a character in the first group of 127 characters (the ASCII character set).  A '1' in the high order bit indicates that the byte is part of a multi-byte sequence. Unfortunately, it might also indicate a Firmata control byte.  Encoding in Base-64 avoids this problem.

Remember that all bytes in the body of the message are encoded prior to transmission. The values shown in the layout tables below are **the 8-bit values before or after encoding / decoding**, they are **_not_** the encoded 7-bit quantities that are actually transmitted.

The layouts shown here determine the contents of the bytes visible during transmission, but the actual byte contents will be different due to the base-64 encoding.

**Note**  Since there are 9 raw bytes in the common part of every message body, after encoding and during transmission each message body starts with a 12-byte encoded byte sequence.  The encoded data bytes, if any, follow after that.

####DEVICE\_QUERY body

The raw `DEVICE_QUERY` message body has the following format before encoding.

    0  Device Action, with values as described below.
    1  Flags (OPEN) or handle value (READ, WRITE, CLOSE) (LSB)
    2  Flags (OPEN) or handle value (READ, WRITE, CLOSE) (MSB)
    3  Register (READ, WRITE) (LSB) or 0 (Reserved) (OPEN, CLOSE)
    4  Register (READ, WRITE) (MSB) or 0 (Reserved) (OPEN, CLOSE)
    5  Requested byte count (READ, WRITE) (LSB) or 0 (Reserved) (OPEN, CLOSE)
    6  Requested byte count (READ, WRITE) (MSB) or 0 (Reserved) (OPEN, CLOSE)
    7  0 (Reserved)
    8  0 (Reserved)
    9..n The data bytes supporting the query, if any

####DEVICE\_RESPONSE body

The raw `DEVICE_RESPONSE` message body has the following format before encoding.  

    0 Device Action that was provided in the associated DEVICE_QUERY.
    1 Handle value (READ, WRITE, CLOSE) (LSB) or 0 (Reserved) (OPEN)
    2 Handle value (READ, WRITE, CLOSE) (MSB) or 0 (Reserved) (OPEN)
    3 Register (READ, WRITE) (LSB) or 0 (Reserved) (OPEN, CLOSE)
    4 Register (READ, WRITE) (MSB) or 0 (Reserved) (OPEN, CLOSE)
    5 Count requested (READ, WRITE) (LSB) or 0 (Reserved) (OPEN, CLOSE)
    6 Count requested (READ, WRITE) (MSB) or 0 (Reserved) (OPEN, CLOSE)
    7 Status: Error code (negative), ESUCCESS (0), handle, or actual byte count read or written (LSB)
    8 Status: Error code (negative), ESUCCESS (0), handle, or actual byte count read or written (MSB)
    9..n The data bytes supporting the response, if any

####Device Action codes

These are 8-bit values identifying the device driver method to be invoked.  They are stored in the Firmata `DEVICE_QUERY` and `DEVICE_RESPONSE` message bodies at offset 0.

    OPEN    (0x00)
    READ    (0x01)
    WRITE   (0x02)
    CLOSE   (0x03)

####Flags or Handle

These are 16-bit values, stored in the Firmata `DEVICE_QUERY` and `DEVICE_QUERY` message bodies at offsets 1 and 2.  The values are stored in the device driver and in the client in a single, wider integer variable (`int16_t`, `int32_t`, etc).

    1 flags (LSB)
    2 flags (MSB)

or

    1 handle (LSB)
    2 handle (MSB)

####Register numbers

These are 16-bit signed values that identify the register (either real or virtual) to be read or written.  See the discussion above about the Device Read and Write Registers for more detail.

####Status / Return Value from Methods

Each of the device driver methods returns an `int` value to the caller.  The meaning of the returned `int` varies depending on the method called.  Error return values are negative.  Success return values are either 0 or positive, and can be the ESUCCESS status code, a handle value, or a byte count, depending on the method called.

Note that the byte count returned by the various methods is the *actual* number of bytes read or written by the device driver, it is *not* the length of the encoded message body.  This shouldn't be a problem except as something to remember when debugging and looking at the messages as they are transmitted.

---

##Detailed Device Driver Message Formats

###Device Driver - Open

####_Query_

> **Message Header** (Plain text)
> 
>     0  START_SYSEX (0xF0)
>     1  DEVICE_QUERY (0x30)
>      
> **Message Body** (before encoding)
> 
>     0  0x00 (OPEN)
>     1  Flags (LSB)
>     2  Flags (MSB)
>     3  0 (Reserved)
>     4  0 (Reserved)
>     5  0 (Reserved)
>     6  0 (Reserved)
>     7  0 (Reserved)
>     8  0 (Reserved)
>     9..n name string (UTF-8, null terminated)
> 
> **Message End** (Plain text)
>  
>     k  END_SYSEX (0XF7)
> 

####_Response_

> **Message Header** (Plain text)
> 
>     0  START_SYSEX (0xF0)
>     1  DEVICE_RESPONSE (0x31)
>
> **Message Body** (before encoding)
>
>     0  0x00 (OPEN)
>     1  0 (Reserved)
>     2  0 (Reserved)
>     3  0 (Reserved)
>     4  0 (Reserved)
>     5  0 (Reserved)
>     6  0 (Reserved)
>     7  Returned handle or error code (LSB)
>     8  Returned handle or error code (MSB)
> 
> **Message End** (Plain text)
>  
>     k  END_SYSEX (0XF7)

---
###Device Driver - Read

####_Query_

> **Message Header** (Plain text)
> 
>     0  START_SYSEX (0xF0)
>     1  DEVICE_QUERY (0x30)
> 
> **Message Body** (before encoding)
> 
>     0  0x01 (READ)
>     1  Handle (LSB)
>     2  Handle (MSB)
>     3  Register (LSB)
>     4  Register (MSB)
>     5  Number of bytes to read (LSB)
>     6  Number of bytes to read (MSB)
>     7  0 (Reserved)
>     8  0 (Reserved)
> 
> **Message End** (Plain text)
>  
>     k  END_SYSEX (0XF7)
> 
####_Response_

> **Message Header** (Plain text)
> 
>     0  START_SYSEX (0xF0)
>     1  DEVICE_RESPONSE (0x31)
> 
> **Message Body** (before encoding)

>     0  0x01 (READ)
>     1  Handle (LSB)
>     2  Handle (MSB)
>     3  Register (LSB)
>     4  Register (MSB)
>     5  Number of bytes to read (LSB)
>     6  Number of bytes to read (MSB)
>     7  Number of bytes actually read or error code (LSB)
>     8  Number of bytes actually read or error code (MSB)
>     9..n The requested data bytes, if any
>
>**Message End** (Plain text)
>
>     k  END_SYSEX (0XF7)

---
###Device Driver - Write
####_Query_

> **Message Header** (Plain text)
> 
>     0  START_SYSEX (0xF0)
>     1  DEVICE_QUERY (0x30)
> 
> **Message Body** (before encoding)
> 
>     0  0x02 (WRITE)
>     1  Handle (LSB)
>     2  Handle (MSB)
>     3  Register (LSB)
>     4  Register (MSB)
>     5  Number of bytes to write (LSB)
>     6  Number of bytes to write (MSB)
>     7  0 (Reserved)
>     8  0 (Reserved)
>     9..n The data bytes to write, if any
>
> **Message End** (Plain text)
>  
>     k  END_SYSEX (0XF7)
> 
####_Response_

> **Message Header** (Plain text)
> 
>     0  START_SYSEX (0xF0)
>     1  DEVICE_RESPONSE (0x31)
> 
> **Message Body** (before encoding)
> 
>     0  0x02 (WRITE)
>     1  Handle (LSB)
>     2  Handle (MSB)
>     3  Register (LSB)
>     4  Register (MSB)
>     5  Number of bytes to write (LSB)
>     6  Number of bytes to write (MSB)
>     7  Number of bytes actually written or error code (LSB)
>     8  Number of bytes actually written or error code (MSB)
>
>**Message End** (Plain text)
>
>     k  END_SYSEX (0XF7)

---
###Device Driver - Close

####_Query_

> **Message Header** (Plain text)
> 
>     0  START_SYSEX (0xF0)
>     1  DEVICE_QUERY (0x30)
>      
> **Message Body** (before encoding)
> 
>     0  0x03 (CLOSE)
>     1  Handle (LSB)
>     2  Handle (MSB)
>     3  0 (Reserved)
>     4  0 (Reserved)
>     5  0 (Reserved)
>     6  0 (Reserved)
>     7  0 (Reserved)
>     8  0 (Reserved)
> 
> **Message End** (Plain text)
>  
>     k  END_SYSEX (0XF7)
> 

####_Response_

> **Message Header** (Plain text)
> 
>     0  START_SYSEX (0xF0)
>     1  DEVICE_RESPONSE (0x31)
>
> **Message Body** (before encoding)
>
>     0  0x03 (CLOSE)
>     1  Handle (LSB)
>     2  Handle (MSB)
>     3  0 (Reserved)
>     4  0 (Reserved)
>     5  0 (Reserved)
>     6  0 (Reserved)
>     7  ESUCCESS or error code (LSB)
>     8  ESUCCESS or error code (MSB)
> 
> **Message End** (Plain text)
>  
>     k  END_SYSEX (0XF7)
