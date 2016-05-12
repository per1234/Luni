#ifndef DDTCommon_h
#define DDTCommon_h

#include <LuniLib.h>
#include <TestFrame/TestManager.h>
#include <TestFrame/DeviceDriverTest.h>
#include <Device/ClientReporter.h>

class DDTCommon : public DeviceDriverTest {
public:
  DDTCommon(const char *unitIdentifier);
  void doTest(TestManager *tst, ClientReporter *r);
};

#endif
