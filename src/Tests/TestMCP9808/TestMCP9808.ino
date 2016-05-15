#include <LuniLib.h>
#include <TestFrame/DeviceDriverTest.h>

// Device Driver selection

#include <DDMCP9808/DDMCP9808.h>
#include <DDMeta/DDMeta.h>

DeviceDriver *selectedDevices[] = {
  new DDMeta("Meta",1),
  new DDMCP9808("MCP9808",1,0x18),
  0};

// Test selection

#include <TestBody/TestMCP9808.h>
#include <TestBody/TestCommon.h>

DeviceDriverTest *selectedTests[] {
  new TestCommon("MCP9808:0"),
  new TestMCP9808("MCP9808:0"),
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
