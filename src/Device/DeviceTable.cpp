#include "DeviceTable.h"
#include "TableDriver.h"

DEFINE_SEMVER(DeviceTable, 0, 1, 0)

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

  previousTime[0] = 0;
  previousTime[1] = 0;
  intervalTime[0] = DEFAULT_UPDATE_INTERVAL;
  intervalTime[1] = DEFAULT_REPORT_INTERVAL;

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
      idx += 1;
    }

    if (enableTableDriver) {
      DeviceDriver *metaDriver = new TableDriver(this, luRootName, 1);
      if (metaDriver == 0) {
        free(devices);
        deviceCount = 0;
      } else {
        devices[idx] = metaDriver;
      }
    }
  }
}

DeviceTable::~DeviceTable() {}

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
  return (status < 0) ? status : ((deviceIndex & 0x7F) << 7) | (status & 0x7F);
}

int DeviceTable::control(int handle, int reg, int count, byte *buf) {
  return devices[getDeviceHandle(handle)]->control(getUnitHandle(handle), reg, count, buf);
}

int DeviceTable::status(int handle, int reg, int count, byte *buf) {
  return devices[getDeviceHandle(handle)]->status(getUnitHandle(handle), reg, count, buf);
}

int DeviceTable::read(int handle, int count, byte * buf) {
  return devices[getDeviceHandle(handle)]->read(getUnitHandle(handle), count, buf);
}

int DeviceTable::write(int handle, int count, byte * buf) {
  return devices[getDeviceHandle(handle)]->write(getUnitHandle(handle), count, buf);
}

int DeviceTable::close(int handle) {
  return devices[getDeviceHandle(handle)]->close(getUnitHandle(handle));
}

//----------------------------------------------------------------------------

void DeviceTable::dispatchTimers() {
  unsigned long elapsedTime;
  int status;

  currentTime[0] = micros();
  currentTime[1] = millis();

  for (int idx = 0; idx < 2; idx++) {
    if (currentTime[idx] >= previousTime[idx]) {
      elapsedTime = currentTime[idx] - previousTime[idx];
    } else {
      elapsedTime = (ULONG_MAX - previousTime[idx]) + (currentTime[idx] + 1);
    }

    if (elapsedTime >= intervalTime[idx]) {
      if (idx == 0) {
        for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
          status = devices[deviceIndex]->update(elapsedTime);
        }
      } else {
        for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
          status = devices[deviceIndex]->report(elapsedTime);
        }
      }
      previousTime[idx] = currentTime[idx];
    }
  }
}
