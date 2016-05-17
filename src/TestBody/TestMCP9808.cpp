#include <LuniLib.h>
#include <DDMCP9808/DDMCP9808.h>
#include "TestMCP9808.h"

TestMCP9808::TestMCP9808(const char * unitID) : DeviceDriverTest(__func__, unitID) {

}

void TestMCP9808::doTest(TestManager *tst, ClientReporter *rpt, Logger *log) {

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

  tst->beforeTest("ReadRegisters");

    for (int r = 1; r < 9; r++) {
      flags = (int)(DAF::NONE);
      count = (r == 8) ? 1 : 2;

      status = gDeviceTable->read(handle, flags, r, count, datablock);
      rpt->reportRead(status, handle, flags, r, count, datablock);

      tst->assertEquals("Device status() count assertEquals:", count, status);

      switch (r) {
        case (int)(DDMCP9808::REG::MANUF_ID):
          log->info("Manufacturer ID: ", from16LEToHost(datablock));
          tst->assertEquals("MCP9808 MANUF_ID assertEquals", 0x54, from16LEToHost(datablock));
          break;
        case (int)(DDMCP9808::REG::DEVICE_ID):
          log->info("Device ID: ", from16LEToHost(datablock));
          tst->assertEquals("MCP9808 DEVICE_ID assertEquals", 0x400, from16LEToHost(datablock));
          break;
      }
    }
  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("ReadStreamOnce");

  flags = (int)(DAF::NONE);
  reg = (int)(CDR::Stream);
  status = gDeviceTable->read(handle, flags, reg, BUF_SIZE, datablock);
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
  reg = (int)(CDR::Stream);
  count = 8;
  status = gDeviceTable->read(handle, flags, reg, count, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);

  tst->afterTest();

  // --------------------------------------------------------

  tst->afterGroup();

}


