#ifndef DeviceDriver_h
#define DeviceDriver_h

#include <arduino.h>
#include <stdlib.h>
#include <string.h>
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

// Device action flags.  There is only one set of flags shared among all the
// action methods open(), read(), write(), and close().  Presumably the usage
// of each flag will be similar across the methods, but there is no requirement
// that it be exactly the same in each method.  Similarly, the same numeric
// value can be used for entirely different meanings in the different methods.
// In the latter case, the flags should have different names for clarity, even
// though the actual numeric values are the same.

enum class DAF : int {
    NONE        = 0x0,
    FORCE       = 0x1,
    LOW_RATE    = 0xD,
    HIGH_RATE   = 0xE,
    DUAL_RATE   = 0xF
};

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
// register values.  The range -256..-32768 is reserved for future allocation
// and should not be used at all.
//
// NOTE:  The range of values from 0..255 is reserved for identifying actual
// physical registers on a device.  Any virtual register numbers defined by a
// DeviceDriver must be greater than 255 to avoid conflict with the
// physical registers.

enum class CDR : int {
    Reset           = -1,   /* Reset all state in the device driver for the specified unit number */
    DriverVersion   = -2,   /* Get driver name and version */
    LibraryVersion  = -3,   /* Get library name and version */
    UnitNamePrefix  = -4,   /* Get/set the name with which the units of this device are opened */
    Configure       = -5,   /* Get/set configuration of a logical unit number instance */
    Intervals       = -6,   /* Get/set current timer intervals for this device */
    Stream          = -7,   /* Read or write bytes using the "primary" data stream source or sink */
    Manufacturer    = -8,   /* Get the manufacturer id associated with the device (PCiSIG, ?, ?) */
    DeviceID        = -9,   /* Unique ID for the component attached to the open unit number */
    Debug           = -255  /* Do something helpful for debugging ... */
};

#define MAX_UNIT_NAME_LENGTH 32

#define MINIMUM_UPDATE_INTERVAL 100    // microseconds
#define DEFAULT_UPDATE_INTERVAL 200

#define MINIMUM_REPORT_INTERVAL 10     // milliseconds
#define DEFAULT_REPORT_INTERVAL 19

class DeviceDriver {

    friend class DeviceTable;       // in order to set deviceNumber

public:

    DeviceDriver(const char *nameRoot, const int count);

    virtual int open(int opts, int flags, const char *name) = 0;
    virtual int read(int handle, int flags, int reg, int count, byte *buf) = 0;
    virtual int write(int handle, int flags, int reg, int count, byte *buf) = 0;
    virtual int close(int handle, int flags) = 0;

    virtual int processTimerEvent(int lun, int timerSelector, ClientReporter *r);
    virtual void reset();

    virtual int checkForTimerEvents(ClientReporter *r) final;

protected:
    DECLARE_VERSION

    char *unitNamePrefix;

    int buildReadPrefixResponse(int count, byte *buf);
    int buildWritePrefixResponse(int count, const byte *newPrefix);

    int logicalUnitCount;
    LogicalUnitInfo **logicalUnits;

    int deviceNumber;        // the major handle value, ie index in the DeviceTable

    int writeIntervals(int handle, int flags, int reg, int count, byte *buf);
    int readIntervals(int handle, int flags, int reg, int count, byte *buf);
    int buildVersionResponse(int count, byte *buf);

private:

    unsigned long calculateElapsedTime(LogicalUnitInfo *lui, int timerIndex);

};

#endif
