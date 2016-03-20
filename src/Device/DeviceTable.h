
#ifndef DeviceTable_h
#define DeviceTable_h

#include <Device/DeviceDriver.h>
#include <Device/ClientReporter.h>

#define MAX_DPB_LENGTH 128  // decoded parameter block length (plain text)

class DeviceTable {

    friend class DDMeta;        // in order to read deviceCount and devices

public:
    DeviceTable(DeviceDriver *deviceArray[], ClientReporter *reporter);
    ~DeviceTable();

    int open(const char *name, int flags = 0);
    int read(int handle, int reg, int count, byte *buf);
    int write(int handle, int reg, int count, byte *buf);
    int close(int handle);

    int dispatchTimers();
    void reset();

private:

    int deviceCount;
    DeviceDriver **devices;

    ClientReporter *cr;             // for use by DDMeta

};

#endif

