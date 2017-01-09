#ifndef DDMotor_h
#define DDMotor_h

#include <Device/DeviceDriver.h>
#include <Device/DeviceTable.h>
#include "LUMotor.h"

/**
 * @ingroup DDImplementation
 */
class DDMotor: public DeviceDriver {

public:
  DDMotor(const char *unitName = "Motor", int count = 2);

  int open(int opts, int flags, const char *name);
  int read(int handle, int flags, int reg, int count, byte *buf);
  int write(int handle, int flags, int reg, int count, byte *buf);
  int close(int handle, int flags);

  // device register numbers

  enum class REG : int {
    PINS = 256,
    SPEED = 257,
    DIRECTION = 258
  };

  // other device constants

  enum class DC : int {
    MAX_MOTORS = 2;
  }

private:
  bool enable;
  LUMotor motor[MAX_MOTORS];
};

#endif
