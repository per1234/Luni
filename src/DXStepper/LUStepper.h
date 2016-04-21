#ifndef LUStepper_h
#define LUStepper_h

#include <Device/LogicalUnitInfo.h>
#include <AsyncStepper.h>

class LUStepper: public LogicalUnitInfo, AsyncStepper {

    friend class DDStepper;

public:

    LUStepper(byte interface = AsyncStepper::DRIVER,
                   int steps_per_rev = 200,
                   byte pin1 = 2,
                   byte pin2 = 3,
                   byte pin3 = 4,
                   byte pin4 = 5);
    ~LUStepper();

private:

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
