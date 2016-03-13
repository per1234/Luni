#ifndef ServoDriver_h
#define ServoDriver_h

#include <Device/DeviceDriver.h>
#include "LUServo.h"

class DDServo: public DeviceDriver {

public:
  DDServo(const char *unitName = "Servo", int count = 12);

  int open(const char *name, int flags = 0);
  int read(int handle, int reg, int count, byte *buf);
  int write(int handle, int reg, int count, byte *buf);
  int close(int handle);

    // attach(pin )  - Attaches a servo motor to an i/o pin.
    // attach(pin, min, max  ) - Attaches to a pin setting min and max values in microseconds
    // default min is 544, max is 2400

    // write()     - Sets the servo angle in degrees.  (invalid angle that is valid as pulse in microseconds is treated as microseconds)
    // writeMicroseconds() - Sets the servo pulse width in microseconds
    // read()      - Gets the last written servo pulse width as an angle between 0 and 180.
    // readMicroseconds()   - Gets the last written servo pulse width in microseconds. (was read_us() in first release)
    // attached()  - Returns true if there is a servo attached.
    // detach()    - Stops an attached servos from pulsing its i/o pin.

  enum class REG : int {
    PIN = 0,
    RANGE_MICROSECONDS = 1,
    POSITION_DEGREES = 2,
    POSITION_MICROSECONDS = 3
  };

private:
  DECLARE_SEMVER
};

#endif
