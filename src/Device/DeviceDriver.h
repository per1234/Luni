#ifndef DeviceDriver_h
#define DeviceDriver_h

#include <arduino.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <limits.h>

#include <Framework/ByteOrder.h>
#include <Framework/SemVer.h>
#include "ClientReporter.h"
#include "LogicalUnitInfo.h"
#include "DeviceError.h"

// Combine device number and unit number to get full handle

#define makeHandle(DEVICE_NUMBER, UNIT_NUMBER) ((int)(((DEVICE_NUMBER & 0x7F) << 8) | (UNIT_NUMBER & 0x7F)))

// Extract 7-bit index values from a 16-bit handle

#define getUnitNumber(HANDLE) ((int)((HANDLE) & 0x7F))
#define getDeviceNumber(HANDLE) ((int)(((HANDLE) >> 8) & 0x7F))

// Open() flags

#define DDO_FORCE_OPEN 0x01

// These are the common register codes used by the DeviceDrivers in
// their read() (CSR) and write() (CCR) methods.  Register names specific
// to a particular device type are defined by the individual DeviceDrivers.
//
// NOTE: Since a device driver knows whether it is performing a read()
// call or a write() call independent of the register number, it is okay
// if the CSR and CCR numeric values and/or names overlap as they will
// not cause a conflict.
//
// NOTE:  The range of values -1..-128 is reserved for the common virtual
// register values.  Any virtual registers defined by a DeviceDriver must
// fall in the range 0..128 to avoid conflict with the common registers.
// Obviously, any non-negative virtual register number must also avoid
// conflict with the actual hardware device register numbers.

enum class CSR : int {
    DriverVersion   = -1,   /* Get driver name and version */
    LibraryVersion  = -2,   /* Get library name and version */
    Configure       = -3,   /* Get configuration of a logical unit number instance */
    Intervals       = -4,   /* Get current timer intervals for this device */
    Stream          = -5,   /* Read (or compose) bytes from the incoming data stream */
    Debug           = -128  /* Do something helpful for debugging ... */
};

enum class CCR : int {
    Reset           = -1,   /* Reset all state in the device driver */
    Configure       = -3,   /* Set configuration of a logical unit number instance */
    Intervals       = -4,   /* Set current timer intervals for this device */
    Stream          = -5,   /* Write bytes to the outgoing character stream */
    Debug           = -128  /* Do something helpful for debugging ... */
};

#define MAX_ROOT_NAME_LENGTH 32

#define MINIMUM_UPDATE_INTERVAL 100    // microseconds
#define DEFAULT_UPDATE_INTERVAL 200

#define MINIMUM_REPORT_INTERVAL 10     // milliseconds
#define DEFAULT_REPORT_INTERVAL 19


class DeviceDriver {

    friend class DeviceTable;

public:

    DeviceDriver(const char *nameRoot, const int count);

    virtual int open(const char *name, int flags) = 0;
    virtual int read(int handle, int reg, int count, byte *buf) = 0;
    virtual int write(int handle, int reg, int count, byte *buf) = 0;
    virtual int close(int handle) = 0;

    virtual int processTimerEvent(int lun, int timerSelector, ClientReporter *r);

    int checkForTimerEvents(ClientReporter *r);

protected:

    const char *rootName;

    int logicalUnitCount;
    LogicalUnitInfo **logicalUnits;
    int deviceNumber;        // the major handle value, ie index in the DeviceTable

    int buildVersionResponse(const byte *semver, const char *name,
                             const char *prLabel, const char *bLabel, int count, byte *buf);

    int writeIntervals(int handle, int reg, int count, byte *buf);
    int readIntervals(int handle, int reg, int count, byte *buf);

private:

    unsigned long calculateElapsedTime(LogicalUnitInfo *lui, int timerIndex);

};

#endif
