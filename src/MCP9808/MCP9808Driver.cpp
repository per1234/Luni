#include "MCP9808Driver.h"
#include <Wire.h>

//---------------------------------------------------------------------------

DEFINE_SEMVER_PRE(MCP9808Driver, 0, 7, 0, beta)

/**
 * This device driver is for the Microchip Technology MCP9808 Digital
 * Temperature Sensor.
 *
 * The device is controlled via I2C.  It expects that 16-bit register values
 * will be written in big-endian order, that is, the most significant byte
 * first, the least significant byte last.
 */
MCP9808Driver::MCP9808Driver(const char *dName, int lunCount, int baseI2CAddress) :
  DeviceDriver(dName, lunCount),
  baseAddress(baseI2CAddress),
  i2c() {}

//---------------------------------------------------------------------------

int MCP9808Driver::open(const char *name, int flags) {
  int lun;
  int status = DeviceDriver::open(name, flags);
  if (status < 0) {
    return status;
  }

  lun = status;
  MCP9808LUI *currentUnit = new MCP9808LUI(baseAddress + lun);

  int address = currentUnit->i2cAddress;
  int theRegister = static_cast<int>(MCP9808Register::MANUF_ID);
  if (i2c.read16BE(address, theRegister) != 0x0054) {
    return ECONNREFUSED;
  }

  theRegister = static_cast<int>(MCP9808Register::DEVICE_ID);
  if (i2c.read16BE(address, theRegister) != 0x0400) {
    return ECONNREFUSED;
  }

  logicalUnits[lun] = currentUnit;
  return lun;
}

/**
 * Read a register on the device.
 */
int MCP9808Driver::read(int handle, int reg, int count, byte *buf) {
  uint8_t v8;
  uint16_t v16;
  int v;

  MCP9808LUI *currentUnit = static_cast<MCP9808LUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (count < 0) return EINVAL;

  int address = currentUnit->i2cAddress;

  switch (reg) {

  case static_cast<int>(CSR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
           preReleaseLabel, buildLabel, count, buf);

  case static_cast<int>(CSR::Stream):
    if (count < 2) {
      return EINVAL;
    }
    reg = static_cast<int>(MCP9808Register::AMBIENT_TEMP);
    v = i2c.read16BE(address, reg);
    fromHostTo16BE(v, buf);
    return count;

  case static_cast<int>(MCP9808Register::CONFIG):
  case static_cast<int>(MCP9808Register::UPPER_TEMP):
  case static_cast<int>(MCP9808Register::LOWER_TEMP):
  case static_cast<int>(MCP9808Register::CRIT_TEMP):
  case static_cast<int>(MCP9808Register::AMBIENT_TEMP):
  case static_cast<int>(MCP9808Register::MANUF_ID):
  case static_cast<int>(MCP9808Register::DEVICE_ID):
    if (count < 2) return EMSGSIZE;
    v16 = i2c.read16BE(address, reg);
    fromHostTo16BE(v16, buf);
    return 2;

  case static_cast<int>(MCP9808Register::RESOLUTION):
    if (count < 1) return EMSGSIZE;
    v8 = i2c.read8(address, reg);
    buf[0] = v8;
    return 1;

  default:
    return ENOTSUP;
  }
}

int MCP9808Driver::write(int handle, int reg, int count, byte *buf) {

  MCP9808LUI *currentUnit = static_cast<MCP9808LUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  int theI2CAddress = currentUnit->i2cAddress;
  if (theI2CAddress < I2C_MIN_7BIT_ADDRESS || theI2CAddress > I2C_MAX_7BIT_ADDRESS) {
    return EBADFD;
  }

  switch (reg) {

  case static_cast<int>(MCP9808Register::CONFIG):
  case static_cast<int>(MCP9808Register::UPPER_TEMP):
  case static_cast<int>(MCP9808Register::LOWER_TEMP):
  case static_cast<int>(MCP9808Register::CRIT_TEMP):
    if (count == 2) {
      i2c.write16BE(theI2CAddress, reg, from16BEToHost(buf));
      return count;
    } else {
      return EINVAL;
    }

  case static_cast<int>(MCP9808Register::RESOLUTION):
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

int MCP9808Driver::close(int handle) {
  return DeviceDriver::close(handle);
}
