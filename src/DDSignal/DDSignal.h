#ifndef DDSignal_h
#define DDSignal_h

#include <Device/DeviceDriver.h>
#include <Device/DeviceTable.h>
#include "LUSignal.h"
#include <utility/Boards.h>

#define OP_ANALOG  0  // pin operating in analog mode
#define OP_DIGITAL 1  // pin operating in digital mode

/**
 * @ingroup DDImplementation
 */
class DDSignal: public DeviceDriver {

public:
  DDSignal(const char *unitName = "Signal", int count = 1);

  int open(int opts, int flags, const char *name);
  int read(int handle, int flags, int reg, int count, byte *buf);
  int write(int handle, int flags, int reg, int count, byte *buf);
  int close(int handle, int flags);
  int processTimerEvent(int lun, int timerSelector, ClientReporter *report);

  enum class REG : int {
    CHANNEL_VALUES = 256
  };

private:

};


#endif
