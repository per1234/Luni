#include "ServoDriver.h"

//---------------------------------------------------------------------------

DEFINE_SEMVER_PRE(ServoDriver, 0, 7, 0, beta)

/**
 * This device driver is for servo controllers.  It uses the basic servo
 * library supplied with Arduino.
 */
ServoDriver::ServoDriver(const char *dName, int lunCount) :
  DeviceDriver(dName, lunCount){}

//---------------------------------------------------------------------------

int ServoDriver::open(const char *name, int flags) {
  int lun;
  // int status = DeviceDriver::open(name, flags);
  int status = DeviceDriver::open(name, 0);
  if (status < 0) {
    return status;
  }

  lun = status;
  ServoLUI *currentUnit = new ServoLUI();

  currentUnit->pin = flags;
  currentUnit->attach(flags);

  logicalUnits[lun] = currentUnit;
  return lun;
}

//---------------------------------------------------------------------------

int ServoDriver::read(int handle, int reg, int count, byte *buf) {
  int units;

  ServoLUI *currentUnit = static_cast<ServoLUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (count < 0) return EINVAL;

  switch (reg) {

  case static_cast<int>(CSR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
           preReleaseLabel, buildLabel, count, buf);

  case (int)(ServoRegister::SCALE):
    if (count < 4) return EMSGSIZE;
    fromHostTo16LE(currentUnit->fromLow, buf);
    fromHostTo16LE(currentUnit->fromHigh, buf + 2);
    return 4;

  case (int)(ServoRegister::POSITION):
    if (count < 2) return EMSGSIZE;
    units = map(currentUnit->read(), 0, 180, currentUnit->fromLow, currentUnit->fromHigh);
    fromHostTo16LE(units, buf);
    return 2;

  case (int)(ServoRegister::PIN):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(currentUnit->pin, buf);
    return 2;

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

//---------------------------------------------------------------------------

int ServoDriver::write(int handle, int reg, int count, byte *buf) {
  int units;
  int degrees;

  ServoLUI *currentUnit = static_cast<ServoLUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(ServoRegister::SCALE):
    if (count != 4) return EMSGSIZE;
    currentUnit->fromLow = from16LEToHost(buf);
    currentUnit->fromHigh = from16LEToHost(buf + 2);
    return 4;

  case (int)(ServoRegister::POSITION):
    if (count != 2) return EMSGSIZE;
    units = from16LEToHost(buf);
    degrees = map(units, currentUnit->fromLow, currentUnit->fromHigh, 0, 180);
    currentUnit->write(degrees);
    return 2;

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

//---------------------------------------------------------------------------

int ServoDriver::close(int handle) {
  return DeviceDriver::close(handle);
}
