#include <FirmataDeviceLibrary.h>
#include <Device/DeviceTable.h>
#include <Framework/Logger.h>
#include <Framework/Tester.h>
#include <Framework/ByteOrder.h>
#include "SelectedDeviceDrivers.h"

/**
   Test the operation of the Hello World driver.
*/

#define BUF_SIZE 256
byte buf[BUF_SIZE];

DeviceTable *dt;
Tester *tst;
Logger *logger;

// --------------------------------------------------------

void setup() {

  // Countdown before running to give time to open the monitor window.

  Serial.begin(115200);

  delay(3000);
  for (int idx = 0; idx < 5; idx++) {
    Serial.print(5 - idx);
    Serial.print(" ");
    delay(1000);
  }
  Serial.println();

  // Load the device driver table

  dt = new DeviceTable(selectedDevices);

  //  Create the objects needed for the testing framework.

  tst = new Tester();
  logger = new Logger("DriverTestHello");
  logger->setCurrentLogLevel(LogLevel::DEBUG);

  // --------------------------------------------------------

  // Begin testing

  tst->beforeGroup("Hello");

  // --------------------------------------------------------

  tst->beforeTest("HelloOpenClose");
  int status = dt->open("HW:0", DDO_FORCE_OPEN);
  tst->assertTrue("Open error.", (status >= 0));
  if (status >= 0) {
    int handle = status;
    status = dt->close(handle);
    tst->assertTrue("Close error.", (status >= 0));
  }
  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("HelloRead");
  byte buf[256];
  status = dt->open("HW:0", 0);
  tst->assertTrue("Open error.", (status >= 0));

  if (status >= 0) {
    int handle = status;
    status = dt->read(handle, 256, buf);
    tst->assertTrue("Read error.", (status >= 0));

    logger->debug("Read n bytes.", status);
    status = dt->close(handle);
    tst->assertTrue("Close error.", (status >= 0));
  }
  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("HelloCDRDriverVersion");
  status = dt->open("HW:0");
  tst->assertTrue("Open error.", (status >= 0));

  int reg = static_cast<int>(CDR::DriverVersion);

  if (status >= 0) {
    int handle = status;
    status = dt->status(handle, reg, BUF_SIZE, buf);
    tst->assertTrue("Device status() Driver Version assertTrue:", (status >= 0));
    if (status >= 0) {
      logger->debug("Version data is n bytes. ", status);
      //      char *svString = SemVer::toString(buf);
      //      logger->debug("  Read n version bytes ", status,svString);
      //      free(svString);
    }
    status = dt->close(handle);
    tst->assertTrue("Close error.", (status >= 0));

  }
  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("HelloStatusMethod");
  status = dt->open("HW:0");
  tst->assertTrue("Open error.", (status >= 0));

  if (status >= 0) {
    int handle = status;

    for (int r = 0; r < 2; r++) {
      int count = BUF_SIZE;

      status = dt->status(handle, r, count, buf);
      tst->assertTrue("Device status() error.", (status >= 0));
      tst->assertTrue("Device status() actual < max assertTrue:", (status < count));
      if (status < 0) continue;

      switch (r) {
        
        case static_cast<int>(HelloRegister::INTERJECTION):
          logger->debug("Interjection byte count:", status);
          break;
          
        case static_cast<int>(HelloRegister::OBJECT):
          logger->debug("Object byte count:", status);
          break;
      }
    }
    status = dt->close(handle);
    tst->assertTrue("Close error.", (status >= 0));
  }
  tst->afterTest();

  // --------------------------------------------------------

  tst->afterGroup();

}

void loop() {}
