#include "HelloDriver.h"

/**
 * This class defines a simple device driver as a sort of
 * HelloWorld component for device drivers and their usage.
 */

DEFINE_SEMVER_PRE(HelloDriver, 0, 1, 0, beta)

//---------------------------------------------------------------------------

HelloDriver::HelloDriver(const char *dName, int count) :
  DeviceDriver(dName,count) {}

//---------------------------------------------------------------------------

int HelloDriver::open(const char *name, int flags) {
  int lun;
  int status = DeviceDriver::open(name,flags);
  if (status < 0) {
    return status;
  }

  lun = status;
  HelloLUI *currentUnit = new HelloLUI("World");

  // Any further validation of the current unit's appropriateness goes here ...

  logicalUnits[lun] = currentUnit;
  return lun;
}

/**
 * Read a status register on the device.
 */
int HelloDriver::status(int handle, int reg, int count, byte *buf) {

  HelloLUI *currentUnit = static_cast<HelloLUI *>(logicalUnits[handle & 0x7F]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {
  case static_cast<int>(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
      preReleaseLabel, buildLabel, count, buf);

  case static_cast<int>(HelloRegister::INTERJECTION):
    strlcpy((char *)buf,currentUnit->getWhat(),count);
    return strlen(currentUnit->getWhat());

  case static_cast<int>(HelloRegister::OBJECT):
    strlcpy((char *)buf,currentUnit->getWho(),count);
    return strlen(currentUnit->getWho());

  default:
    return ENOTSUP;
  }
}

int HelloDriver::control(int handle, int reg, int count, byte *buf) {

  HelloLUI *currentUnit = static_cast<HelloLUI *>(logicalUnits[handle & 0x7F]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case static_cast<int>(HelloRegister::INTERJECTION):
    currentUnit->setWhat((char *)buf);
    return strlen(currentUnit->getWhat());

  case static_cast<int>(HelloRegister::OBJECT):
    currentUnit->setWho((char *)buf);
    return strlen(currentUnit->getWho());

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

int HelloDriver::read(int handle, int count, byte *buf) {
  HelloLUI *currentUnit = static_cast<HelloLUI *>(logicalUnits[handle & 0x7F]);
  if (currentUnit == 0) return ENOTCONN;

  if (count >= (strlen(currentUnit->getWho())+strlen(currentUnit->getWhat())+4)) {
    buf[0] = (uint8_t)'\0';
    strcat((char *)buf,currentUnit->getWhat());
    strcat((char *)buf,", ");
    strcat((char *)buf,currentUnit->getWho());
    strcat((char *)buf,"!");
    return strlen((char *)buf);
  } else {
    return EMSGSIZE;
  }
  return EPANIC;
}

int HelloDriver::write(int handle, int count, byte *buf) {
  return ENOSYS;
}

int HelloDriver::close(int handle) {
  return DeviceDriver::close(handle);
}
