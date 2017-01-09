#ifndef LUSignal_h
#define LUSignal_h

#define MAX_CHANNELS_PER_UNIT  8

class DDSignal;

struct Channel {
public:
  byte op;        // digital, analog
  byte config;
  byte digitalPin;
};

class LUSignal: public LogicalUnitInfo {

  friend class DDSignal;

public:

  LUSignal() {}
  ~LUSignal() {}

private:
  Channel channel[MAX_CHANNELS_PER_UNIT];
  int channelCount;  // number of defined channels in this group
  int direction;     // Input, Output

};

#endif
