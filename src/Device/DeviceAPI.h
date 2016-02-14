#ifndef DeviceAPI_h
#define DeviceAPI_h

#include <SemanticVersion.h>
#include "DeviceError.h"

/**
 * This abstract class defines the common methods available to any caller of
 * a concrete DeviceAPI subclass.  The subclasses are the means of implementing
 * methods that call the standard device driver methods to implement higher
 * level or less complex access to capabilities.
 */

  //-------------------------------------------------------------------------

class DeviceAPI {

public:
  virtual int getDriverVersion(int handle, SemanticVersion * ver) = 0;
  virtual int getAPIVersion(SemanticVersion *ver) = 0;

private:

};
#endif
