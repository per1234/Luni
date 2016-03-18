#ifndef LogicalUnitInfo_h
#define LogicalUnitInfo_h

class LogicalUnitInfo {

  friend class DeviceDriver;
  friend class DDMeta;              // in order to have access to the intervalTimes

public:
  LogicalUnitInfo();
  virtual ~LogicalUnitInfo();

private:

  unsigned long intervalTime[2];    // desired length of time between calls to update() and report()
  unsigned long previousTime[2];    // the time the last interval expired
  unsigned long currentTime[2];     // the current values from micros() and millis()
  unsigned long deltaTime[2];       // amount of time since last interval expired

};

#endif
