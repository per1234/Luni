#include "PeekDriver.h"
#include <limits.h>
#include <Framework/ByteOrder.h>

// #include <ConfigurableFirmata.h>
// #define StringBufferSize 100

/**
 * This PeekDriver class is an administrative and development tool to
 * provide code analysis capabilities and a place to perform timing
 * tests and the like.
 */
DEFINE_SEMVER(PeekDriver, 0, 7, 0)

//---------------------------------------------------------------------------

PeekDriver::PeekDriver(const char *dName, int count) :
  DeviceDriver(dName, count) {
}

//---------------------------------------------------------------------------

int PeekDriver::open(const char *name, int flags) {
  int lun;
  int status = DeviceDriver::open(name, flags);
  if (status < 0) {
    return status;
  }

  lun = status;
  PeekLUI *currentUnit = new PeekLUI();

  // for (int idx = 0; idx < 2; idx++) {
  //   currentUnit->sampleIndex[idx] = 0;
  //   currentUnit->isSampleBufferFull[idx] = false;
  // }

  logicalUnits[lun] = currentUnit;
  return lun;
}

//---------------------------------------------------------------------------

int PeekDriver::read(int handle, int reg, int count, byte *buf) {
  PeekLUI *currentUnit = static_cast<PeekLUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case static_cast<int>(CSR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
           preReleaseLabel, buildLabel, count, buf);

  case static_cast<int>(CSR::Intervals):
    return DeviceDriver::readIntervals(handle, reg, count, buf);

  case static_cast<int>(PeekRegister::AVG_INTERVALS):
    return readATI(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

int PeekDriver::write(int handle, int reg, int count, byte *buf) {
  PeekLUI *currentUnit = static_cast<PeekLUI *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case static_cast<int>(CCR::Intervals):
    return DeviceDriver::writeIntervals(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

int PeekDriver::close(int handle) {
  return DeviceDriver::close(handle);
}

//---------------------------------------------------------------------------

// Collect duration samples.  The sample array is actually 0..SAMPLE_COUNT,
// and the useful samples are in 1..SAMPLE_COUNT.

int PeekDriver::processTimerEvent(int lun, int timerSelector, ClientReporter *r) {

  PeekLUI *cU = static_cast<PeekLUI *>(logicalUnits[getUnitNumber(lun)]);
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

int PeekDriver::readATI(int handle, int reg, int count, byte *buf) {
  unsigned long avg;
  PeekLUI *cU = static_cast<PeekLUI *>(logicalUnits[getUnitNumber(handle)]);
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
