#ifndef DDHello_h
#define DDHello_h

#include <Device/DeviceDriver.h>
#include "LUHello.h"

#define MAX_HELLO_TEXT_LENGTH 31


/**
 * This class defines a simple device driver as an example
 * HelloWorld component for device drivers and their usage.
 */
class DDHello: public DeviceDriver {

public:

    DDHello(const char *unitName = "HELLO", int count = 1);

    int open(const char *name, int flags = 0);
    int read(int handle, int reg, int count, byte *buf);
    int write(int handle, int reg, int count, byte *buf);
    int close(int handle);

enum class REG : int {
  INTERJECTION = 0,
  OBJECT = 1,
};

private:
    DECLARE_SEMVER

};

#endif
