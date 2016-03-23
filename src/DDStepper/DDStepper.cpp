#include "DDStepper.h"
#include "LUStepper.h"

extern LogicalUnitInfo * const OPEN_BUT_NOT_CONFIGURED;

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

DDStepper::DDStepper(const char *dName, int addrCount) :
  DeviceDriver(dName, addrCount) {
  DEFINE_VERSION_PRE(0, 8, 0, beta)
}

//---------------------------------------------------------------------------

int DDStepper::open(const char *name, int flags) {
  int lun;
  int status = DeviceDriver::open(name, flags);
  if (status < 0) {
    return status;
  }

  lun = status;
  logicalUnits[lun] = OPEN_BUT_NOT_CONFIGURED;
  return lun;
}

//---------------------------------------------------------------------------

/**
 * Read a status register on the device.
 */
int DDStepper::read(int handle, int reg, int count, byte *buf) {

  switch (reg) {
  case (int)(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(count, buf);
  }

  LUStepper *currentUnit = static_cast<LUStepper *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (currentUnit == OPEN_BUT_NOT_CONFIGURED) return EBADFD;

  switch (reg) {

  case (int)(CDR::Intervals):
    return DeviceDriver::readIntervals(handle, reg, count, buf);

  case (int)(REG::RPMSpeeds):
    return readRPMSpeeds(handle, reg, count, buf);

  case (int)(REG::PositionEvent):
    return readPositionEvent(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }

  return EPANIC;
}

//---------------------------------------------------------------------------

int DDStepper::write(int handle, int reg, int count, byte *buf) {
  LUStepper *currentUnit = static_cast<LUStepper *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (currentUnit == OPEN_BUT_NOT_CONFIGURED) return EBADFD;

  switch (reg) {
  case (int)(CDR::Configure):
    return writeConfigure(handle, reg, count, buf);

  case (int)(CDR::Intervals):
    return DeviceDriver::writeIntervals(handle, reg, count, buf);

  case (int)(REG::MoveRelative):
    return writeMoveRelative(handle, reg, count, buf);

  case (int)(REG::RPMSpeeds):
    return writeRPMSpeeds(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

//---------------------------------------------------------------------------

int DDStepper::close(int handle) {
  return DeviceDriver::close(handle);
}

//---------------------------------------------------------------------------

int DDStepper::readRPMSpeeds(int handle, int reg, int count, byte *buf) {
  LUStepper *currentUnit = static_cast<LUStepper *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (currentUnit == OPEN_BUT_NOT_CONFIGURED) return EBADFD;

  if (count != 8) return EMSGSIZE;

  fromHostTo16LE(currentUnit->speedRPM, &buf[0]);
  fromHostTo16LE(currentUnit->speedMaxRPM, &buf[2]);
  fromHostTo16LE(currentUnit->accelRPM, &buf[4]);
  fromHostTo16LE(currentUnit->decelRPM, &buf[6]);
  return 8;
}

//---------------------------------------------------------------------------

int DDStepper::readPositionEvent(int handle, int reg, int count, byte *buf) {
  LUStepper *currentUnit = static_cast<LUStepper *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (currentUnit == OPEN_BUT_NOT_CONFIGURED) return EBADFD;

  if (count < 2) return EMSGSIZE;

  bool stopped = currentUnit->update();
  buf[0] = (byte)(stopped);
  buf[1] = (byte)(false);
  return 2;
}

//---------------------------------------------------------------------------

int DDStepper::writeConfigure(int handle, int reg, int count, byte *buf) {
  int interface, stepCount, pin1, pin2, pin3, pin4;
  LUStepper *motor;

  if (count < 7) return EMSGSIZE;

  LUStepper *currentUnit = static_cast<LUStepper *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  interface = from8LEToHost(buf);
  stepCount = from16LEToHost(&buf[1]);
  pin1 = from16LEToHost(&buf[3]);
  pin2 = from16LEToHost(&buf[5]);

  if (interface == (int)(AsyncStepper::DRIVER)) {
    return ENOTSUP;
  } else if (interface == (int)(AsyncStepper::TWO_WIRE)) {
    if (count == 7) {
      motor = new LUStepper(AsyncStepper::TWO_WIRE, stepCount, pin1, pin2);
      if (motor == 0) return ENOMEM;
    } else {
      return EMSGSIZE;
    }
  } else if (interface == (int)(AsyncStepper::FOUR_WIRE)) {
    if (count == 11) {
      pin3 = from16LEToHost(&buf[7]);
      pin4 = from16LEToHost(&buf[9]);
      motor = new LUStepper(AsyncStepper::FOUR_WIRE, stepCount, pin1, pin2, pin3, pin4);
      if (motor == 0) return ENOMEM;
    } else {
      return EMSGSIZE;
    }
  } else {
    return ENOTSUP;
  }

  motor->speedRPM = 0;
  motor->speedMaxRPM = 0;
  motor->accelRPM = 0;
  motor->decelRPM = 0;

  logicalUnits[getUnitNumber(handle)] = motor;

  return count;
}

int DDStepper::writeRPMSpeeds(int handle, int reg, int count, byte *buf) {
  LUStepper *currentUnit = static_cast<LUStepper *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (currentUnit == OPEN_BUT_NOT_CONFIGURED) return EBADFD;
  if (count != 8) return EMSGSIZE;

  currentUnit->speedRPM = from16LEToHost(&buf[0]);
  currentUnit->speedMaxRPM = from16LEToHost(&buf[2]);
  currentUnit->accelRPM = from16LEToHost(&buf[4]);
  currentUnit->decelRPM = from16LEToHost(&buf[6]);
  return count;
}

int DDStepper::writeMoveRelative(int handle, int reg, int count, byte *buf) {
  LUStepper *currentUnit = static_cast<LUStepper *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (currentUnit == OPEN_BUT_NOT_CONFIGURED) return EBADFD;
  if (count != 5) return EMSGSIZE;

  int actualSpeedRequested = (currentUnit->speedMaxRPM <= 0) ? (int)(currentUnit->speedRPM) :
                             (int)(constrain(currentUnit->speedRPM, 0, currentUnit->speedMaxRPM));
  currentUnit->setStepsToMove((int)from32LEToHost(buf), actualSpeedRequested,
                              (int)currentUnit->accelRPM, (int)currentUnit->decelRPM);
  return count;
}

//---------------------------------------------------------------------------

int DDStepper::processTimerEvent(int lun, int timerIndex, ClientReporter *r) {
  LUStepper *currentUnit = static_cast<LUStepper *>(logicalUnits[lun]);
  if (currentUnit == 0) return ENOTCONN;
  if (currentUnit == OPEN_BUT_NOT_CONFIGURED) return EBADFD;
  int result;
  int reg;
  int handle;
  int status;

  switch (timerIndex) {

  case 0:     // microsecond timer

    reg = (int)(REG::PositionEvent);
    handle = makeHandle(deviceNumber, lun);
    status = readPositionEvent(handle, reg, 2, &(currentUnit->buf[0]));

    // notify client application when stepping is complete

    if ((status == 2) && (currentUnit->buf[0] == 1)) {
      r->reportRead(status, handle, reg, 2, currentUnit->buf);
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
