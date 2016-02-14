#include <FirmataDeviceLibrary.h>

// Device Drivers

#include <MCP9808/MCP9808Driver.h>

DeviceDriver *selectedDevices[] = {
  new MCP9808Driver("TempSensor",2,0x18),
  0};

