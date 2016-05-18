/**
 * This class ConsoleReporter is part of the test framework for the Luni
 * DeviceDriver library.  It is a subclass of ClientReporter, and is called
 * after each invocation of open(), read(), write(), and close().  This
 * implementation just writes strings to the Serial output port and assumes
 * that the user is looking at the console output.
 */

#include "ConsoleReporter.h"

const char *actionNames[] = {"OPEN","READ","WRITE","CLOSE"};

//---------------------------------------------------------------------------

ConsoleReporter::ConsoleReporter() {}

void ConsoleReporter::reportOpen(int status, int openOpts, int flags, const byte *buf) {
  sendDeviceResponse((int)(DAC::OPEN), status, openOpts, flags, 0, 0,buf);
}
/**
 * Translates a message from the DeviceDriver environment to a call to a Firmata-aware method.
 * @param status The status code or actual byte count associated with this read.
 * @param handle The handle of the unit doing the reply
 * @param reg The register identifier associated with the read() being reported
 * @param count The number of bytes that were requested.  May be less than or
 * equal to the byte count in status after a successful read.
 * @param buf The byte[] result of the read().
 */
void ConsoleReporter::reportRead(int status, int handle, int flags, int reg, int count, const byte *buf) {
  sendDeviceResponse((int)(DAC::READ), status, handle, flags, reg, count, buf);
}
/**
 * Translates a message from the DeviceDriver environment to a call to a Firmata-aware method.
 * @param status The status code or actual byte count associated with this write.
 * @param handle The handle of the unit doing the reply
 * @param reg The register identifier associated with the write() being reported
 * @param count The number of bytes that were requested.  May be less than or
 * equal to the byte count in status after a successful write.
 */
void ConsoleReporter::reportWrite(int status, int handle, int flags, int reg, int count) {
  sendDeviceResponse((int)(DAC::WRITE), status, handle, flags, reg, count);
}

void ConsoleReporter::reportClose(int status, int handle, int flags) {
  sendDeviceResponse((int)(DAC::CLOSE), status, handle, flags);
}

void ConsoleReporter::reportString(const byte *dataBytes) {
  Serial.print("String value: ");
  Serial.println((const char *)dataBytes);
}

void ConsoleReporter::reportError(int status) {
  sendDeviceResponse((int)(DAC::CLOSE), status);
}

void ConsoleReporter::reportClaimPin(int pin) {
  Serial.print("Event: Claim pin ");
  Serial.println(pin);
}

void ConsoleReporter::reportReleasePin(int pin) {
  Serial.print("Event: Release pin ");
  Serial.println(pin);
}


//---------------------------------------------------------------------------

/**
 * This method is called when there is a message to be sent back to the
 * client.  It may be in response to a DEVICE_REQUEST that was just
 * processed, or it may be an asynchronous event such as a stepper motor in
 * a new position or a continuous read data packet.
 *
 */
void ConsoleReporter::sendDeviceResponse(int action, int status, int handle, int flags, int reg, int count,
                                       const byte *dataBytes) {

  int rawCount;

  Serial.print("Action: ");
  Serial.print(actionNames[action]);

  Serial.print(", status: ");
  Serial.print(status);

  Serial.print(", handle: ");
  Serial.print(highByte(handle));
  Serial.print("/");
  Serial.print(lowByte(handle));

  Serial.print(", flags: ");
  Serial.print(flags);

  Serial.print(", reg: ");
  Serial.print(reg);

  Serial.print(", count: ");
  Serial.print(count);

  if (dataBytes != 0 && status >= 0) {
    Serial.print(", dataBytes: ");
    if (action == (int)(DAC::OPEN)) {
      rawCount = strlen((const char *)dataBytes)+1;
    } else if (action == (int)(DAC::READ) && status > 0) {
      rawCount = status;
    }
    for (int idx = 0; idx < rawCount; idx++) {
      Serial.print(" ");
      Serial.print(dataBytes[idx]);
    }
  }
  Serial.println();
}
