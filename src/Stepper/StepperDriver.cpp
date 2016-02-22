#include "StepperDriver.h"
#include "StepperDriverLUI.h"

DEFINE_SEMVER(StepperDriver, 0, 1, 0)

/**
 * This class defines a stepper motor device driver using the asynchronous
 * FirmataStepper library that is standard with Firmata.  The library was
 * renamed AsyncStepper for this use but otherwise it is the exact same code
 * as comes standard with Firmata.
 *
 * These are the AsyncStepper library methods available to us for this
 * implementation:
 *
 * AsyncStepper(byte interface, int steps_per_rev, byte pin1, byte pin2,
 *   byte pin3, byte pin4);
 * void setStepsToMove(long steps_to_move, int speed, int accel = 0, int decel = 0);
 * bool update();
 * byte version(void);
 */

//---------------------------------------------------------------------------

StepperDriver::StepperDriver(const char *dName, int addrCount) :
  DeviceDriver(dName, addrCount) {
}

//---------------------------------------------------------------------------

int StepperDriver::open(const char *name, int flags) {
  int lun;
  int status = DeviceDriver::open(name, flags);
  if (status < 0) {
    return status;
  }

  lun = status;
  logicalUnits[lun] = new StepperDriverLUI();
  if (logicalUnits[lun] == 0) {
    return ENOMEM;
  }
  return lun;
}

//---------------------------------------------------------------------------

/**
 * Read a status register on the device.
 */
int StepperDriver::status(int handle, int reg, int count, byte *buf) {

  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {
  case static_cast<int>(CSR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
           preReleaseLabel, buildLabel, count, buf);

  default:
    return ENOTSUP;
  }

  return EPANIC;
}

int StepperDriver::control(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);
  if (currentUnit == 0) {
    return ENOTCONN;
  }

  switch (reg) {
  case static_cast<int>(CCR::Configure):
    return controlCCR_Configure(handle, reg, count, buf);

  case static_cast<int>(StepperRegister::MoveRelative):
    return controlSTP_MoveR(handle, reg, count, buf);

  case static_cast<int>(StepperRegister::RPMSpeed):
    return controlSTP_RPMSpeed(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

int StepperDriver::controlCCR_Configure(int handle, int reg, int count, byte *buf) {
  int interface, stepCount, pin1, pin2, pin3, pin4;
  AsyncStepper *motor;

  if (count < 7) return EMSGSIZE;

  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);

  AsyncStepper *deviceObject = currentUnit->getDeviceObject();
  if (deviceObject != 0) return EBUSY;

  interface = from8LEToHost(buf);
  stepCount = from16LEToHost(&buf[1]);
  pin1 = from16LEToHost(&buf[3]);
  pin2 = from16LEToHost(&buf[5]);

  if (interface == static_cast<int>(StepperInterface::EZStepper)) {
    return ENOTSUP;
  } else if (interface == static_cast<int>(StepperInterface::TWO_WIRE)) {
    if (count == 7) {
      motor = new AsyncStepper(AsyncStepper::TWO_WIRE, stepCount, pin1, pin2);
    } else {
      return EMSGSIZE;
    }
  } else if (interface == static_cast<int>(StepperInterface::FOUR_WIRE)) {
    if (count == 11) {
      pin3 = from16LEToHost(&buf[7]);
      pin4 = from16LEToHost(&buf[9]);
      motor = new AsyncStepper(AsyncStepper::FOUR_WIRE, stepCount, pin1, pin2, pin3, pin4);
    } else {
      return EMSGSIZE;
    }
  } else {
    return ENOTSUP;
  }
  currentUnit->setDeviceObject(motor);
  currentUnit->setRPMSpeed(0);
  currentUnit->setRPMMax(0);
  currentUnit->setRPMA(0);
  currentUnit->setRPMD(0);

  return count;
}

int StepperDriver::controlSTP_RPMSpeed(int handle, int reg, int count, byte *buf) {
  AsyncStepper *motor;
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);
  AsyncStepper *deviceObject = static_cast<AsyncStepper *>(currentUnit->getDeviceObject());
  if (deviceObject == 0) return EBADSLT;
  if (count != 4) return EMSGSIZE;

  currentUnit->setRPMSpeed(from32LEToHost(buf));
  return count;
}

int StepperDriver::controlSTP_MoveR(int handle, int reg, int count, byte *buf) {
  AsyncStepper *motor;
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);
  AsyncStepper *deviceObject = static_cast<AsyncStepper *>(currentUnit->getDeviceObject());
  if (deviceObject == 0) return EBADSLT;
  if (count != 5) return EMSGSIZE;

  deviceObject->setStepsToMove((int)from32LEToHost(buf), currentUnit->getRPMSpeed(),
                               currentUnit->getRPMA(), currentUnit->getRPMD());
  return count;
}

//---------------------------------------------------------------------------

int StepperDriver::read(int handle, int count, byte *buf) {
  return ENOSYS;
}

int StepperDriver::write(int handle, int count, byte *buf) {
  return ENOSYS;
}

int StepperDriver::close(int handle) {
  return DeviceDriver::close(handle);
}

//---------------------------------------------------------------------------

int StepperDriver::microTimer(unsigned long deltaMicros, ClientReporter *r) {
  return ESUCCESS;
}

int StepperDriver::milliTimer(unsigned long deltaMillis, ClientReporter *r) {
  return ESUCCESS ;
}
