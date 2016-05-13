#include <LuniLib.h>
#include <DDServo/DDServo.h>
#include "TestServo.h"

TestServo::TestServo(const char * unitID) : DeviceDriverTest(__func__, unitID) {

}

void TestServo::doTest(TestManager *tst, ClientReporter *rpt, Logger *log) {

  int flags;
  int handle;
  int openOpts;
  int reg;
  int count;
  int status;

  #define BUF_SIZE 256
  byte datablock[BUF_SIZE];

  // Begin testing

  tst->beforeGroup(groupID);

  // --------------------------------------------------------

  tst->beforeTest("Open");

  flags  = (int)(DAF::FORCE);
  openOpts = 0;
  strlcpy((char *)datablock,unitID,BUF_SIZE);

  status = globalDeviceTable->open(openOpts, flags, (const char *)datablock);
  rpt->reportOpen(status, openOpts, flags, datablock);
  if (status > 0) {
    rpt->reportString(datablock);
  }

  tst->assertTrue("Open error.", (status >= 0));

  handle = status;
  tst->afterTest();

  // =============================================================
  // Open.  Put device specific one-shot tests below this comment.
  // =============================================================



  // =============================================================
  // Close.  Put device specific one-shot tests above this comment.
  // =============================================================

  tst->beforeTest("Close");
  flags = (int)(DAF::NONE);
  status = globalDeviceTable->close(handle, flags);
  rpt->reportClose(status, handle, flags);
  tst->assertTrue("Close error.", (status >= 0));
  tst->afterTest();

  // =============================================================
  // Put device specific continuous tests between here and the end.
  // =============================================================


  // --------------------------------------------------------

  tst->afterGroup();

}


