#ifndef TestServo_h
#define TestServo_h

#include <LuniLib.h>
#include <Device/ClientReporter.h>

#include <TestFrame/TestManager.h>
#include <TestFrame/DeviceDriverTest.h>
#include <TestFrame/Logger.h>

class TestServo : public DeviceDriverTest {
public:
  TestServo(const char *unitIdentifier);
  void doTest(TestManager *tst, ClientReporter *r, Logger *log);
};

#endif
