
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

  int open(const char *name, int flags = 0);
  int read(int handle, int reg, int count, byte *buf);
  int write(int handle, int reg, int count, byte *buf);
  int close(int handle);

  int processTimerEvent(int lun, int timerIndex, ClientReporter *r);

private:
  DECLARE_SEMVER

  int writeConfigure(int handle, int reg, int count, byte *buf);
  int writeMoveRelative(int handle, int reg, int count, byte *buf);
  int writeRPMSpeeds(int handle, int reg, int count, byte *buf);
  int readRPMSpeeds(int handle, int reg, int count, byte *buf);
  int readPositionEvent(int handle, int reg, int count, byte *buf);

};

#endif
