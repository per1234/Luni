#include "LogicalUnitInfo.h"
#include "DeviceDriver.h"

  LogicalUnitInfo::LogicalUnitInfo() {

  previousTime[0] = 0;
  previousTime[1] = 0;
  intervalTime[0] = 0;
  intervalTime[1] = 0;

  milliAction.enabled = false;
  microAction.enabled = false;

  }

  LogicalUnitInfo::~LogicalUnitInfo() {}


LogicalUnitInfo NULL_LUI;
LogicalUnitInfo * const OPEN_BUT_NOT_CONFIGURED = &NULL_LUI;

