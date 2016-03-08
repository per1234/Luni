#include "DeviceTable.h"
#include "TableDriver.h"
#include "ClientReporter.h"

DEFINE_SEMVER(DeviceTable, 0, 7, 0)

/**
 * This DeviceTable class provides single point access to all the loaded
 * DeviceDriver objects.  The open() method is used to identify the driver of
 * interest, and the returned handle is then supplied in future calls to the
 * rest of the methods.It implements all the DeviceDriver standard methods via
 * dispatch to the appropriate individual driver.
 *
 * Note that generally speaking a caller should use either DeviceTable methods
 * or direct calls to DeviceDriver objects, but not both.  This is because the
 * 14-bit handles returned by DeviceTable contain both a device index value and
 * a logical unit index value, whereas the 7-bit handles returned by the
 * DeviceDrivers themselves contain only a logical unit value.
 */
DeviceTable::DeviceTable(DeviceDriver *deviceArray[], const char*luRootName) {

  deviceCount = 0;
  if (deviceArray == 0) {
    devices = 0;
    return;
  }

  bool enableTableDriver = (luRootName != 0);
  while (deviceArray[deviceCount] != 0) {
    deviceCount += 1;
  }
  deviceCount = (enableTableDriver) ? deviceCount + 1 : deviceCount;

  devices = new DeviceDriver*[deviceCount];
  if (devices == 0) {
    deviceCount = 0;
    return;
  } else {
    int idx = 0;
    while (deviceArray[idx] != 0) {
      devices[idx] = deviceArray[idx];
      devices[idx]->deviceNumber = idx;
      idx += 1;
    }

    if (enableTableDriver) {
      DeviceDriver *metaDriver = new TableDriver(this, luRootName, 1);
      if (metaDriver == 0) {
        free(devices);
        deviceCount = 0;
      } else {
        devices[idx] = metaDriver;
        devices[idx]->deviceNumber = idx;
      }
    }
  }
}

DeviceTable::~DeviceTable() {}

void DeviceTable::reset() {
}

//---------------------------------------------------------------------------

int DeviceTable::open(const char *name, int flags) {
  int deviceIndex;
  int status = ENODEV;
  for (deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
    status = devices[deviceIndex]->open((char *)name, flags);
    if (status == ENXIO || status == ENODEV) {
      continue;
    } else {
      break;
    }
  }
  if (status >= 0) {
    return makeHandle(deviceIndex, status);
  } else {
    return status;
  }
}

int DeviceTable::read(int handle, int reg, int count, byte *buf) {
  return devices[getDeviceNumber(handle)]->read(getUnitNumber(handle), reg, count, buf);
}

int DeviceTable::write(int handle, int reg, int count, byte *buf) {
  return devices[getDeviceNumber(handle)]->write(getUnitNumber(handle), reg, count, buf);
}

int DeviceTable::close(int handle) {
  return devices[getDeviceNumber(handle)]->close(getUnitNumber(handle));
}

//----------------------------------------------------------------------------

int DeviceTable::dispatchTimers(ClientReporter *reporter) {
  int result = ESUCCESS;
  for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
    int status = devices[deviceIndex]->checkForTimerEvents(reporter);
    result = (status < 0) ? status : result;
  }
  return result;
}
