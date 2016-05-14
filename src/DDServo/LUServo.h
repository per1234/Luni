#ifndef LUServo_h
#define LUServo_h

#include <Servo.h>

class LUServo: public LogicalUnitInfo, Servo {

  friend class DDServo;

public:

  LUServo() {}
  ~LUServo() {}

private:
  int pin;
  int minPulse = MIN_PULSE_WIDTH;
  int maxPulse = MAX_PULSE_WIDTH;

  // Track the current continuous write profile

  int pulseIncrement;

  int currentStep = 0;
  int stepIncrement = +1;
  int stepCount = 10;

};

#endif
