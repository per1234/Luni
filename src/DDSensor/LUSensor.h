#ifndef LUSensor_h
#define LUSensor_h

class LUSensor: public LogicalUnitInfo {

friend class DDSensor;

public:

  LUSensor() : LogicalUnitInfo() {}
  ~LUSensor() {}

private:

  int pin;
  bool attached = false;
  int resolution;

  bool isAttached() { return this->attached;}
  void attach(int pin) {this->pin = pin; this->attached = true;}
  void detach() {this->attached = false;}

};

#endif
