#include <LuniLib.h>
#include <DDHello/DDHello.h>
#include "TestHello.h"

TestHello::TestHello(const char * unitID) : DeviceDriverTest(__func__, unitID) {

}

void TestHello::doTest(TestManager *tst, ClientReporter *rpt, Logger *log) {

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

  status = gDeviceTable->open(openOpts, flags, (const char *)datablock);
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

  tst->beforeTest("ReadInterjection");

  flags = (int)(DAF::NONE);
  reg = (int)(DDHello::REG::INTERJECTION);
  status = gDeviceTable->read(handle, flags, reg, BUF_SIZE, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  if (status > 0) {
    rpt->reportString(datablock);
  }

  tst->assertTrue("Read error.", (status >= 0));

  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("ReadObject");

  flags = (int)(DAF::NONE);
  reg = (int)(DDHello::REG::OBJECT);
  status = gDeviceTable->read(handle, flags, reg, BUF_SIZE, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  if (status > 0) {
    rpt->reportString(datablock);
  }

  tst->assertTrue("Read error.", (status >= 0));

  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("ReadStreamOnce");

  flags = (int)(DAF::NONE);
  reg = (int)(CDR::Stream);
  status = gDeviceTable->read(handle, flags, reg, BUF_SIZE, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  if (status > 0) {
    rpt->reportString(datablock);
  }

  tst->assertTrue("Read error.", (status >= 0));

  tst->afterTest();

  // =============================================================
  // Close.  Put device specific one-shot tests above this comment.
  // =============================================================

  tst->beforeTest("Close");

  flags = (int)(DAF::NONE);
  status = gDeviceTable->close(handle, flags);
  rpt->reportClose(status, handle, flags);

  tst->assertTrue("Close error.", (status >= 0));

  tst->afterTest();

  // =============================================================
  // Put device specific continuous tests between here and the end.
  // =============================================================

  tst->beforeTest("ReadStreamContinuous");

  // Open

  flags  = (int)(DAF::NONE);
  openOpts = 0;
  strlcpy((char *)datablock,unitID,BUF_SIZE);

  status = gDeviceTable->open(openOpts, flags, (const char *)datablock);
  rpt->reportOpen(status, openOpts, flags, datablock);
  handle = status;

  // Write Intervals

  flags = (int)(DAF::NONE);
  reg = (int)(CDR::Intervals);
  fromHostTo32LE(0UL,datablock);
  fromHostTo32LE(5000UL,datablock+4);
  count = 8;
  status = gDeviceTable->write(handle, flags, reg, count, datablock);
  rpt->reportWrite(status, handle, flags, reg, count);

  // Read Intervals

  flags = (int)(DAF::NONE);
  reg = (int)(CDR::Intervals);
  count = 8;
  status = gDeviceTable->read(handle, flags, reg, count, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);

  tst->assertTrue("Read error.", (status >= 0));

  // Initiate continuous read

  flags = (int)(DAF::MILLI_RUN);
  reg = (int)(CDR::Stream);
  count = BUF_SIZE;
  status = gDeviceTable->read(handle, flags, reg, count, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  if (status > 0) {
    rpt->reportString(datablock);
  }

  tst->afterTest();


  // --------------------------------------------------------

  tst->afterGroup();

}


