#include <FirmataDeviceLibrary.h>

// Device Drivers

#include <Hello/HelloDriver.h>
#include <Peek/PeekDriver.h>
#include <MCP9808/MCP9808Driver.h>

DeviceDriver *selectedDevices[] = {
  new HelloDriver("HW"),
  new PeekDriver("PK"),
  new MCP9808Driver("TC",2),
  0};

