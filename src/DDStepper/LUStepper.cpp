#include "LUStepper.h"

LUStepper::LUStepper(byte interface, int steps_per_rev,
                     byte pin1, byte pin2, byte pin3, byte pin4) :
    AsyncStepper(interface, steps_per_rev, pin1, pin2, pin3, pin4)
{ }

LUStepper::~LUStepper() {}
