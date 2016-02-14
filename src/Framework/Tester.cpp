#include "Tester.h"

Tester::Tester() {
  theTestName = "Unnamed";
  testFailureCount = 0;
  groupFailureCount = 0;
  logger = new Logger("Tester log");
}

Tester::~Tester() {
  if (theGroupName != 0) {
    free(theGroupName);
  }
  if (theTestName !=0) {
    free(theTestName);
  }
  if (logger != 0) {
    free(logger);
  }
}

void Tester::beforeGroup(const char *groupName) {
  theGroupName = strdup(groupName);
  Serial.print("\n#Begin group ");
  Serial.println(theGroupName);
  groupFailureCount = 0;
  testFailureCount = 0;

};

void Tester::beforeTest(const char *testName) {
  theTestName = strdup(testName);
  testFailureCount = 0;
  Serial.print("\n*Begin test ");
  Serial.println(theTestName);
}

void Tester::afterTest() {
  Serial.print("*After test ");
  Serial.print(theTestName);
  Serial.print(". ");
  Serial.print(testFailureCount);
  Serial.println(" assertion failures.");
  groupFailureCount += testFailureCount;
  testFailureCount = 0;
}

void Tester::afterGroup() {
  Serial.print("\n#After group ");
  Serial.print(theGroupName);
  Serial.print(". ");
  Serial.print(groupFailureCount);
  Serial.println(" cumulative assertion failures.\n");
  groupFailureCount = 0;
  testFailureCount = 0;
};

int Tester::getTestFailureCount() {
  return testFailureCount;
}

int Tester::getGroupFailureCount() {
  return groupFailureCount;
}

void Tester::assertTrue(const char *msg, bool condition) {
  if (condition) return;
  logger->fatal(msg);
  testFailureCount += 1;
}

void Tester::assertFalse(const char *msg, bool condition) {
  assertTrue(msg, !condition);
}
