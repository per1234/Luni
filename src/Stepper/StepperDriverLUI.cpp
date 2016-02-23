#include "StepperDriverLUI.h"

StepperDriverLUI::StepperDriverLUI() : LogicalUnitInfo() { }

StepperDriverLUI::~StepperDriverLUI() {}

AsyncStepper *StepperDriverLUI::getDeviceObject() {
  return motor;
}

void StepperDriverLUI::setDeviceObject(AsyncStepper *m) {
  motor = m;
}
