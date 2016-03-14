#include <Device/DeviceDriver.h>

//---------------------------------------------------------------------------

DeviceDriver::DeviceDriver(const char *r, const int count) :
  rootName(r),
  logicalUnitCount(count),
  deviceNumber(0) {
  logicalUnits = new LogicalUnitInfo *[count]();
  if (logicalUnits = 0) {
    logicalUnitCount = 0;
  }
  for (int idx = 0; idx < count; idx++) {
    logicalUnits[idx] = 0;
  }
};

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
  LogicalUnitInfo *currentUnit = logicalUnits[getUnitNumber(handle)];
  if (currentUnit != 0) {
    delete currentUnit;
    logicalUnits[getUnitNumber(handle)] = 0;
  }
  return ESUCCESS;
}

//---------------------------------------------------------------------------

int DeviceDriver::checkForTimerEvents(ClientReporter *r) {
  int status;
  int result = ESUCCESS;
  for (int lun = 0; lun < logicalUnitCount; lun++) {
    LogicalUnitInfo *currentUnit = logicalUnits[lun];
    if (currentUnit != 0) {
      for (int timer = 0; timer < 2; timer++) {
        if (currentUnit->intervalTime[timer] > 0) {
          currentUnit->deltaTime[timer] = calculateElapsedTime(currentUnit, timer);
          if (currentUnit->deltaTime[timer] >= currentUnit->intervalTime[timer]) {
            status = this->processTimerEvent(lun, timer, r);
            currentUnit->previousTime[timer] = currentUnit->currentTime[timer];
            result = (status == ESUCCESS) ? result : status;
          }
        }
      }
    }
  }
  return result;
}

int DeviceDriver::processTimerEvent(int lun, int timerSelector, ClientReporter * r) {
  return ESUCCESS;
}

//---------------------------------------------------------------------------

int DeviceDriver::readIntervals(int handle, int reg, int count, byte *buf) {
  LogicalUnitInfo *currentUnit = logicalUnits[getUnitNumber(handle)];
  if (currentUnit == 0) return ENOTCONN;
  if (count < 8) return EMSGSIZE;

  fromHostTo32LE(currentUnit->intervalTime[0], &buf[0]);
  fromHostTo32LE(currentUnit->intervalTime[1], &buf[4]);
  return 8;
}

//---------------------------------------------------------------------------

int DeviceDriver::writeIntervals(int handle, int reg, int count, byte *buf) {
  LogicalUnitInfo *currentUnit = logicalUnits[getUnitNumber(handle)];
  if (currentUnit == 0) return ENOTCONN;
  if (count < 8) return EMSGSIZE;

  currentUnit->intervalTime[0] = from32LEToHost(&buf[0]);
  currentUnit->intervalTime[1] = from32LEToHost(&buf[4]);
  return 8;
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

unsigned long DeviceDriver::calculateElapsedTime(LogicalUnitInfo *lui, int timerIndex) {
  unsigned long elapsedTime;

  lui->currentTime[timerIndex] = (timerIndex == 0) ? micros() : millis();

  if (lui->currentTime[timerIndex] >= lui->previousTime[timerIndex]) {
    elapsedTime = lui->currentTime[timerIndex] - lui->previousTime[timerIndex];
  } else {
    elapsedTime = (ULONG_MAX - lui->previousTime[timerIndex]) + (lui->currentTime[timerIndex] + 1);
  }

  return elapsedTime;

}
