#ifndef ServoDriver_h
#define ServoDriver_h

#include <Device/DeviceDriver.h>
#include "ServoLUI.h"

enum class ServoRegister : int {
  PIN = 0,
  SCALE = 1,
  POSITION = 2
};

class ServoDriver: public DeviceDriver {

public:
    ServoDriver(const char *unitName = "Servo", int count = 12);

    int open(const char *name, int flags = 0);
    int read(int handle, int reg, int count, byte *buf);
    int write(int handle, int reg, int count, byte *buf);
    int close(int handle);

private:
    DECLARE_SEMVER
};

#endif
