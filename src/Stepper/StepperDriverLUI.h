#ifndef StepperDriverLUI_h
#define StepperDriverLUI_h

class StepperDriverLUI: public LogicalUnitInfo {

public:

    StepperDriverLUI();
    ~StepperDriverLUI();

    AsyncStepper *getDeviceObject();

private:

    AsyncStepper motor;
};

#endif
