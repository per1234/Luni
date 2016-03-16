#include "DDSignalDriver.h"

//---------------------------------------------------------------------------

DEFINE_SEMVER_PRE(DDSignal, 0, 7, 0, beta)

/**
 * This device driver provides access to basic signal IO functions, including
 * pinMode(), digitalWrite(), digitalRead(), analogReference(), analogRead(),
 * and analogWrite().
 */
DDSignal::DDSignal(const char *dName, int lunCount) :
  DeviceDriver(dName, lunCount) {
  if (logicalUnitCount > 0) {
    servos = new LUSignal[logicalUnitCount];
    if (servos == 0) {
      delete logicalUnits;
      logicalUnitCount = 0;
    }
  }
  for (int idx = 0; idx < logicalUnitCount; idx++) {
    logicalUnits[idx] = 0;
  }
}

//---------------------------------------------------------------------------

int DDSignal::open(const char *name, int flags) {
  int lun;
  int status = DeviceDriver::open(name, flags);
  if (status < 0) {
    return status;
  }

  lun = status;
  LUSignal *currentUnit = &servos[lun];

  currentUnit->detach();
  currentUnit->minPulse = MIN_PULSE_WIDTH;
  currentUnit->maxPulse = MAX_PULSE_WIDTH;

  logicalUnits[lun] = currentUnit;
  return lun;
}

//---------------------------------------------------------------------------

int DDSignal::read(int handle, int reg, int count, byte *buf) {
  int units;

  LUSignal *currentUnit = static_cast<LUSignal *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (count < 0) return EINVAL;

  // Registers for which no pin attachment is needed

  switch (reg) {

  case (int)(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
           preReleaseLabel, buildLabel, count, buf);

  case (int)(CDR::Intervals):
    return DeviceDriver::readIntervals(handle, reg, count, buf);

  }

  //  Registers for which we must be attached to a pin (and thus a servo)

  if (!currentUnit->attached()) return ENODATA;

  switch (reg) {

  case (int)(DDSignal::REG::PIN):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(currentUnit->pin, buf);
    return 2;

  case (int)(DDSignal::REG::RANGE_MICROSECONDS):
    if (count < 4) return EMSGSIZE;
    fromHostTo16LE(currentUnit->minPulse, buf);
    fromHostTo16LE(currentUnit->maxPulse, buf + 2);
    return 4;

  case (int)(DDSignal::REG::POSITION_DEGREES):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(currentUnit->read(), buf);
    return 2;

  case (int)(DDSignal::REG::POSITION_MICROSECONDS):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(currentUnit->readMicroseconds(), buf);
    return 2;

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

//---------------------------------------------------------------------------

int DDSignal::write(int handle, int reg, int count, byte *buf) {
  int thePin;
  uint8_t channel;
  int loPulse;
  int hiPulse;
  int pos;

  LUSignal *currentUnit = static_cast<LUSignal *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(DDSignal::REG::PIN):
    if (count != 2) return EMSGSIZE;
    if (currentUnit->attached()) {
      currentUnit->detach();
    }
    thePin = from16LEToHost(buf);
    channel = currentUnit->attach(thePin, currentUnit->minPulse, currentUnit->maxPulse);
    if (channel == 255) return EMFILE;
    currentUnit->pin = thePin;
    return 2;

  case (int)(DDSignal::REG::RANGE_MICROSECONDS):
    if (count != 4) return EMSGSIZE;
    loPulse = from16LEToHost(buf);
    hiPulse = from16LEToHost(buf + 2);
    if (currentUnit->attached()) {
      currentUnit->detach();
      channel = currentUnit->attach(currentUnit->pin, loPulse, hiPulse);
      if (channel == 255) return EMFILE;
    }
    currentUnit->minPulse = loPulse;
    currentUnit->maxPulse = hiPulse;
    return 4;

  case (int)(DDSignal::REG::POSITION_DEGREES):
    if (count != 2) return EMSGSIZE;
    pos = from16LEToHost(buf);
    currentUnit->write(pos);
    return 2;

  case (int)(DDSignal::REG::POSITION_MICROSECONDS):
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

int DDSignal::close(int handle) {
  int lun = getUnitNumber(handle);
  LUSignal *currentUnit = static_cast<LUSignal *>(logicalUnits[lun]);
  currentUnit->detach();
  logicalUnits[lun] = 0;
  return DeviceDriver::close(handle);
}
