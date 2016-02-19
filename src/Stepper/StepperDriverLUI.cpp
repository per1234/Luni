#include "StepperDriverLUI.h"

StepperDriverLUI::StepperDriverLUI() : LogicalUnitInfo() { }

StepperDriverLUI::~StepperDriverLUI() {}

AsyncStepper *StepperDriverLUI::getDeviceObject() {
  return &motor;
}
