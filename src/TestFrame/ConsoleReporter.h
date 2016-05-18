#ifndef ConsoleReporter_h
#define ConsoleReporter_h

#include <Device/ClientReporter.h>

/**
 * This class performs a simple serial reporter function for standalone
 * testing of DeviceDriver modules.
 */
class ConsoleReporter: public ClientReporter {

public:

  ConsoleReporter();

  void reportOpen(int status, int opts, int flags, const byte *buf);
  void reportRead(int status, int handle, int flags, int reg, int count, const byte *dataBytes);
  void reportWrite(int status, int handle, int flags,  int reg, int count);
  void reportClose(int status, int handle, int flags );
  void reportString(const byte *dataBytes);
  void reportError(int status);
  void reportClaimPin(int pin);
  void reportReleasePin(int pin);

private:

    void sendDeviceResponse(int action, int status, int handle = 0, int flags = 0, int reg = 0, int count = 0, const byte *dataBytes = 0);

};

#endif
