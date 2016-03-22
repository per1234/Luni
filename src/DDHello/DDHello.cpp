#include "DDHello.h"

/**
 * This class defines a simple device driver as a sort of
 * HelloWorld component for device drivers and their usage.
 */

DEFINE_SEMVER_PRE(DDHello, 0, 7, 0, beta)

//---------------------------------------------------------------------------

DDHello::DDHello(const char *dName, int count) :
  DeviceDriver(dName, count) {}

//---------------------------------------------------------------------------

int DDHello::open(const char *name, int flags) {

  int lun;
  int status = DeviceDriver::open(name, flags);
  if (status < 0) {
    return status;
  }

  lun = status;
  LUHello *currentUnit = new LUHello("World");

  // Any further validation of the current unit's appropriateness goes here ...

  logicalUnits[lun] = currentUnit;
  return lun;
}

//---------------------------------------------------------------------------

int DDHello::read(int handle, int reg, int count, byte *buf) {

  // First, handle connection-optional requests

  switch (reg) {

  case (int)(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
           preReleaseLabel, buildLabel, count, buf);

  case (int)(CDR::UnitNamePrefix):
      return DeviceDriver::buildReadPrefixResponse(count,buf);
 }

  // Second, deal with connection-required requests

  int lun = getUnitNumber(handle);
  if (lun < 0 || lun >= logicalUnitCount) return EINVAL;
  LUHello *currentUnit = static_cast<LUHello *>(logicalUnits[lun]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(CDR::Stream):
    if ((size_t)count >= (strlen(currentUnit->getWho()) + strlen(currentUnit->getWhat()) + 4)) {
      buf[0] = (uint8_t)'\0';
      strcat((char *)buf, currentUnit->getWhat());
      strcat((char *)buf, ", ");
      strcat((char *)buf, currentUnit->getWho());
      strcat((char *)buf, "!");
      return strlen((char *)buf);
    } else {
      return EMSGSIZE;
    }

  case (int)(REG::INTERJECTION):
    strlcpy((char *)buf, currentUnit->getWhat(), count);
    return strlen(currentUnit->getWhat());

  case (int)(REG::OBJECT):
    strlcpy((char *)buf, currentUnit->getWho(), count);
    return strlen(currentUnit->getWho());

  default:
    return ENOTSUP;
  }
}

int DDHello::write(int handle, int reg, int count, byte *buf) {

  // First, handle connection-optional requests

  switch (reg) {

  case (int)(CDR::UnitNamePrefix):
      return DeviceDriver::buildWritePrefixResponse(count,buf);
  }

  // Second, deal with connection-required requests

  int lun = getUnitNumber(handle);
  if (lun < 0 || lun >= logicalUnitCount) return EINVAL;
  LUHello *currentUnit = static_cast<LUHello *>(logicalUnits[lun]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(REG::INTERJECTION):
    currentUnit->setWhat((char *)buf);
    return strlen(currentUnit->getWhat());

  case (int)(REG::OBJECT):
    currentUnit->setWho((char *)buf);
    return strlen(currentUnit->getWho());

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

int DDHello::close(int handle) {
  return DeviceDriver::close(handle);
}
