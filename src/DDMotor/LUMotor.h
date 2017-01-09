#ifndef LUMotor_h
#define LUMotor_h

class LUMotor: public LogicalUnitInfo {

  friend class DDMotor;

public:

  LUMotor() {}
  ~LUMotor() {}
  void run(bool enable);
  void setSpeed(uint8_t s);
  void setDirection(uint8_t d)

private:
  int modePin[2];       // each motor has two mode control pins
  int pwmPin;          // each motor has one PWM speed control pin

};

#endif
