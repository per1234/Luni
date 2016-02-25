#ifndef Stepper_h
#define Stepper_h

#include <Device/DeviceDriver.h>

// Stepper register definitions

enum class Stepper : int {
    MoveRelative = 0,
    RPMSpeeds = 1,
    PositionEvent = 2
};

// Stepper interface definitions

enum class StepperInterface : int {
    EZStepper = 0,
    TWO_WIRE = 1,
    FOUR_WIRE = 2
};


#endif
