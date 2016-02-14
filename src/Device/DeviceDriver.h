#ifndef DeviceDriver_h
#define DeviceDriver_h

#include <arduino.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include <Framework/ByteOrder.h>
#include <Framework/SemVer.h>
#include "LogicalUnitInfo.h"
#include "DeviceError.h"

// Open() flags

#define DDO_FORCE_OPEN 0x01

// These are the Common Device Register codes used by the DeviceDrivers in
// their status() and control() methods.  Register names specific to a
// particular device type are defined by the individual DeviceDrivers.

enum class CDR : int {
    DriverVersion   = -1,   /* Get driver name and version */
    LibraryVersion  = -2,   /* Get library name and version */
    Reset           = -3,   /* Reset all state in the device driver */
    Configure       = -4,   /* Configure a logical unit number instance */
    Debug           = -256  /* Do something helpful for debugging ... */
};

#define MAX_ROOT_NAME_LENGTH 32

class DeviceDriver {

public:

    DeviceDriver(const char *nameRoot, const int count);

    virtual int open(const char *name, int flags) = 0;
    virtual int status(int handle, int reg, int count, byte *buf) = 0;
    virtual int control(int handle, int reg, int count, byte *buf) = 0;
    virtual int read(int handle, int count, byte *buf) = 0;
    virtual int write(int handle, int count, byte *buf) = 0;
    virtual int close(int handle) = 0;

    /**
     * Called repeatedly by the application program at the expiration of a
     * microsecond based interval to perform real time updates of position and
     * control.
     * @param deltaMicros Length, in microseconds, of the interval since the last call to this
     * method.
     */
    virtual void update(unsigned long deltaMicros);

    /**
     * Called repeatedly by the application program at the expiration of a
     * millisecond based interval to provide an opportunity for reporting and
     * other lower frequency tasks.
     * @param deltaMillis Length, in milliseconds, of the interval since the last call to this method.
     */
    virtual void report(unsigned long deltaMillis);

protected:
    const char *rootName;
    int logicalUnitCount;
    LogicalUnitInfo **logicalUnits;

    int buildVersionResponse(const byte *semver, const char *name,
        const char *prLabel, const char *bLabel, int count, byte *buf);
};

#endif
