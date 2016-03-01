#include "TableDriver.h"

//---------------------------------------------------------------------------

DEFINE_SEMVER(TableDriver, 0, 6, 0)

/**
 * This device driver provides access to the DeviceTable using the same basic
 * methods as the other device drivers.  In other words, it is a device driver
 * for a virtual device, the DeviceTable and its array of DeviceDriver pointers.
 *
 */
TableDriver::TableDriver(const DeviceTable *dt, const char *dName, int lunCount) :
  DeviceDriver(dName, lunCount) {
    theDeviceTable = dt;
  }

//---------------------------------------------------------------------------

int TableDriver::open(const char *name, int flags) {
  int lun;
  int status = DeviceDriver::open(name, flags);
  if (status < 0) {
    return status;
  }

  lun = status;
  TableLUI *currentUnit = new TableLUI();

  logicalUnits[lun] = currentUnit;
  return lun;
}

/**
 * Read a status value from the virtual device.
 */
int TableDriver::read(int handle, int reg, int count, byte *buf) {
  TableLUI *currentUnit = static_cast<TableLUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) {
    return ENOTCONN;
  }

  switch (reg) {

  case static_cast<int>(CSR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName, preReleaseLabel, buildLabel, count, buf);

  default:
    return ENOTSUP;
  }
}

/**
 * Write a control value to the virtual device.
 */
int TableDriver::write(int handle, int reg, int count, byte *buf) {
  TableLUI *currentUnit = static_cast<TableLUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) {
    return ENOTCONN;
  }
  return ENOSYS;
}

int TableDriver::close(int handle) {
  return DeviceDriver::close(handle);
}
