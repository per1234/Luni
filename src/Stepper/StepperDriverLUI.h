#ifndef StepperDriverLUI_h
#define StepperDriverLUI_h

#include <Device/LogicalUnitInfo.h>
#include <AsyncStepper.h>

class StepperDriverLUI: public LogicalUnitInfo {

public:

    StepperDriverLUI();
    ~StepperDriverLUI();

    AsyncStepper *getDeviceObject();
    void setDeviceObject(AsyncStepper *motor);

    //----- Motor speed and acceleration---------------------------------------

    /**
     * Set the desired constant speed in Revolutions Per Minute (RPM).
     * For a 200-step per revolution motor, typical RPM values are in the
     * range 1..600.  This yields an equivalent Steps Per Second range of 3..2000
     * SPS and step time targets of 1,000,000 .. 500 microseconds per step.
     *
     * @param targetRPM Desired speed in RPM.  Positive is clockwise,
     * negative is counter-clockwise.
     */

    void setRPMSpeed(int targetRPM);
    int getRPMSpeed();

    void setRPMA(int targetRPMA);
    int getRPMA();

    void setRPMD(int targetRPMD);
    int getRPMD();

    void setRPMMax(int maxRPM);
    int getRPMMax();

private:

    AsyncStepper *motor;

    int speedRPM;
    int accelRPM;
    int decelRPM;
    int speedMaxRPM;

};

#endif
