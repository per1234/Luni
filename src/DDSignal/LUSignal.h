#ifndef LUSignal_h
#define LUSignal_h

#include "DDSignal.h"

class LUSignal: public LogicalUnitInfo {

friend class DDSignal;

public:

  LUSignal() {}
  ~LUSignal() {}

private:
  int pin;
  int minPulse = MIN_PULSE_WIDTH;
  int maxPulse = MAX_PULSE_WIDTH;

};

#endif
