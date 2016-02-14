#ifndef MCP9808Driver_h
#define MCP9808Driver_h

#include <Silicon/I2CPort.h>
#include <Device/DeviceDriver.h>
#include "MCP9808LUI.h"
#include "MCP9808.h"
class MCP9808Driver: public DeviceDriver {

public:
    MCP9808Driver(const char *unitName = "TC", int count = 8, int base = 0x18);

    int open(const char *name, int flags = 0);
    int status(int handle, int reg, int count, byte *buf);
    int control(int handle, int reg, int count, byte *buf);
    int read(int handle, int count, byte *buf);
    int write(int handle, int count, byte *buf);
    int close(int handle);

private:
    DECLARE_SEMVER

    int statusCDR_Debug(int handle, int reg, int count, byte *buf);

    int baseAddress;
    I2CPort i2c;

};

#endif
