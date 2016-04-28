#include "DDMCP9808.h"
#include <Wire.h>

//---------------------------------------------------------------------------


/**
 * This device driver is for the Microchip Technology MCP9808 Digital
 * Temperature Sensor.
 *
 * The device is controlled via I2C.  It expects that 16-bit register values
 * will be written in big-endian order, that is, the most significant byte
 * first, the least significant byte last.
 */
DDMCP9808::DDMCP9808(const char *dName, int lunCount, int baseI2CAddress) :
  DeviceDriver(dName, lunCount),
  baseAddress(baseI2CAddress),
  i2c() {
  DEFINE_VERSION_PRE(0, 8, 0, beta)
}

//---------------------------------------------------------------------------

int DDMCP9808::open(int opts, int flags, const char *name) {
  int lun;
  int status = DeviceDriver::open(opts, flags, name);
  if (status < 0) {
    return status;
  }

  lun = status;
  LUMCP9808 *currentUnit = new LUMCP9808(baseAddress + lun);

  int address = currentUnit->i2cAddress;
  int theRegister = static_cast<int>(REG::MANUF_ID);
  if (i2c.read16BE(address, theRegister) != 0x0054) {
    return ECONNREFUSED;
  }

  theRegister = static_cast<int>(REG::DEVICE_ID);
  if (i2c.read16BE(address, theRegister) != 0x0400) {
    return ECONNREFUSED;
  }

  logicalUnits[lun] = currentUnit;
  return lun;
}

/**
 * Read a register on the device.
 */
int DDMCP9808::read(int handle, int flags, int reg, int count, byte *buf) {
  uint8_t v8;
  uint16_t v16;
  int v;

  switch (reg) {

  case (int)(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(count, buf);

  case (int)(CDR::UnitNamePrefix):
      return DeviceDriver::buildReadPrefixResponse(count,buf);

  }

  LUMCP9808 *currentUnit = static_cast<LUMCP9808 *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (count < 0) return EINVAL;

  int address = currentUnit->i2cAddress;

  switch (reg) {

  case (int)(CDR::Stream):
    if (count < 2) {
      return EINVAL;
    }
    reg = static_cast<int>(REG::AMBIENT_TEMP);
    v = i2c.read16BE(address, reg);
    fromHostTo16BE(v, buf);
    return count;

  case static_cast<int>(REG::CONFIG):
  case static_cast<int>(REG::UPPER_TEMP):
  case static_cast<int>(REG::LOWER_TEMP):
  case static_cast<int>(REG::CRIT_TEMP):
  case static_cast<int>(REG::AMBIENT_TEMP):
  case static_cast<int>(REG::MANUF_ID):
  case static_cast<int>(REG::DEVICE_ID):
    if (count < 2) return EMSGSIZE;
    v16 = i2c.read16BE(address, reg);
    fromHostTo16BE(v16, buf);
    return 2;

  case static_cast<int>(REG::RESOLUTION):
    if (count < 1) return EMSGSIZE;
    v8 = i2c.read8(address, reg);
    buf[0] = v8;
    return 1;

  default:
    return ENOTSUP;
  }
}

int DDMCP9808::write(int handle, int flags, int reg, int count, byte *buf) {

  LUMCP9808 *currentUnit = static_cast<LUMCP9808 *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  int theI2CAddress = currentUnit->i2cAddress;
  if (theI2CAddress < I2C_MIN_7BIT_ADDRESS || theI2CAddress > I2C_MAX_7BIT_ADDRESS) {
    return EBADFD;
  }

  switch (reg) {

  case static_cast<int>(REG::CONFIG):
  case static_cast<int>(REG::UPPER_TEMP):
  case static_cast<int>(REG::LOWER_TEMP):
  case static_cast<int>(REG::CRIT_TEMP):
    if (count == 2) {
      i2c.write16BE(theI2CAddress, reg, from16BEToHost(buf));
      return count;
    } else {
      return EINVAL;
    }

  case static_cast<int>(REG::RESOLUTION):
    if (count == 1) {
      i2c.write8BE(theI2CAddress, reg, from8BEToHost(buf));
      return count;
    } else {
      return EINVAL;
    }

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

int DDMCP9808::close(int handle, int flags) {
  return DeviceDriver::close(handle, flags);
}
