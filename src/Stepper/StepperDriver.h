
#ifndef StepperDriver_h
#define StepperDriver_h

#include <Device/DeviceDriver.h>
#include <Stepper/Stepper.h>
#include <Framework/ByteOrder.h>
#include <AsyncStepper.h>

/**
 * This class defines a stepper motor device driver using the asynchronous
 * FirmataStepper library that is standard with Firmata.  The library was
 * renamed AsyncStepper for this use but otherwise it is the exact same code
 * as comes standard with Firmata.
 */
class StepperDriver: public DeviceDriver {

public:

  StepperDriver(const char *dName, int addrCount);

  virtual int open(const char *name, int flags = 0);
  virtual int status(int handle, int reg, int count, byte *buf);
  virtual int control(int handle, int reg, int count, byte *buf);
  virtual int read(int handle, int count, byte *buf);
  virtual int write(int handle, int count, byte *buf);
  virtual int close(int handle);

  int millisecondTimeBase();
  int microsecondTimeBase();

private:
  DECLARE_SEMVER

  int controlCCR_Configure(int handle, int reg, int count, byte *buf);
  int controlSTP_MoveR(int handle, int reg, int count, byte *buf);
  int controlSTP_RPMSpeed(int handle, int reg, int count, byte *buf);

};

    // void setRPMSpeed(int s);
    // void setRPMAccelerations(int a, int d);

    // int getRPMSpeed();
    // int getRPMAcceleration();
    // int getRPMDeceleration();
    //
// private:
//     int theSpeed;
//     int theAcceleration;
//     int theDeceleration;

#endif
