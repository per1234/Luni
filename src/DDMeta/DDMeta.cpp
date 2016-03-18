#include "DDMeta.h"
#include <Framework/ByteOrder.h>
#include <limits.h>

DEFINE_SEMVER(DDMeta, 0, 8, 0)

/**
 * This DDMeta class is an administrative and development tool.  As a device
 * driver, it provides an API for:
 * <ul>
 * <li>access to the DeviceTable (how many devices, device driver versions, etc)</li>
 * <li>code analysis capabilities (memory usage, timing, etc)</li>
 * </ul>
 */

//---------------------------------------------------------------------------

DDMeta::DDMeta(const char *dName, int count) :
  DeviceDriver(dName, count) {
}

//---------------------------------------------------------------------------

int DDMeta::open(const char *name, int flags) {
  int lun;
  int status = DeviceDriver::open(name, flags);
  if (status < 0) return status;

  lun = status;
  LUMeta *currentUnit = new LUMeta();

  logicalUnits[lun] = currentUnit;
  return lun;
}

//---------------------------------------------------------------------------

int DDMeta::read(int handle, int reg, int count, byte *buf) {
  LUMeta *currentUnit = static_cast<LUMeta *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
           preReleaseLabel, buildLabel, count, buf);

  case (int)(CDR::Intervals):
    return DeviceDriver::readIntervals(handle, reg, count, buf);

  case (int)(REG::AVG_INTERVALS):
    return readATI(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

int DDMeta::write(int handle, int reg, int count, byte *buf) {
  LUMeta *currentUnit = static_cast<LUMeta *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(CDR::Intervals):
    return DeviceDriver::writeIntervals(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

int DDMeta::close(int handle) {
  return DeviceDriver::close(handle);
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

int DDMeta::readATI(int handle, int reg, int count, byte *buf) {
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
