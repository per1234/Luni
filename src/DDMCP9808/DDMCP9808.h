#ifndef DDMCP9808_h
#define DDMCP9808_h

#include <Silicon/I2CPort.h>
#include <Device/DeviceDriver.h>
#include "LUMCP9808.h"

class DDMCP9808: public DeviceDriver {

public:
  DDMCP9808(const char *unitName = "TC", int count = 8, int base = 0x18);

  int open(const char *name, int flags = 0);
  int read(int handle, int reg, int count, byte *buf);
  int write(int handle, int reg, int count, byte *buf);
  int close(int handle);

  enum class REG : int {
    RESERVED = 0,
    CONFIG = 1,
    UPPER_TEMP = 2,
    LOWER_TEMP = 3,
    CRIT_TEMP = 4,
    AMBIENT_TEMP = 5,
    MANUF_ID = 6,
    DEVICE_ID = 7,
    RESOLUTION = 8
  };

private:
  DECLARE_SEMVER

  int baseAddress;
  I2CPort i2c;

};

#endif
