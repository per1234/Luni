#ifndef TableDriver_h
#define TableDriver_h

#include <Device/DeviceDriver.h>
#include "DeviceTable.h"
#include "TableDriver.h"
#include "TableLUI.h"

class TableDriver: public DeviceDriver {

public:
    TableDriver(const DeviceTable *dt, const char *unitName = "DeviceTable", int count = 1);
    ~TableDriver();

    int open(const char *name, int flags = 0);
    int status(int handle, int reg, int count, byte *buf);
    int control(int handle, int reg, int count, byte *buf);
    int read(int handle, int count, byte *buf);
    int write(int handle, int count, byte *buf);
    int close(int handle);

private:
    DECLARE_SEMVER

    const DeviceTable *theDeviceTable;
};

#endif
