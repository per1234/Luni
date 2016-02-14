#ifndef PeekDriver_h
#define PeekDriver_h

#include <Device/DeviceDriver.h>
#include "PeekLUI.h"
#include "Peek.h"

#define SAMPLE_COUNT 16

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

    void report(unsigned long milliDelta);
    void update(unsigned long microDelta);

private:
    DECLARE_SEMVER

    int statusCDR_Debug(int handle, int reg, int count, byte *buf);

    unsigned long calculateAverageInterval();

    unsigned long previousTime[2];   // the time the last interval expired
    unsigned long currentTime[2];    // the current values from millis() and micros()
    unsigned long samples[SAMPLE_COUNT+1];
    int sampleIndex;
    bool isSampleBufferFull;

};

#endif
