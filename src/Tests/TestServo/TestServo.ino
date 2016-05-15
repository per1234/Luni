#include <LuniLib.h>
#include <TestFrame/DeviceDriverTest.h>

// Device Driver selection

#include <DDServo/DDServo.h>
#include <DDMeta/DDMeta.h>

DeviceDriver *selectedDevices[] = {
//  new DDMeta("Meta",1),
  new DDServo("Servo",1),
  0};

// Test selection

#include <TestBody/TestServo.h>
#include <TestBody/TestCommon.h>

DeviceDriverTest *selectedTests[] {
  new TestCommon("Servo:0"),
  new TestServo("Servo:0"),
  0
};

TestManager *tst = new TestManager(__FILE__);

// --------------------------------------------------------

void setup() {
  Serial.begin(115200);
  tst->countDown(5);
  tst->runSelected();
}

void loop() {
    tst->dispatchTimers();
}
