#include "DDServo.h"

//---------------------------------------------------------------------------


/**
 * This device driver is for servo controllers.  It uses the basic servo
 * library supplied with Arduino.
 *
 * Information needed by the device driver about each attached servo is held in
 * an object of the LUServo class.  The LUServo class extends the Servo class
 * that the library itself uses for maintaining the detailed state of each
 * attached servo.
 *
 * Because the library uses up a slot index every time a Servo object is allocated,
 * and has no way to reclaim slot indexes no longer in use, this device driver
 * constructor allocates an LUServo object for every available lun and then just
 * points to them in the logicalUnits pointer array when needed for open() and
 * zeroes out the pointer for close().
 *
 * If the memory allocations fail, logicalUnitCount is set to 0 and all calls to
 * the open() method for this device will fail with ENXIO, No such device or address.
 */
DDServo::DDServo(const char *dName, int lunCount) :
  DeviceDriver(dName, lunCount) {
  DEFINE_VERSION_PRE(0, 8, 0, beta)

  if (logicalUnitCount > 0) {
    servos = new LUServo[logicalUnitCount];
    if (servos == 0) {
      delete logicalUnits;
      logicalUnitCount = 0;
    }
  }
  for (int idx = 0; idx < logicalUnitCount; idx++) {
    logicalUnits[idx] = 0;
  }
}

//---------------------------------------------------------------------------

int DDServo::open(int opts, int flags, const char *name) {
  int lun;
  int status = DeviceDriver::open(opts, flags, name);
  if (status < 0) {
    return status;
  }

  lun = status;
  LUServo *currentUnit = &servos[lun];

  currentUnit->detach();
  currentUnit->minPulse = MIN_PULSE_WIDTH;
  currentUnit->maxPulse = MAX_PULSE_WIDTH;

  logicalUnits[lun] = currentUnit;
  return lun;
}

//---------------------------------------------------------------------------

int DDServo::read(int handle, int flags, int reg, int count, byte *buf) {

  // Registers for which no handle is needed

  switch (reg) {

  case (int)(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(count, buf);

  case (int)(CDR::UnitNamePrefix):
      return DeviceDriver::buildPrefixResponse(count,buf);

  }

  //  Registers for which we must have a handle (ie, open() has
  //  been done) but we don't need to be attached to a pin

  LUServo *currentUnit = static_cast<LUServo *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;
  if (count < 0) return EINVAL;

  switch (reg) {

    case (int)(CDR::Intervals):
      return DeviceDriver::readIntervals(handle, flags, reg, count, buf);

  }

  //  Registers for which we must have a handle (ie, open() has
  //  been done) and we need to be attached to a pin (and thus
  //  a servo)

  if (!currentUnit->attached()) return ENODATA;

  switch (reg) {

  case (int)(DDServo::REG::PIN):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(currentUnit->pin, buf);
    return 2;

  case (int)(DDServo::REG::RANGE_MICROSECONDS):
    if (count < 4) return EMSGSIZE;
    fromHostTo16LE(currentUnit->minPulse, buf);
    fromHostTo16LE(currentUnit->maxPulse, buf + 2);
    return 4;

  case (int)(DDServo::REG::POSITION_DEGREES):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(currentUnit->read(), buf);
    return 2;

  case (int)(DDServo::REG::POSITION_MICROSECONDS):
    if (count < 2) return EMSGSIZE;
    fromHostTo16LE(currentUnit->readMicroseconds(), buf);
    return 2;

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

//---------------------------------------------------------------------------

int DDServo::write(int handle, int flags, int reg, int count, byte *buf) {
  int thePin;
  uint8_t channel;
  int loPulse;
  int hiPulse;
  int pos;

  LUServo *currentUnit = static_cast<LUServo *>(logicalUnits[getUnitNumber(handle)]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(DDServo::REG::PIN):
    if (count != 2) return EMSGSIZE;
    thePin = from16LEToHost(buf);
    if (!(IS_PIN_PWM(thePin))) return EINVAL;

    if (currentUnit->attached()) {
      currentUnit->detach();
    }
    channel = currentUnit->attach(thePin, currentUnit->minPulse, currentUnit->maxPulse);
    if (channel == 255) return EMFILE;
    currentUnit->pin = thePin;
    return 2;

  case (int)(DDServo::REG::RANGE_MICROSECONDS):
    if (count != 4) return EMSGSIZE;
    loPulse = from16LEToHost(buf);
    hiPulse = from16LEToHost(buf + 2);
    if (currentUnit->attached()) {
      currentUnit->detach();
      channel = currentUnit->attach(currentUnit->pin, loPulse, hiPulse);
      if (channel == 255) return EMFILE;
    }
    currentUnit->minPulse = loPulse;
    currentUnit->maxPulse = hiPulse;
    return 4;

  case (int)(DDServo::REG::POSITION_DEGREES):
    if (count != 2) return EMSGSIZE;
    pos = from16LEToHost(buf);
    currentUnit->write(pos);
    return 2;

  case (int)(DDServo::REG::POSITION_MICROSECONDS):
    if (count != 2) return EMSGSIZE;
    pos = from16LEToHost(buf);
    currentUnit->writeMicroseconds(pos);
    return 2;

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

//---------------------------------------------------------------------------

int DDServo::close(int handle, int flags) {
  int lun = getUnitNumber(handle);
  LUServo *currentUnit = static_cast<LUServo *>(logicalUnits[lun]);
  currentUnit->detach();
  logicalUnits[lun] = 0;
  return DeviceDriver::close(handle, flags);
}
