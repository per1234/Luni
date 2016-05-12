#ifndef TestCommon_h
#define TestCommon_h

#include <LuniLib.h>
#include <Device/ClientReporter.h>

#include <TestFrame/TestManager.h>
#include <TestFrame/DeviceDriverTest.h>
#include <TestFrame/Logger.h>

class TestCommon : public DeviceDriverTest {
public:
  TestCommon(const char *unitIdentifier);
  void doTest(TestManager *tst, ClientReporter *r, Logger *log);
};

#endif
