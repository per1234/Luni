#include "DDSensor.h"
#include <Wire.h>

//---------------------------------------------------------------------------

/**
 * This device driver is used with a variety of analog input sensors.
 *
 */
DDSensor::DDSensor(const char *dName, int lunCount) :
  DeviceDriver(dName, lunCount) {
  DEFINE_VERSION_PRE(0, 8, 0, beta)
}

//---------------------------------------------------------------------------

int DDSensor::open(int opts, int flags, const char *name) {
  int lun;
  int status = DeviceDriver::open(opts, flags, name);
  if (status < 0) {
    return status;
  }

  lun = status;
  LUSensor *currentUnit = new LUSensor();

  logicalUnits[lun] = currentUnit;
  return lun;
}

/**
 * Read a register on the device.
 */
int DDSensor::read(int handle, int flags, int reg, int count, byte *buf) {
  uint8_t v8;
  uint16_t v16;
  int v;

  // Registers for which no handle is needed

  switch (reg) {

  case (int)(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(count, buf);

  case (int)(CDR::UnitNamePrefix):
      return DeviceDriver::buildReadPrefixResponse(count,buf);

  }

  //  Registers for which we must have a handle (ie, open() has
  //  been done) but we don't need to be attached to a pin

  LUSensor *currentUnit = static_cast<LUSensor *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (count < 0) return EINVAL;

  switch (reg) {

    case (int)(CDR::Intervals):
      return DeviceDriver::readIntervals(handle, flags, reg, count, buf);

  }

  //  Registers for which we must have a handle (ie, open() has
  //  been done) and we need to be attached to a pin (and thus
  //  a transducer of some sort)

  if (!currentUnit->isAttached()) return ENODATA;

  switch (reg) {

  case (int)(DDSensor::REG::PIN):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(currentUnit->pin, buf);
    return 2;

  case (int)(CDR::Stream):
    if (count < 2) {
      return EINVAL;
    }
    v16 = analogRead(PIN_TO_ANALOG(currentUnit->pin));
    fromHostTo16BE(v, buf);
    return count;

  case static_cast<int>(REG::RESOLUTION):
    if (count < 1) return EMSGSIZE;
    buf[0] = currentUnit->resolution;
    return 1;

  default:
    return ENOTSUP;
  }
}

int DDSensor::write(int handle, int flags, int reg, int count, byte *buf) {
  int thePin;

  LUSensor *currentUnit = static_cast<LUSensor *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(DDSensor::REG::PIN):
    if (count != 2) return EMSGSIZE;
    thePin = from16LEToHost(buf);
    if (!(IS_PIN_ANALOG(thePin))) return EINVAL;

    if (currentUnit->isAttached()) {
      currentUnit->detach();
    }
    currentUnit->attach(thePin);
    return 2;

  case static_cast<int>(REG::RESOLUTION):
    if (count == 1) {
      currentUnit->resolution = from8BEToHost(buf);
      return count;
    } else {
      return EINVAL;
    }

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

int DDSensor::close(int handle, int flags) {
  return DeviceDriver::close(handle, flags);
}
