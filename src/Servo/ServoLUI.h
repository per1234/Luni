#ifndef ServoLUI_h
#define ServoLUI_h

#include <Servo.h>

class ServoLUI: public LogicalUnitInfo, Servo {

friend class ServoDriver;

public:

  ServoLUI() {}

  ~ServoLUI() {}

private:

  int fromLow = 0;
  int fromHigh = 180;
  int pin;

};

#endif
