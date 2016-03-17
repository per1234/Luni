#include <LuniLib.h>

// Device Drivers

#include <Hello/HelloDriver.h>
#include <Peek/DDPeek.h>
#include <MCP9808/MCP9808Driver.h>

DeviceDriver *selectedDevices[] = {
  new HelloDriver("HW"),
  new DDPeek("PK"),
  new MCP9808Driver("TC",2),
  0};

