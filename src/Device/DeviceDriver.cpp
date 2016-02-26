#include <Device/DeviceDriver.h>

//---------------------------------------------------------------------------

DeviceDriver::DeviceDriver(const char *r, const int count) :
  rootName(r),
  logicalUnitCount(count),
  logicalUnits(new LogicalUnitInfo * [count]()),
  deviceIndex(0)
{};

//---------------------------------------------------------------------------

int DeviceDriver::open(const char *name, int flags) {
  int lun;

  size_t unitNameLength = strcspn(name, ":");
  if ((strlen(rootName) != unitNameLength) || (strncmp(rootName, name, unitNameLength) != 0)) {
    return ENODEV;
  }

  lun = atoi(&name[unitNameLength + 1]);
  if (lun < 0 || lun >= logicalUnitCount) {
    return ENXIO;
  }

  if (logicalUnits[lun] == 0) {
    return lun;
  }

  if ((flags & DDO_FORCE_OPEN) == 0) {
    return EADDRINUSE;
  } else {
    delete logicalUnits[lun];
    return lun;
  }
  return ENXIO;
}

int DeviceDriver::close(int handle) {
  LogicalUnitInfo *currentDevice = logicalUnits[(handle & 0x7F)];
  if (currentDevice != 0) {
    delete currentDevice;
    logicalUnits[(handle & 0x7F)] = 0;
  }
  return ESUCCESS;
}

//---------------------------------------------------------------------------

int DeviceDriver::statusIntervals(int handle, int reg, int count, byte *buf) {
  LogicalUnitInfo *currentUnit = logicalUnits[getUnitHandle(handle)];
  if (currentUnit == 0) return ENOTCONN;
  if (count < 8) return EMSGSIZE;

  fromHostTo32LE(currentUnit->intervalTime[0], &buf[0]);
  fromHostTo32LE(currentUnit->intervalTime[1], &buf[4]);
  return 8;
}

//---------------------------------------------------------------------------

int DeviceDriver::controlIntervals(int handle, int reg, int count, byte *buf) {
  LogicalUnitInfo *currentUnit = logicalUnits[getUnitHandle(handle)];
  if (currentUnit == 0) return ENOTCONN;
  if (count < 8) return EMSGSIZE;

  currentUnit->intervalTime[0] = from32LEToHost(&buf[0]);
  currentUnit->intervalTime[1] = from32LEToHost(&buf[4]);
  return 8;
}

//---------------------------------------------------------------------------

int DeviceDriver::getFullHandle(int lun) {
  return ((deviceIndex & 0x7F) << 7) | (lun & 0x7F);
}

//---------------------------------------------------------------------------

int DeviceDriver::buildVersionResponse(const byte *semver, const char *name,
                                       const char *prLabel, const char *bLabel, int count, byte *buf) {

  int nameLength = strlen_P(name);
  int prLength = strlen_P(prLabel);
  int bLength = strlen_P(bLabel);

  int packetSize = pgm_read_byte_near(&semver[0]);

  if (count < (1 + packetSize + nameLength + 1 + prLength + 1 + bLength + 1)) {
    return EMSGSIZE;
  }

  int byteIndex = 0;
  buf[byteIndex++] = packetSize;

  // version

  for (int idx = 0; idx < packetSize; idx++) {
    buf[byteIndex++] = pgm_read_byte_near(&semver[idx + 1]);
  }

  // preReleaseLabel (including terminating null)

  for (int idx = 0; idx <= prLength; idx++) {
    buf[byteIndex++] = pgm_read_byte_near(&prLabel[idx]);
  }

  // buildLabel (including terminating null)

  for (int idx = 0; idx <= bLength; idx++) {
    buf[byteIndex++] = pgm_read_byte_near(&bLabel[idx]);
  }

  // name (including terminating null)

  for (int idx = 0; idx <= nameLength; idx++) {
    buf[byteIndex++] = pgm_read_byte_near(&name[idx]);
  }

  return byteIndex;
}

//---------------------------------------------------------------------------

int DeviceDriver::checkForTimerEvents(ClientReporter *r) {
  int status;
  int result = ESUCCESS;
  for (int timer = 0; timer < 2; timer++) {
    for (int lun = 0; lun < logicalUnitCount; lun++) {
      LogicalUnitInfo *currentDevice = logicalUnits[lun];
      if (currentDevice != 0 && currentDevice->intervalTime[timer] != 0) {
        currentDevice->deltaTime[timer] = calculateElapsedTime(currentDevice, timer);
        if (currentDevice->deltaTime[timer] >= currentDevice->intervalTime[timer]) {
          status = processTimerEvent(lun, timer, r);
          result = (status == ESUCCESS) ? result : status;
        }
      }
    }
  }
  return result;
}

unsigned long DeviceDriver::calculateElapsedTime(LogicalUnitInfo *lui, int timerIndex) {
  unsigned long elapsedTime;
  int status;

  lui->currentTime[timerIndex] = (lui == 0) ? micros() : millis();

  if (lui->currentTime[timerIndex] >= lui->previousTime[timerIndex]) {
    elapsedTime = lui->currentTime[timerIndex] - lui->previousTime[timerIndex];
  } else {
    elapsedTime = (ULONG_MAX - lui->previousTime[timerIndex]) + (lui->currentTime[timerIndex] + 1);
  }

  if (elapsedTime >= lui->intervalTime[timerIndex]) {
    lui->previousTime[timerIndex] = lui->currentTime[timerIndex];
    return elapsedTime;
  } else {
    return 0;
  }
}

int DeviceDriver::processTimerEvent(int lun, int timerIndex, ClientReporter *r) {
  return ESUCCESS;
}
