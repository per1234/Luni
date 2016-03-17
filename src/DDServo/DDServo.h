#ifndef DDServo_h
#define DDServo_h

#include <Device/DeviceDriver.h>
#include "LUServo.h"

class DDServo: public DeviceDriver {

public:
  DDServo(const char *unitName = "Servo", int count = 12);

  int open(const char *name, int flags = 0);
  int read(int handle, int reg, int count, byte *buf);
  int write(int handle, int reg, int count, byte *buf);
  int close(int handle);

  enum class REG : int {
    PIN = 0,
    RANGE_MICROSECONDS = 1,
    POSITION_DEGREES = 2,
    POSITION_MICROSECONDS = 3
  };

private:
  DECLARE_SEMVER

  LUServo *servos;
};

#endif
