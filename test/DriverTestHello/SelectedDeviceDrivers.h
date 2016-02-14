#include <FirmataDeviceLibrary.h>

// Device Drivers

#include <Hello/HelloDriver.h>

DeviceDriver *selectedDevices[] = {
  new HelloDriver("HW",2),
  0};

