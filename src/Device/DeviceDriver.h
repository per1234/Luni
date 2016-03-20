#ifndef DeviceDriver_h
#define DeviceDriver_h

#include <arduino.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <limits.h>

#include <Framework/ByteOrder.h>
#include <Framework/SemVer.h>
#include <Framework/Boards.h>
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

// These are the common register identifiers used by the DeviceDrivers in their
// read() and write() methods.  Register identifiers specific to a particular
// device type are defined by the individual DeviceDrivers.
//
// Register identifiers are 16-bit signed integers.
//
// There is no requirement that each DeviceDriver implement every common
// register.
//
// NOTE:  The range of values -1..-255 is reserved for the common virtual
// register values.  Any virtual registers defined by a DeviceDriver must
// fall outside of this range to avoid conflict with the common registers.
// Obviously, any non-negative virtual register number must also avoid
// conflict with the actual hardware device register numbers.

enum class CDR : int {
    Reset           = -1,   /* Reset all state in the device driver */
    DriverVersion   = -2,   /* Get driver name and version */
    LibraryVersion  = -3,   /* Get library name and version */
    Configure       = -4,   /* Get/set configuration of a logical unit number instance */
    Intervals       = -5,   /* Get/set current timer intervals for this device */
    Stream          = -6,   /* Read or write bytes using the "primary" data stream source or sink */
    Debug           = -255  /* Do something helpful for debugging ... */
};

#define MAX_ROOT_NAME_LENGTH 32

#define MINIMUM_UPDATE_INTERVAL 100    // microseconds
#define DEFAULT_UPDATE_INTERVAL 200

#define MINIMUM_REPORT_INTERVAL 10     // milliseconds
#define DEFAULT_REPORT_INTERVAL 19

class DeviceDriver {

    friend class DeviceTable;       // in order to set deviceNumber

public:

    DeviceDriver(const char *nameRoot, const int count);

    virtual int open(const char *name, int flags) = 0;
    virtual int read(int handle, int reg, int count, byte *buf) = 0;
    virtual int write(int handle, int reg, int count, byte *buf) = 0;
    virtual int close(int handle) = 0;

    virtual int processTimerEvent(int lun, int timerSelector, ClientReporter *r);
    virtual void reset();

    virtual int checkForTimerEvents(ClientReporter *r) final;

protected:

    const char *rootName;

    int logicalUnitCount;
    LogicalUnitInfo **logicalUnits;

    int deviceNumber;        // the major handle value, ie index in the DeviceTable

    int writeIntervals(int handle, int reg, int count, byte *buf);
    int readIntervals(int handle, int reg, int count, byte *buf);
    int buildVersionResponse(const byte *semver, const char *name,
                             const char *prLabel, const char *bLabel, int count, byte *buf);

private:

    unsigned long calculateElapsedTime(LogicalUnitInfo *lui, int timerIndex);

};

#endif
