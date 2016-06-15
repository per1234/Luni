#include "DDSignal.h"

//---------------------------------------------------------------------------

extern DeviceTable *gDeviceTable;

/**
 * This DDSignal device driver provides basic analog and digital I/O functions.
 *
 * Values are collected on a channel by channel basis.  A single logical unit
 * addresses a channel group, one or more channels pointing in the same
 * direction (INPUT or OUTPUT).
 */
DDSignal::DDSignal(const char *dName, int lunCount) :
  DeviceDriver(dName, lunCount) {
  DEFINE_VERSION(0, 10, 0)
}

//---------------------------------------------------------------------------

int DDSignal::open(int opts, int flags, const char *name) {
  int lun;
  int status = DeviceDriver::open(opts, flags, name);
  if (status < 0) {
    return status;
  }

  lun = getUnitNumber(status);
  LUSignal *currentUnit = new LUSignal();
  if (currentUnit == 0) {
    return ENOMEM;
  }

  currentUnit->channelCount = 0;

  logicalUnits[lun] = currentUnit;
  return lun;
}

//---------------------------------------------------------------------------

int DDSignal::read(int handle, int flags, int reg, int count, byte *buf) {

  int bufIndex;

   // First, handle registers that can be read even before a connection
   // has been made. Note that currently the only source of handle-less
   // operations is the Meta device driver.  All other clients must do
   // an open() (ie, get a handle) before doing any reads or writes.

  switch (reg) {

  case (int)(CDR::DriverVersion):
    return DeviceDriver::buildVersionResponse(count, buf);

  case (int)(CDR::UnitNamePrefix):
      return DeviceDriver::buildPrefixResponse(count,buf);
  }

  //  Second, registers for which we must have a handle (ie, open() has
  //  been done) but we don't need to have any defined channels

  int lun = getUnitNumber(handle);
  if (lun < 0 || lun >= logicalUnitCount) return EINVAL;
  LUSignal *currentUnit = static_cast<LUSignal *>(logicalUnits[lun]);
  if (currentUnit == 0) return ENOTCONN;

  if (count < 0) return EINVAL;

  switch (reg) {
  case (int)(CDR::Intervals):
    return DeviceDriver::readIntervals(handle, flags, reg, count, buf);
  }

  //  Third, registers for which we must have a handle (ie, open() has
  //  been done) AND we need to have at least one channel defined

  if (currentUnit->channelCount == 0) return ENODATA;

  switch (reg) {
  case (int)(REG::CHANNEL_VALUES):
    if (currentUnit->direction != INPUT) {
      return ENOTSUP;
    }
    if (count < 2*currentUnit->channelCount) return EMSGSIZE;

    bufIndex = 0;
    fromHostTo8LE(currentUnit->channelCount,buf + bufIndex++);
    for (int c=0; c < currentUnit->channelCount; c++) {
      if (currentUnit->channel[c].op = OP_DIGITAL) {
        fromHostTo16LE(digitalRead(currentUnit->channel[c].pin),buf + bufIndex);
      } else {
        fromHostTo16LE(analogRead(currentUnit->channel[c].pin),buf + bufIndex);
      }
      bufIndex += 2;
    }
    return bufIndex;

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

//---------------------------------------------------------------------------

int DDSignal::write(int handle, int flags, int reg, int count, byte *buf) {
  int numChannels;
  int bufIndex;
  bool isNewLock;

  // First, handle registers that can be written even before a connection
  // has been made.

  // ... No applicable registers ...

  //  Second, handle registers for which we must have a handle (ie, open() has
  //  been done) but we don't need to have any defined channels

  int lun = getUnitNumber(handle);
  if (lun < 0 || lun >= logicalUnitCount) return EINVAL;
  LUSignal *currentUnit = static_cast<LUSignal *>(logicalUnits[lun]);
  if (currentUnit == 0) return ENOTCONN;

  switch (reg) {

  case (int)(CDR::Intervals):
    return DeviceDriver::writeIntervals(handle, flags, reg, count, buf);

  case (int)(CDR::Configure):
    if (count < 5) return EMSGSIZE;
    bufIndex = 0;
    currentUnit->direction = from8LEToHost(buf+bufIndex++);
    currentUnit->channelCount = 0;
    numChannels = from8LEToHost(buf+bufIndex++);
    if (numChannels > MAX_CHANNELS_PER_UNIT) {
      return EINVAL;
    }
    for (int c=0; c < numChannels; c++) {
      currentUnit->channel[c].op = from8LEToHost(buf+bufIndex++);
      currentUnit->channel[c].config = from8LEToHost(buf+bufIndex++);
      currentUnit->channel[c].pin = from8LEToHost(buf+bufIndex++);
      isNewLock = currentUnit->lockPin(currentUnit->channel[c].pin);
      if (!isNewLock) return EBUSY;
    }

    if (currentUnit->direction == INPUT) {
      for (int c=0; c < numChannels; c++) {
        switch (currentUnit->channel[c].op) {
          case OP_DIGITAL:
            pinMode(currentUnit->channel[c].pin,currentUnit->channel[c].config);
            break;
          case OP_ANALOG:
            pinMode(currentUnit->channel[c].pin,INPUT);
            break;
        }
      }
    } else if (currentUnit->direction == OUTPUT) {
      for (int c=0; c < numChannels; c++) {
        switch (currentUnit->channel[c].op) {
          case OP_DIGITAL:
            pinMode(currentUnit->channel[c].pin,OUTPUT);
            break;
          case OP_ANALOG:
            pinMode(currentUnit->channel[c].pin,OUTPUT);
            break;
        }
      }
    } else {
      return EINVAL;  // direction has to be INPUT or OUTPUT
    }
    currentUnit->channelCount = numChannels;
    return bufIndex;
  }

  //  Third, registers for which we must have a handle (ie, open() has
  //  been done) AND we need to have at least one channel defined

  if (currentUnit->channelCount == 0) return ENODATA;

  // Enable continuous write, if requested

  // if (flags == (int)DAF::MILLI_RUN) {
  //   currentUnit->pulseIncrement = (currentUnit->maxPulse-currentUnit->minPulse) / currentUnit->stepCount;
  //   currentUnit->currentStep = 0;
  //   DeviceDriver::milliRateRun((int)DAC::WRITE, handle, flags, reg, count,buf);
  // } else if (flags == (int)DAF::MILLI_STOP) {
  //   DeviceDriver::milliRateStop((int)DAC::WRITE, handle, flags, reg, count,buf);
  // }

  switch (reg) {
  case (int)(REG::CHANNEL_VALUES):
    bufIndex = 0;

    if (currentUnit->direction != OUTPUT) {
      return ENOTSUP;
    }
    if (count < 2*currentUnit->channelCount) return EMSGSIZE;
    numChannels = from8LEToHost(buf + bufIndex++);
    if (numChannels != currentUnit->channelCount) return EMSGSIZE;

    for (int c=0; c < currentUnit->channelCount; c++) {
      if (currentUnit->channel[c].op = OP_DIGITAL) {
        digitalWrite(currentUnit->channel[c].pin,from16LEToHost(buf + bufIndex));
      } else {
        analogWrite(currentUnit->channel[c].pin,from16LEToHost(buf + bufIndex));
      }
      bufIndex += 2;
    }
    return bufIndex;

  default:
    return ENOTSUP;
  }
  return EPANIC;
}

//---------------------------------------------------------------------------

int DDSignal::close(int handle, int flags) {
  int lun = getUnitNumber(handle);
  if (lun < 0 || lun >= logicalUnitCount) return EINVAL;
  LUSignal *currentUnit = static_cast<LUSignal *>(logicalUnits[lun]);
  if (currentUnit == 0) return ENOTCONN;

  if (currentUnit->channelCount > 0) {
    for (int c=0; c < channelCount; c++) {
      currentUnit->unlockPin(currentUnit->channel[c].pin);
    }
  }
  return DeviceDriver::close(handle, flags);
}

//---------------------------------------------------------------------------

// If a continuous write has been requested, process it.

// One use of continuous write for this device is to have the servo follow a
// position profile.  The default, and currently the only, profile is a simple
// sweep between limits, but function tables or other interesting algorithms
// would be easy to implement for servos and other devices.

int DDSignal::processTimerEvent(int lun, int timerSelector, ClientReporter *report) {
  int nextStep;
  int nextPulse;
  int status;

  LUSignal *cU = static_cast<LUSignal *>(logicalUnits[getUnitNumber(lun)]);
  if (cU == 0) return ENOTCONN;

  int h = cU->eventAction[1].handle;
  int f = cU->eventAction[1].flags;
  int r = cU->eventAction[1].reg;
  int c = min(cU->eventAction[1].count,QUERY_BUFFER_SIZE);

  // Is it time to do another write?
  // If so, calculate new position and set it.

  if ((timerSelector == 1) && (cU->eventAction[1].enabled)) {
    if ((cU->eventAction[1].action) == (int)(DAC::WRITE))  {
      switch (cU->eventAction[1].reg) {
      case (int)(REG::POSITION_MICROSECONDS):
        nextStep = cU->currentStep + cU->stepIncrement;
        if ((nextStep < 0) || (nextStep == cU->stepCount)) {
          cU->stepIncrement = -cU->stepIncrement;
          nextStep = cU->currentStep + cU->stepIncrement;
        }
        cU->currentStep = nextStep;
        nextPulse = cU->minPulse + (nextStep * cU->pulseIncrement);
        fromHostTo16LE(nextPulse,cU->eventAction[1].queryBuffer);
        f = 0;
        c = 2;
        r = (int)(REG::POSITION_MICROSECONDS);
        status = gDeviceTable->write(h, f, r, c, cU->eventAction[1].queryBuffer);
        report->reportWrite(status, h, f, r, c);
        return status;
      }
    }
  }
  return ESUCCESS;
}
