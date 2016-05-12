#ifndef TestMeta_h
#define TestMeta_h

#include <LuniLib.h>
#include <Device/ClientReporter.h>

#include <TestFrame/TestManager.h>
#include <TestFrame/DeviceDriverTest.h>
#include <TestFrame/Logger.h>

class TestMeta : public DeviceDriverTest {
public:
  TestMeta(const char *unitIdentifier);
  void doTest(TestManager *tst, ClientReporter *r, Logger *log);
};

#endif
