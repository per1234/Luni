#ifndef StepperDriverLUI_h
#define StepperDriverLUI_h

#include <Device/LogicalUnitInfo.h>
#include <AsyncStepper.h>

class StepperDriverLUI: public LogicalUnitInfo {

    friend class StepperDriver;

public:

    StepperDriverLUI();
    ~StepperDriverLUI();

    AsyncStepper *getDeviceObject();
    void setDeviceObject(AsyncStepper *motor);

private:

    AsyncStepper *motor;

    /**
     * SpeedRPM is the desired constant speed in Revolutions Per Minute (RPM).
     * For a 200-step per revolution motor, typical RPM values are in the
     * range 1..600.  This yields an equivalent Steps Per Second range of 3..2000
     * SPS and step time targets of 1,000,000 .. 500 microseconds per step.
     *
     * Positive is clockwise, negative is counter-clockwise.
     */

    long speedRPM;
    long accelRPM;
    long decelRPM;
    long speedMaxRPM;

    byte buf[2];

};

#endif
