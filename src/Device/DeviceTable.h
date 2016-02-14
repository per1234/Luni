
#ifndef DeviceTable_h
#define DeviceTable_h

#include <Device/DeviceDriver.h>
#include <limits.h>

#define MINIMUM_REPORT_INTERVAL 10     // milliseconds
#define DEFAULT_REPORT_INTERVAL 19

#define MINIMUM_UPDATE_INTERVAL 100    // microseconds
#define DEFAULT_UPDATE_INTERVAL 200

#define MAX_DPB_LENGTH 128  // decoded parameter block length (plain text)

class DeviceTable {

    friend class TableDriver;

public:
    DeviceTable(DeviceDriver *deviceArray[], const char*luRootName=0);
    ~DeviceTable();

    int open(const char *name, int flags = 0);
    int status(int handle, int reg, int count, byte *buf);
    int control(int handle, int reg, int count, byte *buf);
    int read(int handle, int count, byte *buf);
    int write(int handle, int count, byte *buf);
    int close(int handle);

    void dispatchTimers();
    void update(unsigned long deltaMicros);
    void report(unsigned long deltaMillis);

private:
    DECLARE_SEMVER

    int deviceCount;
    DeviceDriver **devices;

    unsigned long previousTime[2];   // the time the last interval expired
    unsigned long currentTime[2];    // the current values from millis() and micros()
    unsigned long intervalTime[2];   // length of time between calls to update() and report()

};

#endif

