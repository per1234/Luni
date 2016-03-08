#ifndef Reporter_h
#define Reporter_h

#include "DeviceDriver.h"

/**
 * This abstract class declares method signatures for any class that would like
 * to be the channel back to the "client", either on a remote client system,
 * the server system itself or whatever.
 *
 * The main idea is to get around the problem of having Firmata related code
 * way down in the guts of the device drivers in the timer based methods.
 * Using this class, any sort of client-facing asynchronouse message processing
 * can be supported without affecting the device drivers.
 */
class ClientReporter {
public:

    virtual void reportOpen(int status) = 0;
    virtual void reportRead(int status, int handle, int reg, int count, const byte *buf) = 0;
    virtual void reportWrite(int status, int handle, int reg, int count) = 0;
    virtual void reportClose(int status, int handle) = 0;
    virtual void reportError(int status) = 0;
};

#endif
