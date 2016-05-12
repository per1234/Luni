#include <LuniLib.h>
#include "DeviceDriverTest.h"

DeviceDriverTest::DeviceDriverTest(const char *group, const char *unit) {
  groupID = (char *)group;
  unitID = (char *)unit;
}
