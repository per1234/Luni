March 2016 v0.8 Device Drivers for Arduino and Firmata

##Current Plan

* **Done**.  Use raw I2C to control device from client.
* **Done**.  Write device driver to control device from client (MCP9808).
* **Done**.  Make provisions for multiple command input streams (Firmata and direct call)
* **Done**. Implement board level libraries as Features of type Mode.  I2C.  **No**, remove Mode altogether and fold back in to Features.
* **Done** v0.3.  Write device driver to control device from server (MCP9808).
* **Done** Write server device driver to control virtual device (Hello).
* **Done** Add status register code "get device driver version"
* **Done** v0.8 Delegate dispatchTimers() and addSelectedFeatures() to DeviceTable via Firmata.
* **Done** v0.8 Take interval expiration code out of DeviceTable and put in DeviceDriver
* **Done** v0.8 Add separate DeviceTableDriver object to the driver list during DeviceTable initialization and use it for things like "status() - list of installed drivers"  [implemented in DDMeta]
* **Started** v0.8 Johnny 5 drivers and tests
* **Done** v0.8 Write and test Servo device driver, j-5 client API
* **Done** v0.8 Write and test MCP9808 device driver, j-5 client API
* **Done** v0.8 Generate and receive async messages (MCP9808, Meta)
* **Done** v0.8 extended ACTIONs: Read (continuous fast, continuous slow, once)
* v0.9 Write and test Stepper device driver, client API
* v0.9 Write and test Ping device driver, client API
* v0.9 spec - add discussion of the use of micro and milli interval timers, device action flags
* v0.9 spec - add discussion of the interface between Features and DeviceDrivers.

##Deferred Features and changes.

* **Done** Restore FirmataFeature method names as in ConfigurableFirmata. (tossed the entire CoreFirmata structure and went back to Configurable Firmata as the base)
* **Done** Error value return facility.
* **Done**. Get rid of the double initializaton of cmd in Java message classes.
* Debug info defined by LUN, retrieved with read (?)
* Put tools ram method in Meta

##Interesting, but not planned for implementation

* Wire, SPI, OneWire Mode Ports
* Logging facility as a FirmataFeature.
* Task scheduler as FirmataFeature.

