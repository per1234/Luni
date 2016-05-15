#include <LuniLib.h>
#include <TestFrame/DeviceDriverTest.h>

// Device Driver selection

#include <DDHello/DDHello.h>
#include <DDMeta/DDMeta.h>

DeviceDriver *selectedDevices[] = {
  new DDHello("HW",1),
  new DDMeta("Meta",1),
  0};

// Test selection

#include <TestBody/TestHello.h>
#include <TestBody/TestMeta.h>
#include <TestBody/TestCommon.h>

DeviceDriverTest *selectedTests[] {
  new TestCommon("Meta:0"),
  new TestMeta("Meta:0"),
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
