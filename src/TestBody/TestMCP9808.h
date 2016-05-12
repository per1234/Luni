#ifndef TestMCP9808_h
#define TestMCP9808_h

#include <LuniLib.h>
#include <Device/ClientReporter.h>

#include <TestFrame/TestManager.h>
#include <TestFrame/DeviceDriverTest.h>
#include <TestFrame/Logger.h>

class TestMCP9808 : public DeviceDriverTest {
public:
  TestMCP9808(const char *unitIdentifier);
  void doTest(TestManager *tst, ClientReporter *r, Logger *log);
};

#endif
