#ifndef LogicalUnitInfo_h
#define LogicalUnitInfo_h

#include <arduino.h>

#define LUI_RESPONSE_BUFFER_SIZE 32

class LogicalUnitInfo {

  friend class DeviceDriver;
  friend class DDMeta;              // in order to have access to the intervalTimes

public:
  LogicalUnitInfo();
  virtual ~LogicalUnitInfo();

  struct RateAction {
    bool enabled;
    int action;
    int handle;
    int flags;
    int reg;
    int count;
    byte responseBuffer[LUI_RESPONSE_BUFFER_SIZE];
  };

  RateAction microAction;
  RateAction milliAction;

private:

  unsigned long intervalTime[2];    // desired length of time between calls to update() and report()
  unsigned long previousTime[2];    // the time the last interval expired
  unsigned long currentTime[2];     // the current values from micros() and millis()
  unsigned long deltaTime[2];       // amount of time since last interval expired
};

#endif
