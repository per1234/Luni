#include <LuniLib.h>
#include <DDMeta/DDMeta.h>
#include "TestMeta.h"

TestMeta::TestMeta(const char * unitID) : DeviceDriverTest(__func__, unitID) {

}

void TestMeta::doTest(TestManager *tst, ClientReporter *rpt, Logger *log) {

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

  tst->beforeTest("ReadAvgIntervals");

  flags = (int)(DAF::NONE);
  reg = (int)(DDMeta::REG::AVG_INTERVALS);
  count = 8;
  status = gDeviceTable->read(handle, flags, reg, count, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);

  tst->assertTrue("Read error.", (status >= 0));

  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("ReadDriverCount");

  flags = (int)(DAF::NONE);
  reg = (int)(DDMeta::REG::DRIVER_COUNT);
  count = 2;
  status = gDeviceTable->read(handle, flags, reg, count, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);

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
  fromHostTo32LE(512UL,datablock);
  fromHostTo32LE(4096UL,datablock+4);
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
  reg = (int)(DDMeta::REG::AVG_INTERVALS);
  count = 8;
  status = gDeviceTable->read(handle, flags, reg, count, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);

  tst->afterTest();

  // --------------------------------------------------------

  tst->afterGroup();

}


