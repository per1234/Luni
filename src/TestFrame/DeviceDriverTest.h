#ifndef DeviceDriverTest_h
#define DeviceDriverTest_h

#include <LuniLib.h>
#include <Device/DeviceTable.h>
#include <Device/ClientReporter.h>

#include "Logger.h"
#include "TestManager.h"

extern DeviceTable *gDeviceTable;

class DeviceDriverTest {

public:
  DeviceDriverTest(const char *groupName, const char *unitIdentifier);
  virtual void doTest(TestManager *tst, ClientReporter *r, Logger *log) = 0;

protected:
  char *groupID;
  char *unitID;
};

#endif
