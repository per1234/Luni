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

  case static_cast<int>(Stepper::RPMSpeeds):
    return statusRPMSpeeds(handle, reg, count, buf);

  case static_cast<int>(Stepper::AtPosition):
    return statusAtPosition(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }

  return EPANIC;
}

//---------------------------------------------------------------------------

int StepperDriver::control(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {
  case static_cast<int>(CCR::Configure):
    return controlConfigure(handle, reg, count, buf);

  case static_cast<int>(Stepper::MoveRelative):
    return controlMoveRelative(handle, reg, count, buf);

  case static_cast<int>(Stepper::RPMSpeeds):
    return controlRPMSpeeds(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
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

int StepperDriver::statusRPMSpeeds(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  AsyncStepper *motor = currentUnit->getDeviceObject();
  if (motor == 0) return EBADFD;
  if (count != 16) return EMSGSIZE;

  fromHostTo32LE(currentUnit->speedRPM, &buf[0]);
  fromHostTo32LE(currentUnit->accelRPM, &buf[4]);
  fromHostTo32LE(currentUnit->decelRPM, &buf[8]);
  fromHostTo32LE(currentUnit->speedMaxRPM, &buf[12]);
  return 16;
}

//---------------------------------------------------------------------------

int StepperDriver::statusAtPosition(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  AsyncStepper *motor = currentUnit->getDeviceObject();
  if (motor == 0) return EBADFD;
  if (count < 2) return EMSGSIZE;

  bool stopped = motor->update();
  buf[0] = (byte)(stopped);
  buf[1] = (byte)(false);
  return 2;
}

//---------------------------------------------------------------------------

int StepperDriver::controlConfigure(int handle, int reg, int count, byte *buf) {
  int interface, stepCount, pin1, pin2, pin3, pin4;
  AsyncStepper *motor;

  if (count < 7) return EMSGSIZE;

  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  AsyncStepper *deviceObject = currentUnit->getDeviceObject();
  if (deviceObject == 0) return EBADFD;

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
  currentUnit->speedRPM = 0;
  currentUnit->speedMaxRPM = 0;
  currentUnit->accelRPM = 0;
  currentUnit->decelRPM = 0;

  return count;
}

int StepperDriver::controlRPMSpeeds(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  AsyncStepper *motor = static_cast<AsyncStepper *>(currentUnit->getDeviceObject());
  if (motor == 0) return EBADSLT;
  if (count != 16) return EMSGSIZE;

  currentUnit->speedRPM = from32LEToHost(&buf[0]);
  currentUnit->accelRPM = from32LEToHost(&buf[4]);
  currentUnit->decelRPM = from32LEToHost(&buf[8]);
  currentUnit->speedMaxRPM = from32LEToHost(&buf[12]);
  return count;
}

int StepperDriver::controlMoveRelative(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitHandle(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  AsyncStepper *motor = static_cast<AsyncStepper *>(currentUnit->getDeviceObject());
  if (motor == 0) return EBADSLT;
  if (count != 5) return EMSGSIZE;

  int actualSpeedRequested = (currentUnit->speedMaxRPM <= 0) ? (int)(currentUnit->speedRPM) :
                             (int)(constrain(currentUnit->speedRPM, 0, currentUnit->speedMaxRPM));
  motor->setStepsToMove((int)from32LEToHost(buf), actualSpeedRequested,
                        (int)currentUnit->accelRPM, (int)currentUnit->decelRPM);
  return count;
}

//---------------------------------------------------------------------------

int StepperDriver::microTimer(unsigned long deltaMicros, ClientReporter *r) {

  // update active stepper positions

  for (int lun = 0; lun < logicalUnitCount; lun++) {
    if (logicalUnits[lun] != 0) {
      StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[lun]);
      AsyncStepper *motor = currentUnit->getDeviceObject();
      if (motor != 0) {

        int reg = static_cast<int>(Stepper::AtPosition);
        int status = statusAtPosition(lun, reg, 2, &(currentUnit->buf[0]));

        // notify client application when stepping is complete

        if ((status == 2) && (currentUnit->buf[0] == 1)) {
          int handle = DeviceDriver::getFullHandle(lun);
          r->reportStatus(handle, status, &(currentUnit->buf[0]));
        }
      }
    }
  }
}

int StepperDriver::milliTimer(unsigned long deltaMillis, ClientReporter *r) {
  return ESUCCESS ;
}
