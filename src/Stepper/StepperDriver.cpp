#include "StepperDriver.h"
#include "StepperDriverLUI.h"

DEFINE_SEMVER(StepperDriver, 0, 6, 0)

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
int StepperDriver::read(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {
  case static_cast<int>(CSR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
           preReleaseLabel, buildLabel, count, buf);

  case static_cast<int>(CSR::Intervals):
    return DeviceDriver::readIntervals(handle, reg, count, buf);

  case static_cast<int>(Stepper::RPMSpeeds):
    return readRPMSpeeds(handle, reg, count, buf);

  case static_cast<int>(Stepper::PositionEvent):
    return readPositionEvent(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }

  return EPANIC;
}

//---------------------------------------------------------------------------

int StepperDriver::write(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {
  case static_cast<int>(CCR::Configure):
    return writeConfigure(handle, reg, count, buf);

  case static_cast<int>(CCR::Intervals):
    return DeviceDriver::writeIntervals(handle, reg, count, buf);

  case static_cast<int>(Stepper::MoveRelative):
    return writeMoveRelative(handle, reg, count, buf);

  case static_cast<int>(Stepper::RPMSpeeds):
    return writeRPMSpeeds(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

//---------------------------------------------------------------------------

int StepperDriver::close(int handle) {
  return DeviceDriver::close(handle);
}

//---------------------------------------------------------------------------

int StepperDriver::readRPMSpeeds(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  AsyncStepper *motor = currentUnit->getDeviceObject();
  if (motor == 0) return EBADFD;
  if (count != 8) return EMSGSIZE;

  fromHostTo16LE(currentUnit->speedRPM, &buf[0]);
  fromHostTo16LE(currentUnit->speedMaxRPM, &buf[2]);
  fromHostTo16LE(currentUnit->accelRPM, &buf[4]);
  fromHostTo16LE(currentUnit->decelRPM, &buf[6]);
  return 8;
}

//---------------------------------------------------------------------------

int StepperDriver::readPositionEvent(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitNumber(handle)]);
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

int StepperDriver::writeConfigure(int handle, int reg, int count, byte *buf) {
  int interface, stepCount, pin1, pin2, pin3, pin4;
  AsyncStepper *motor;

  if (count < 7) return EMSGSIZE;

  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitNumber(handle)]);
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

int StepperDriver::writeRPMSpeeds(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  AsyncStepper *motor = static_cast<AsyncStepper *>(currentUnit->getDeviceObject());
  if (motor == 0) return EBADSLT;
  if (count != 8) return EMSGSIZE;

  currentUnit->speedRPM = from16LEToHost(&buf[0]);
  currentUnit->speedMaxRPM = from16LEToHost(&buf[2]);
  currentUnit->accelRPM = from16LEToHost(&buf[4]);
  currentUnit->decelRPM = from16LEToHost(&buf[6]);
  return count;
}

int StepperDriver::writeMoveRelative(int handle, int reg, int count, byte *buf) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[getUnitNumber(handle)]);
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

int StepperDriver::processTimerEvent(int lun, int timerIndex, ClientReporter *r) {
  StepperDriverLUI *currentUnit = static_cast<StepperDriverLUI *>(logicalUnits[lun]);
  if (currentUnit == 0) return ENOTCONN;
  int result;
  int reg;
  int handle;
  int status;
  AsyncStepper *motor;

  switch (timerIndex) {

  case 0:     // microsecond timer
    motor = currentUnit->getDeviceObject();
    if (motor == 0) return EBADFD;

    reg = static_cast<int>(Stepper::PositionEvent);
    handle = DeviceDriver::getFullHandle(lun);
    status = readPositionEvent(handle, reg, 2, &(currentUnit->buf[0]));

    // notify client application when stepping is complete

    if ((status == 2) && (currentUnit->buf[0] == 1)) {
      r->reportRead(handle, status, &(currentUnit->buf[0]));
    }
    result = ESUCCESS;
    break;

  case 1:       // millisecond timer
    result = ESUCCESS;
    break;

  default:      // unrecognized timer index
    result = ENOTSUP;
    break;
  }

  return result;
}
