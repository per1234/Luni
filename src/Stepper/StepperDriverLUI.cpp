#include "StepperDriverLUI.h"

StepperDriverLUI::StepperDriverLUI() : LogicalUnitInfo() { }

StepperDriverLUI::~StepperDriverLUI() {}

AsyncStepper *StepperDriverLUI::getDeviceObject() {
  return motor;
}

void StepperDriverLUI::setDeviceObject(AsyncStepper *m) {
  motor = m;
}

void StepperDriverLUI::setRPMSpeed(int targetRPM) {
  speedRPM = targetRPM;
}

int StepperDriverLUI::getRPMSpeed() {
  return speedRPM;
}

void StepperDriverLUI::setRPMA(int targetRPMA) {
  accelRPM = targetRPMA;
}

int StepperDriverLUI::getRPMA() {
  return accelRPM;
}

void StepperDriverLUI::setRPMD(int targetRPMD) {
  decelRPM = targetRPMD;
}

int StepperDriverLUI::StepperDriverLUI::getRPMD(){
  return decelRPM;
}

void StepperDriverLUI::setRPMMax(int maxRPM) {
  speedMaxRPM = maxRPM;
}

int StepperDriverLUI::getRPMMax() {
  return speedMaxRPM;
}



