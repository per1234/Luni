#ifndef PeekDriver_h
#define PeekDriver_h

#include <Device/DeviceDriver.h>
#include "PeekLUI.h"
#include "Peek.h"

/**
 * This PeekDriver class is an administrative and development tool to
 * provide code analysis capabilities and a place to perform timing
 * tests and the like.
 */
class PeekDriver: public DeviceDriver {

public:

    PeekDriver(const char *unitName = "PEEK", int count = 1);

    int open(const char *name, int flags = 0);
    int status(int handle, int reg, int count, byte *buf);
    int control(int handle, int reg, int count, byte *buf);
    int read(int handle, int count, byte *buf);
    int write(int handle, int count, byte *buf);
    int close(int handle);

    int processTimerEvent(int lun, int timerSelector, ClientReporter *r);

private:
    DECLARE_SEMVER

    int statusATI(int handle, int reg, int count, byte *buf);

    unsigned long calculateAverageInterval(int idx);

};

#endif
