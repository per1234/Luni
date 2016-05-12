#include <LuniLib.h>

#include <Device/DeviceDriver.h>

#include "TestManager.h"
#include "DeviceDriverTest.h"
#include "ConsoleReporter.h"

extern DeviceDriver *selectedDevices[];
extern DeviceDriverTest *selectedTests[];
//extern DeviceDriverTest *commonTests[];

DeviceTable *globalDeviceTable;

// --------------------------------------------------------

  TestManager::TestManager(const char *filename) {

  rpt = new ConsoleReporter();
  globalDeviceTable = new DeviceTable(selectedDevices,rpt);

  logger = new Logger("TestManager");
  logger->setCurrentLogLevel(LogLevel::DEBUG);

  theTestName = "Unnamed";
  testFailureCount = 0;
  groupFailureCount = 0;

}

TestManager::~TestManager() {
  if (theGroupName != 0) {
    free((void*)theGroupName);
  }
  if (theTestName !=0) {
    free((void*)theTestName);
  }
  if (logger != 0) {
    free(logger);
  }
}

// --------------------------------------------------------

void TestManager::runCommon() {
  // int idx = 0;
  // while (commonTests[idx] != 0) {
  //   commonTest[idx].execute(this, rpt);
  //   idx++;
  // }
}

void TestManager::runSelected() {
  int idx = 0;
  while (selectedTests[idx] != 0) {
    selectedTests[idx]->doTest(this, rpt);
    idx++;
  }
}

void TestManager::dispatchTimers() {
  globalDeviceTable->dispatchTimers();
}

// --------------------------------------------------------

void TestManager::countDown(int seconds) {
  delay(3000);
  Serial.println();
  Serial.print("Pre-test countdown: ");
  for (int idx = 0; idx < seconds; idx++) {
    Serial.print(seconds - idx);
    Serial.print(" ");
    delay(1000);
  }
  Serial.println();
}


void TestManager::beforeGroup(const char *groupName) {
  theGroupName = strdup(groupName);
  Serial.print("\n#Begin group ");
  Serial.println(theGroupName);
  groupFailureCount = 0;
  testFailureCount = 0;

};

void TestManager::beforeTest(const char *testName) {
  theTestName = strdup(testName);
  testFailureCount = 0;
  Serial.print("\n*Begin test ");
  Serial.println(theTestName);
}

void TestManager::afterTest() {
  Serial.print("*After test ");
  Serial.print(theTestName);
  Serial.print(". ");
  Serial.print(testFailureCount);
  Serial.println(" assertion failures.");
  groupFailureCount += testFailureCount;
  testFailureCount = 0;
}

void TestManager::afterGroup() {
  Serial.print("\n#After group ");
  Serial.print(theGroupName);
  Serial.print(". ");
  Serial.print(groupFailureCount);
  Serial.println(" cumulative assertion failures.\n");
  groupFailureCount = 0;
  testFailureCount = 0;
};

int TestManager::getTestFailureCount() {
  return testFailureCount;
}

int TestManager::getGroupFailureCount() {
  return groupFailureCount;
}

void TestManager::assertTrue(const char *msg, bool condition) {
  if (condition) return;
  logger->fatal(msg);
  testFailureCount += 1;
}

void TestManager::assertFalse(const char *msg, bool condition) {
  assertTrue(msg, !condition);
}
