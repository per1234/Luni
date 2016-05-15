#ifndef TestManager_h
#define TestManager_h

#include <Arduino.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "Logger.h"
#include <Device/ClientReporter.h>

class TestManager {
public:

  TestManager(const char *filename);
  ~TestManager();

  void runSelected();

  void dispatchTimers();
  void countDown(int seconds);

  void beforeGroup(const char *groupName);
  void beforeTest(const char *testName);
  void afterTest();
  void afterGroup();

  void assertTrue(const char *msg, bool condition);
  void assertFalse(const char *msg, bool condition);

  template <typename T0, typename T1>
  void assertEquals(const char *msg, T0 expected, T1 actual);

  int getTestFailureCount();
  int getGroupFailureCount();

private:

  Logger *logger;
  ClientReporter *rpt;

  const char *theFileName;
  const char *theTestName;
  const char *theGroupName;

  int testFailureCount;
  int groupFailureCount;
};

#include "TestManagerTemplates.h"

#endif
