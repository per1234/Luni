#include "DeviceTable.h"
#include "ClientReporter.h"

/**
 * This DeviceTable class provides single point access to all the loaded
 * DeviceDriver objects.  The open() method is used to identify the driver of
 * interest, and the returned handle is then supplied in future calls to the
 * rest of the methods.It implements all the DeviceDriver standard methods via
 * dispatch to the appropriate individual driver.
 *
 * Note that generally speaking a caller should use either DeviceTable methods
 * or direct calls to DeviceDriver objects, but not both.  This is because the
 * 16-bit handles returned by DeviceTable contain both a device index value and
 * a logical unit index value, whereas the 7-bit handles returned by the
 * DeviceDrivers themselves contain only a logical unit value.
 */
DeviceTable::DeviceTable(DeviceDriver *deviceArray[], ClientReporter *reporter) {

  cr = reporter;

  deviceCount = 0;
  if (deviceArray == 0) {
    devices = 0;
    return;
  }

  while (deviceArray[deviceCount] != 0) {
    deviceCount += 1;
  }

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
  }
}

DeviceTable::~DeviceTable() {}

void DeviceTable::reset() {
  for (int idx = 0; idx < deviceCount; idx++) {
    devices[idx]->reset();
  }
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
  int deviceIndex = getDeviceNumber(handle);
  if (deviceIndex < 0 || deviceIndex >= deviceCount) return EINVAL;
  return devices[deviceIndex]->read(handle, reg, count, buf);
}

int DeviceTable::write(int handle, int reg, int count, byte *buf) {
  int deviceIndex = getDeviceNumber(handle);
  if (deviceIndex < 0 || deviceIndex >= deviceCount) return EINVAL;
  return devices[deviceIndex]->write(handle, reg, count, buf);
}

int DeviceTable::close(int handle) {
  int deviceIndex = getDeviceNumber(handle);
  if (deviceIndex < 0 || deviceIndex >= deviceCount) return EINVAL;
  return devices[deviceIndex]->close(handle);
}

//----------------------------------------------------------------------------

int DeviceTable::dispatchTimers() {
  int result = ESUCCESS;
  for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
    int status = devices[deviceIndex]->checkForTimerEvents(cr);
    result = (status < 0) ? status : result;
  }
  return result;
}
