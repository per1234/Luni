#ifndef MCP9808_h
#define MCP9808_h

enum class MCP9808Register : int {
  RESERVED = 0,
  CONFIG = 1,
  UPPER_TEMP = 2,
  LOWER_TEMP = 3,
  CRIT_TEMP = 4,
  AMBIENT_TEMP = 5,
  MANUF_ID = 6,
  DEVICE_ID = 7,
  RESOLUTION = 8
};

#endif
