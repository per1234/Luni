#include "LogicalUnitInfo.h"
#include "DeviceDriver.h"
#include "DeviceTable.h"

extern DeviceTable *gDeviceTable;

  LogicalUnitInfo::LogicalUnitInfo() {

  previousTime[0] = 0;
  previousTime[1] = 0;
  intervalTime[0] = 0;
  intervalTime[1] = 0;

  eventAction[0].enabled = false;
  eventAction[1].enabled = false;

  }

  LogicalUnitInfo::~LogicalUnitInfo() {}

//----------------------------------------------------------------------------

/**
 * Request exclusive use of a particular pin.
 * @param  pin A full-range (digital) pin number.
 * @return True if pin was idle, now it's ours.  Return false if pin
 * was already in use by someone else and we can't have it.
 */
bool LogicalUnitInfo::lockPin(int pin) {
  return (gDeviceTable->claimPin(pin));
}

// return t
// return false

/**
 * Release all claim to exclusive use of a particular pin.
 * @param  pin A full-range (digital) pin number.
 * @return True if pin was ours, now it's not.  Return false if
 * pin was not in use by anyone!
 */
bool LogicalUnitInfo::unlockPin(int pin) {
  return (gDeviceTable->releasePin(pin));
}

LogicalUnitInfo NULL_LUI;
const LogicalUnitInfo *OPEN_BUT_NOT_CONFIGURED = &NULL_LUI;

