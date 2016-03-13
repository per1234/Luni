#include "DDServo.h"

//---------------------------------------------------------------------------

DEFINE_SEMVER_PRE(DDServo, 0, 7, 0, beta)

/**
 * This device driver is for servo controllers.  It uses the basic servo
 * library supplied with Arduino.
 */
DDServo::DDServo(const char *dName, int lunCount) :
  DeviceDriver(dName, lunCount) {}

//---------------------------------------------------------------------------

int DDServo::open(const char *name, int flags) {
  int lun;
  int status = DeviceDriver::open(name, flags);
  if (status < 0) {
    return status;
  }

  lun = status;
  LUServo *currentUnit = new LUServo();

  logicalUnits[lun] = currentUnit;
  return lun;
}

//---------------------------------------------------------------------------

int DDServo::read(int handle, int reg, int count, byte *buf) {
  int units;

  LUServo *currentUnit = static_cast<LUServo *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (count < 0) return EINVAL;

  switch (reg) {

  case (int)(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
           preReleaseLabel, buildLabel, count, buf);

  case (int)(CDR::Intervals):
    return DeviceDriver::readIntervals(handle, reg, count, buf);

  case (int)(DDServo::REG::PIN):
    if (count < 2) return EMSGSIZE;
    if (!currentUnit->attached()) return ENODATA;
    fromHostTo16LE(currentUnit->pin, buf);
    return 2;

  case (int)(DDServo::REG::RANGE_MICROSECONDS):
    if (count < 4) return EMSGSIZE;
    fromHostTo16LE(currentUnit->minPulse, buf);
    fromHostTo16LE(currentUnit->maxPulse, buf + 2);
    return 4;

  case (int)(DDServo::REG::POSITION_DEGREES):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(currentUnit->read(), buf);
    return 2;

  case (int)(DDServo::REG::POSITION_MICROSECONDS):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(currentUnit->readMicroseconds(), buf);
    return 2;

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

//---------------------------------------------------------------------------

int DDServo::write(int handle, int reg, int count, byte *buf) {
  int thePin;
  uint8_t channel;
  int loPulse;
  int hiPulse;
  int pos;

  LUServo *currentUnit = static_cast<LUServo *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(DDServo::REG::PIN):
    if (count != 2) return EMSGSIZE;
    thePin = from16LEToHost(buf);
    if (currentUnit->attached()) {
      currentUnit->detach();
    }
    channel = currentUnit->attach(thePin, currentUnit->minPulse, currentUnit->maxPulse);
    if (channel == 255) return EIO;
    currentUnit->pin = thePin;
    return 2;

  case (int)(DDServo::REG::RANGE_MICROSECONDS):
    if (count != 4) return EMSGSIZE;
    loPulse = from16LEToHost(buf);
    hiPulse = from16LEToHost(buf + 2);
    if (currentUnit->attached()) {
      currentUnit->detach();
      channel = currentUnit->attach(currentUnit->pin, loPulse, hiPulse);
      if (channel == 0) return EIO;
    }
    currentUnit->minPulse = loPulse;
    currentUnit->maxPulse = hiPulse;
    return 4;

  case (int)(DDServo::REG::POSITION_DEGREES):
    if (count != 2) return EMSGSIZE;
    pos = from16LEToHost(buf);
    currentUnit->write(pos);
    return 2;

  case (int)(DDServo::REG::POSITION_MICROSECONDS):
    if (count != 2) return EMSGSIZE;
    pos = from16LEToHost(buf);
    currentUnit->writeMicroseconds(pos);
    return 2;

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

//---------------------------------------------------------------------------

int DDServo::close(int handle) {
  return DeviceDriver::close(handle);
}
