#ifndef DDPeek_h
#define DDPeek_h

#include <Device/DeviceDriver.h>
#include "LUPeek.h"

/**
 * This DDPeek class is an administrative and development tool to
 * provide code analysis capabilities and a place to perform timing
 * tests and the like.
 */
class DDPeek: public DeviceDriver {

public:

  DDPeek(const char *unitName = "PEEK", int count = 1);

  int open(const char *name, int flags = 0);
  int read(int handle, int reg, int count, byte *buf);
  int write(int handle, int reg, int count, byte *buf);
  int close(int handle);

  int processTimerEvent(int lun, int timerSelector, ClientReporter *r);

  enum class PeekRegister : int {
    DRIVER_COUNT = 0,
    INSTALLED_DRIVERS = 1,
    AVG_INTERVALS = 10
  };

private:
  DECLARE_SEMVER

  int readATI(int handle, int reg, int count, byte *buf);

  unsigned long calculateAverageInterval(int idx);

};

#endif
