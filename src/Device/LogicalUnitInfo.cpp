#include "LogicalUnitInfo.h"
#include "DeviceDriver.h"

  LogicalUnitInfo::LogicalUnitInfo() {

  previousTime[0] = 0;
  previousTime[1] = 0;
  intervalTime[0] = DEFAULT_UPDATE_INTERVAL;
  intervalTime[1] = DEFAULT_REPORT_INTERVAL;

  }

  LogicalUnitInfo::~LogicalUnitInfo() {}
