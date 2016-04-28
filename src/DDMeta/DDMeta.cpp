#include "DDMeta.h"
#include <Device/DeviceTable.h>
#include <Framework/ByteOrder.h>
#include <limits.h>

//---------------------------------------------------------------------------

extern DeviceTable *Device;


/**
 * This DDMeta class is an administrative and development tool.  As a device
 * driver, it provides an API for:
 * <ul>
 * <li>access to the DeviceTable (how many devices, device driver versions, etc)</li>
 * <li>code analysis capabilities (memory usage, timing, etc)</li>
 * </ul>
 */
DDMeta::DDMeta(const char *dName, int count) :
  DeviceDriver(dName, count) {
  DEFINE_VERSION_PRE(0, 8, 0, beta)
}

//---------------------------------------------------------------------------

int DDMeta::open(int opts, int flags, const char *name) {
  int lun;
  int status = DeviceDriver::open(opts, flags, name);
  if (status < 0) return status;

  lun = status;
  LUMeta *currentUnit = new LUMeta();

  logicalUnits[lun] = currentUnit;
  return lun;
}

//---------------------------------------------------------------------------

int DDMeta::read(int handle, int flags, int reg, int count, byte *buf) {
  int status;
  byte versionBuffer[256];

  // First, handle connection-optional requests

  switch (reg) {

  case (int)(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(count, buf);

  case (int)(CDR::UnitNamePrefix):
      return DeviceDriver::buildReadPrefixResponse(count,buf);

  case (int)(REG::DRIVER_COUNT):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(Device->deviceCount, buf);
    return 2;

  case (int)(REG::DRIVER_VERSION_LIST):
    for (int idx=0; idx<Device->deviceCount; idx++) {
      status = Device->read(makeHandle(idx,0), flags, (int)(CDR::DriverVersion), 256, versionBuffer);
      Device->cr->reportRead(status, handle, flags, (int)(CDR::DriverVersion), 256, versionBuffer);
    }
    return ESUCCESS;

  case (int)(REG::UNIT_NAME_PREFIX_LIST):
    for (int idx=0; idx<Device->deviceCount; idx++) {
      status = Device->read(makeHandle(idx,0), flags, (int)(CDR::UnitNamePrefix), 256, versionBuffer);
      Device->cr->reportRead(status, handle, flags, (int)(CDR::UnitNamePrefix), 256, versionBuffer);
    }
    return ESUCCESS;
}

  // Second, deal with connection-required requests

  int lun = getUnitNumber(handle);
  if (lun < 0 || lun >= logicalUnitCount) return EINVAL;
  LUMeta *currentUnit = static_cast<LUMeta *>(logicalUnits[lun]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(CDR::Intervals):
    return DeviceDriver::readIntervals(handle, flags, reg, count, buf);

  case (int)(REG::AVG_INTERVALS):
    return readATI(handle, flags, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

int DDMeta::write(int handle, int flags, int reg, int count, byte *buf) {
  int unitNameLength;

  // First, handle connection-optional requests

  switch (reg) {

  case (int)(CDR::UnitNamePrefix):
      return DeviceDriver::buildWritePrefixResponse(count,buf);
  }

  // Second, deal with connection-required requests

  int lun = getUnitNumber(handle);
  if (lun < 0 || lun >= logicalUnitCount) return EINVAL;
  LUMeta *currentUnit = static_cast<LUMeta *>(logicalUnits[lun]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(CDR::Intervals):
    return DeviceDriver::writeIntervals(handle, flags, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

int DDMeta::close(int handle, int flags) {
  return DeviceDriver::close(handle, flags);
}

//---------------------------------------------------------------------------

// Collect duration samples.  The sample array is actually 0..SAMPLE_COUNT,
// and the useful samples are in 1..SAMPLE_COUNT.

int DDMeta::processTimerEvent(int lun, int timerSelector, ClientReporter *r) {

  LUMeta *cU = static_cast<LUMeta *>(logicalUnits[getUnitNumber(lun)]);
  if (cU == 0) return ENOTCONN;

  switch (timerSelector) {

  case 0:       // microsecond timer
  case 1:       // millisecond timer

    cU->samples[timerSelector][cU->sampleIndex[timerSelector]] = cU->deltaTime[timerSelector];
    cU->isSampleBufferFull[timerSelector] |= (cU->sampleIndex[timerSelector] == SAMPLE_COUNT);
    cU->sampleIndex[timerSelector] = 1 + ((cU->sampleIndex[timerSelector]) % (int)(SAMPLE_COUNT));

    return ESUCCESS;

  default:      // unrecognized timer index
    return ENOTSUP;

  }
}

//---------------------------------------------------------------------------

int DDMeta::readATI(int handle, int flags, int reg, int count, byte *buf) {
  unsigned long avg;
  LUMeta *cU = static_cast<LUMeta *>(logicalUnits[getUnitNumber(handle)]);
  if (cU == 0) return ENOTCONN;

  if (count < 8) return EMSGSIZE;

  for (int timerIndex = 0; timerIndex < 2; timerIndex++) {
    unsigned long sum = 0;

    if (cU->intervalTime[timerIndex] != 0) {
      if (!(cU->isSampleBufferFull[timerIndex])) return ENODATA;
      for (int idx = 1; idx <= SAMPLE_COUNT; idx++) {
        sum += cU->samples[timerIndex][idx];
      }
      avg = sum / SAMPLE_COUNT;
    } else {
      avg = 0;
    }
    fromHostTo32LE(avg, buf + (4 * timerIndex));
  }

  return 8;
}
