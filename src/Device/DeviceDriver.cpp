#include <Device/DeviceDriver.h>

//---------------------------------------------------------------------------

DeviceDriver::DeviceDriver(const char *pre, const int count) :
  unitNamePrefix((char *)pre),
  logicalUnitCount(count),
  logicalUnits(new LogicalUnitInfo * [count]) {

  if (logicalUnits == 0) {  // out of memory
    logicalUnitCount = 0;
  }

  for (int idx = 0; idx < logicalUnitCount; idx++) {
    logicalUnits[idx] = 0;
  }
};

//---------------------------------------------------------------------------

int DeviceDriver::open(const char *name, int flags) {
  int lun;

  int prefixLength = strcspn(name, ":");
  if ((strlen(unitNamePrefix) != prefixLength) || (strncmp(unitNamePrefix, name, prefixLength) != 0)) {
    return ENODEV;
  }

  lun = atoi(&name[prefixLength + 1]);
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

// Any subclass of DeviceDriver that requires additional functionality in its
// processTimerEvent() or reset() methods should override these implementations
// appropriately.

int DeviceDriver::processTimerEvent(int lun, int timerSelector, ClientReporter * r) {
  return ESUCCESS;
}

void DeviceDriver::reset() {}

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

int DeviceDriver::buildReadPrefixResponse(int count, byte *buf) {
  int scopeLength = strlen(scopeName);
  int prefixLength = strlen(unitNamePrefix);
  if (count < (scopeLength + prefixLength + 2)) return EMSGSIZE;

  buf[0] = 0;
  strcat((char *)buf, scopeName);
  strcat((char *)buf, unitNamePrefix);
  return scopeLength + prefixLength + 2;
}

int DeviceDriver::buildWritePrefixResponse(int count, const byte *newPrefix) {
  free(unitNamePrefix);
  unitNamePrefix = strdup((char *)newPrefix);
  if (unitNamePrefix == 0) return ENOMEM;
  return strlen(unitNamePrefix);
}

//---------------------------------------------------------------------------

int DeviceDriver::buildVersionResponse(int count, byte *buf) {

  int packetSize = releaseVersion[0];
  int nameLength = strlen(scopeName);
  int prLength = strlen(preReleaseLabel);
  int bLength = strlen(buildLabel);

  if (count < (1 + packetSize + nameLength + 1 + prLength + 1 + bLength + 1)) {
    return EMSGSIZE;
  }

  int byteIndex = 0;

  // version

  buf[byteIndex++] = packetSize;
  for (int idx = 0; idx < packetSize; idx++) {
    buf[byteIndex++] = releaseVersion[idx + 1];
  }

  // preReleaseLabel (including terminating null)

  for (int idx = 0; idx <= prLength; idx++) {
    buf[byteIndex++] = preReleaseLabel[idx];
  }

  // buildLabel (including terminating null)

  for (int idx = 0; idx <= bLength; idx++) {
    buf[byteIndex++] = buildLabel[idx];
  }

  // name (including terminating null)

  for (int idx = 0; idx <= nameLength; idx++) {
    buf[byteIndex++] = scopeName[idx];
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
