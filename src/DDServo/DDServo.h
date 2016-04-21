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
    PIN = 256,
    RANGE_MICROSECONDS = 257,
    POSITION_DEGREES = 258,
    POSITION_MICROSECONDS = 259
  };

private:
  LUServo *servos;
};

#endif
