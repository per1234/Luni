#ifndef HelloDriver_h
#define HelloDriver_h

#include <Device/DeviceDriver.h>
#include "Hello.h"
#include "HelloLUI.h"

/**
 * This class defines a simple device driver as an example
 * HelloWorld component for device drivers and their usage.
 */
class HelloDriver: public DeviceDriver {

public:

    HelloDriver(const char *unitName = "HELLO", int count = 1);

    int open(const char *name, int flags = 0);
    int status(int handle, int reg, int count, byte *buf);
    int control(int handle, int reg, int count, byte *buf);
    int read(int handle, int count, byte *buf);
    int write(int handle, int count, byte *buf);
    int close(int handle);

private:
    DECLARE_SEMVER
    int statusCDR_Debug(int handle, int reg, int count, byte *buf);
    int controlCDR_Debug(int handle, int reg, int count, byte *buf);

};

#endif
