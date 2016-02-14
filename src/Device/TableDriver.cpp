#include "TableDriver.h"

//---------------------------------------------------------------------------

DEFINE_SEMVER(TableDriver, 0, 1, 0)

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
int TableDriver::status(int handle, int reg, int count, byte *buf) {
  TableLUI *currentUnit = static_cast<TableLUI *>(logicalUnits[handle & 0x7F]);
  if (currentUnit == 0) {
    return ENOTCONN;
  }

  switch (reg) {

  case static_cast<int>(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName, preReleaseLabel, buildLabel, count, buf);

  default:
    return ENOTSUP;
  }
}

/**
 * Write a control value to the virtual device.
 */
int TableDriver::control(int handle, int reg, int count, byte *buf) {
  TableLUI *currentUnit = static_cast<TableLUI *>(logicalUnits[handle & 0x7F]);
  if (currentUnit == 0) {
    return ENOTCONN;
  }
  return ENOSYS;
}

/**
 * The read() method of this device driver returns a string of all the
 * version strings for all the installed device drivers.
 * @param  handle [the identifier of the logical unit, as returned by an earlier call to open()]
 * @param  count  [size of the buffer in which to return the result]
 * @param  buf    [result buffer]
 * @return        [status code.  <0 error, >= count of bytes read]
 */
int TableDriver::read(int handle, int count, byte * buf) {
  // TableLUI *currentUnit = static_cast<TableLUI *>(logicalUnits[handle & 0x7F]);
  // if (currentUnit == 0) {
  //   return ENOTCONN;
  // }

  // for (int deviceIndex = 0; deviceIndex < theDeviceTable->deviceCount; deviceIndex++) {
  //   DeviceDriver *dd = theDeviceTable->devices[deviceIndex];
  //   int regV = static_cast<int>(CDR::DriverVersion);
  //   dd->status(127,regV,count,buf);
  // }
  // return count;
  return ENOTSUP;
}

int TableDriver::write(int handle, int count, byte * buf) {
  TableLUI *currentUnit = static_cast<TableLUI *>(logicalUnits[handle & 0x7F]);
  if (currentUnit == 0) {
    return ENOTCONN;
  }
  return ENOSYS;
}

int TableDriver::close(int handle) {
  return DeviceDriver::close(handle);
}
