#ifndef DDMeta_h
#define DDMeta_h

#include <Device/DeviceDriver.h>
#include <Device/DeviceTable.h>
#include "LUMeta.h"

class DDMeta: public DeviceDriver {

public:

  DDMeta(const char *unitName = "Meta", int count = 1);

  int open(const char *name, int flags = 0);
  int read(int handle, int reg, int count, byte *buf);
  int write(int handle, int reg, int count, byte *buf);
  int close(int handle);

  int processTimerEvent(int lun, int timerSelector, ClientReporter *r);

  enum class REG : int {
    AVG_INTERVALS = 0,
    DRIVER_COUNT = 1,
    DRIVER_VERSION_LIST = 2
  };

private:
  const DeviceTable *theDeviceTable;

  int readATI(int handle, int reg, int count, byte *buf);

  unsigned long calculateAverageInterval(int idx);

};

#endif
