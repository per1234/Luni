#ifndef DDSensor_h
#define DDSensor_h

#include <Device/DeviceDriver.h>
#include "LUSensor.h"

class DDSensor: public DeviceDriver {

public:
  DDSensor(const char *unitName = "CHAN", int count = 4);

  int open(int opts, int flags, const char *name);
  int read(int handle, int flags, int reg, int count, byte *buf);
  int write(int handle, int flags, int reg, int count, byte *buf);
  int close(int handle, int flags);

  enum class REG : int {
    PIN = 256,
    RESOLUTION = 257,
    MANUF_ID = 258,
    DEVICE_ID = 259
  };

private:
  LUSensor *channels;

};

#endif
