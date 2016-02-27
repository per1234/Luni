#include "PeekDriver.h"
#include <limits.h>
#include <Framework/ByteOrder.h>

/**
 * This PeekDriver class is an administrative and development tool to
 * provide code analysis capabilities and a place to perform timing
 * tests and the like.
 */
DEFINE_SEMVER(PeekDriver, 0, 1, 0)

//---------------------------------------------------------------------------

PeekDriver::PeekDriver(const char *dName, int count) :
  DeviceDriver(dName, count) {
  for (int idx = 0; idx < 2; idx++) {
    previousTime[idx] = 0;
    currentTime[idx] = 0;
    sampleIndex[idx] = 0;
    isSampleBufferFull[idx] = false;
  }
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

  case static_cast<int>(CSR::Intervals):
    return DeviceDriver::statusIntervals(handle, reg, count, buf);

  case static_cast<int>(PeekRegister::AVG_INTERVALS):
    return statusATI(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
}

int PeekDriver::control(int handle, int reg, int count, byte *buf) {
  PeekLUI *currentUnit = static_cast<PeekLUI *>(logicalUnits[handle & 0x7F]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case static_cast<int>(CCR::Intervals):
    return DeviceDriver::controlIntervals(handle, reg, count, buf);

  default:
    return ENOTSUP;
  }
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

// Collect duration samples.  The sample array is actually 0..SAMPLE_COUNT,
// and the useful samples are in 1..SAMPLE_COUNT.

int PeekDriver::processTimerEvent(int lun, int timerIndex, ClientReporter *r) {
  unsigned long elapsedTime;

  switch (timerIndex) {

  case 0:       // microsecond timer
  case 1:       // millisecond timer

    currentTime[timerIndex] = (timerIndex == 0) ? micros() : millis();
    if (currentTime[timerIndex] >= previousTime[timerIndex]) {
      elapsedTime = currentTime[timerIndex] - previousTime[timerIndex];
    } else {
      elapsedTime = (ULONG_MAX - (previousTime[timerIndex] - currentTime[timerIndex])) + 1;
    }

    samples[timerIndex][sampleIndex[timerIndex]] = elapsedTime;
    isSampleBufferFull[timerIndex] |= (sampleIndex[timerIndex] == SAMPLE_COUNT);
    sampleIndex[timerIndex] = 1 + ((sampleIndex[timerIndex]) % SAMPLE_COUNT);
    previousTime[timerIndex] = currentTime[timerIndex];
    return ESUCCESS;

  default:      // unrecognized timer index
    return ENOTSUP;

  }
}

//---------------------------------------------------------------------------

int PeekDriver::statusATI(int handle, int reg, int count, byte *buf) {
  if (count < 8) {
    return EMSGSIZE;
  }
  if (!isSampleBufferFull[0]) {
    return EOWNERDEAD;
  }
  if (!isSampleBufferFull[1]) {
    return ENOTRECOVERABLE;
  }
  for (int idx = 0; idx < 2; idx++) {
    unsigned long avg = calculateAverageInterval(idx);
    fromHostTo32LE(avg, buf + (4 * idx));
  }
  return 8;
}

unsigned long PeekDriver::calculateAverageInterval(int timerIndex) {
  int sum = 0;
  for (int idx = 1; idx <= SAMPLE_COUNT; idx++) {
    sum += samples[timerIndex][idx];
  }
  return sum / SAMPLE_COUNT;
}
