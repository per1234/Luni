#include "PeekDriver.h"
#include <limits.h>

/**
 * This PeekDriver class is an administrative and development tool to
 * provide code analysis capabilities and a place to perform timing
 * tests and the like.
 */
DEFINE_SEMVER(PeekDriver, 0, 1, 0)

//---------------------------------------------------------------------------

PeekDriver::PeekDriver(const char *dName, int count) :
  DeviceDriver(dName, count) {
  previousTime[0] = 0;
  previousTime[1] = 0;
  currentTime[0] = 0;
  currentTime[1] = 0;
  sampleIndex = 0;
  isSampleBufferFull = false;
}

//---------------------------------------------------------------------------

int PeekDriver::open(const char *name, int flags) {
  int lun;
  int status = DeviceDriver::open(name,flags);
  if (status < 0) {
    return status;
  }

  lun = status;
  PeekLUI *currentUnit = new PeekLUI();

  // Any further validation of the current unit's appropriateness goes here ...

  logicalUnits[lun] = currentUnit;
  return lun;
}

//---------------------------------------------------------------------------

int PeekDriver::status(int handle, int reg, int count, byte *buf) {
  PeekLUI *currentUnit = static_cast<PeekLUI *>(logicalUnits[handle & 0x7F]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case static_cast<int>(CSR::DriverVersion):
    return DeviceDriver::buildVersionResponse(releaseVersion, scopeName,
      preReleaseLabel, buildLabel, count, buf);

  case static_cast<int>(PeekRegister::AVG_REPORT_INTERVAL):
    return statusARI(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

int PeekDriver::control(int handle, int reg, int count, byte *buf) {
  return ENOSYS;
}

int PeekDriver::read(int handle, int count, byte *buf) {
  return ENOSYS;
}
int PeekDriver::write(int handle, int count, byte *buf) {
  return ENOSYS;
}

int PeekDriver::close(int handle) {
  return DeviceDriver::close(handle);
}

//---------------------------------------------------------------------------

int PeekDriver::update(unsigned long deltaMicros) {
  return ENODATA;
}

//---------------------------------------------------------------------------

// Collect a millisecond interval (report()) duration sample.  The sample array
// is actually 0..SAMPLE_COUNT, and the useful samples are in 1..SAMPLE_COUNT.

int PeekDriver::report(unsigned long deltaMillis) {
  currentTime[1] = millis();

  unsigned long elapsedTime;

  if (currentTime[1] >= previousTime[1]) {
    elapsedTime = currentTime[1] - previousTime[1];
  } else {
    elapsedTime = (ULONG_MAX - (previousTime[1] - currentTime[1])) + 1;
  }

  samples[sampleIndex] = elapsedTime;
  isSampleBufferFull |= (sampleIndex == SAMPLE_COUNT);
  sampleIndex = 1 + ((sampleIndex) % SAMPLE_COUNT);
  previousTime[1] = currentTime[1];

  return ENODATA;
}

//---------------------------------------------------------------------------

int PeekDriver::statusARI(int handle, int reg, int count, byte *buf) {
  if (count < 4) {
    return EMSGSIZE;
  } else if (isSampleBufferFull) {
    unsigned long avg = calculateAverageInterval();
    buf[0] = avg & 0xFF;
    buf[1] = (avg >> 8) & 0xFF;
    buf[2] = (avg >> 16) & 0xFF;
    buf[3] = (avg >> 24) & 0xFF;
    return 4;
  } else {
    return ENODATA;
  }

  return ENOTSUP;
}

unsigned long PeekDriver::calculateAverageInterval() {
  int sum = 0;
  for (int idx = 1; idx <= SAMPLE_COUNT; idx++) {
    sum += samples[idx];
  }
  return sum / SAMPLE_COUNT;
}
