#include <Device/DeviceDriver.h>

//---------------------------------------------------------------------------

DeviceDriver::DeviceDriver(const char *r, const int count) :
rootName(r),
logicalUnitCount(count),
logicalUnits(new LogicalUnitInfo*[count]())
{};

//---------------------------------------------------------------------------

void DeviceDriver::update(unsigned long deltaMicros) {
  return;
}

void DeviceDriver::report(unsigned long deltaMillis) {
  return;
}

//---------------------------------------------------------------------------

int DeviceDriver::open(const char *name, int flags) {
  int lun;

  int unitNameLength = strcspn(name,":");
  if ((strlen(rootName) != unitNameLength) || (strncmp(rootName,name,unitNameLength) != 0)) {
    return ENODEV;
  }

  lun = atoi(&name[unitNameLength+1]);
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

int DeviceDriver::buildVersionResponse(const byte *semver,const char *name,
  const char *prLabel, const char *bLabel, int count, byte *buf) {

  int nameLength = strlen_P(name);
  int prLength = strlen_P(prLabel);
  int bLength = strlen_P(bLabel);

  int packetSize = pgm_read_byte_near(&semver[0]);

  if (count < (1 + packetSize + nameLength + 1 + prLength + 1 + bLength +1)) {
    return EMSGSIZE;
  }

  int byteIndex = 0;
  buf[byteIndex++] = packetSize;

  // version

  for (int idx=0; idx < packetSize; idx++) {
    buf[byteIndex++] = pgm_read_byte_near(&semver[idx+1]);
  }

  // preReleaseLabel (including terminating null)

  for (int idx=0;idx <= prLength;idx++) {
    buf[byteIndex++] = pgm_read_byte_near(&prLabel[idx]);
  }

  // buildLabel (including terminating null)

  for (int idx=0;idx <= bLength;idx++) {
    buf[byteIndex++] = pgm_read_byte_near(&bLabel[idx]);
  }

  // name (including terminating null)

  for (int idx=0;idx <= nameLength;idx++) {
    buf[byteIndex++] = pgm_read_byte_near(&name[idx]);
  }

  return byteIndex;
}


