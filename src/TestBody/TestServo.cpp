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

#define BUF_SIZE 32
  byte datablock[BUF_SIZE];

  // Begin testing

  tst->beforeGroup(groupID);

  // --------------------------------------------------------

  tst->beforeTest("Open");

  flags  = (int)(DAF::FORCE);
  openOpts = 0;
  strlcpy((char *)datablock, unitID, BUF_SIZE);

  status = gDeviceTable->open(openOpts, flags, (const char *)datablock);
  rpt->reportOpen(status, openOpts, flags, datablock);
  if (status >= 0) {
    rpt->reportString(datablock);
  }
  tst->assertTrue("Open error.", (status >= 0));

  handle = status;
  tst->afterTest();

  // =============================================================
  // Open.  Put device specific one-shot tests below this comment.
  // =============================================================

  tst->beforeTest("ReadWritePin");
  flags = 0;
  reg = (int)(DDServo::REG::PIN);
  count = 2;
  status = gDeviceTable->read(handle, flags, reg, count, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  tst->assertEquals("Pin read.", ENODATA, status);

  fromHostTo16LE((int)3, datablock);
  status = gDeviceTable->write(handle, flags, reg, count, datablock);
  rpt->reportWrite(status, handle, flags, reg, count);
  tst->assertEquals("Pin write:", 2, status);

  status = gDeviceTable->read(handle, flags, reg, count, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  tst->assertEquals("Pin read:", 2, status);

  int pin = ((datablock[1] & 0xFF) << 8) | (datablock[0] & 0xFF);
  tst->assertEquals("Pin number.", 3, pin);

  log->info("Pin number: ", pin);

  tst->afterTest();

//   // --------------------------------------------------------

  tst->beforeTest("ReadRegisters");

  status = gDeviceTable->read(handle, 0, (int)(DDServo::REG::POSITION_DEGREES), 2, datablock);
  tst->assertEquals("Pos_D read:", 2, status);
  int pos = from16LEToHost(datablock + 0);
  log->info("Position D: {}", pos);

  status = gDeviceTable->read(handle, 0, (int)(DDServo::REG::POSITION_MICROSECONDS), 2, datablock);
  tst->assertEquals("Pos_M read:", 2, status);
  pos = from16LEToHost(datablock + 0);
  log->info("Position M: {}", pos);

  status = gDeviceTable->read(handle, 0, (int)(DDServo::REG::RANGE_MICROSECONDS), 4, datablock);
  tst->assertEquals("Range read:", 4, status);
  int lo = from16LEToHost(datablock + 0);
  int hi = from16LEToHost(datablock + 2);
  log->info("Range lo, hi:", status, lo, hi);

  tst->afterTest();

// --------------------------------------------------------

  tst->beforeTest("ReadPulseRange");

  status = gDeviceTable->read(handle, 0, (int)(DDServo::REG::RANGE_MICROSECONDS), 4, datablock);
  if (status >= 0) {
    int lo = ((datablock[1]) << 8) | (datablock[0] << 0);
    int hi = ((datablock[3]) << 8) | (datablock[2] << 0);

  log->info("Range lo, hi:", status, lo, hi);
  }

  tst->assertEquals("Range read:", 4, status);

  tst->afterTest();

// --------------------------------------------------------

  tst->beforeTest("Sweep");

  status = gDeviceTable->read(handle, 0, (int)(DDServo::REG::PIN), 2, datablock);
  tst->assertEquals("Pin read:", 2, status);

  pin = ((datablock[1] & 0xFF) << 8) | (datablock[0] & 0xFF);
  tst->assertEquals("Pin number.", 3, pin);

  datablock[0] = 0;
  datablock[1] = 0;
  flags = 0;
  reg = (int)(DDServo::REG::POSITION_DEGREES);
  count = 2;
  status = gDeviceTable->write(handle, flags, reg, count, datablock);
  rpt->reportWrite(status, handle, flags, reg, count);
  tst->assertEquals("Position write:", 2, status);

  delay(1000);

  datablock[0] = (byte) 180;
  datablock[1] = 0;
  flags = 0;
  reg = (int)(DDServo::REG::POSITION_DEGREES);
  count = 2;
  status = gDeviceTable->write(handle, flags, reg, count, datablock);
  rpt->reportWrite(status, handle, flags, reg, count);
  tst->assertEquals("Position write:", 2, status);

  delay(1000);

  tst->afterTest();

// --------------------------------------------------------

  tst->beforeTest("Center");

  datablock[0] = 90;
  datablock[1] = 0;
  status = gDeviceTable->write(handle, 0, (int)(DDServo::REG::POSITION_DEGREES), 2, datablock);
  rpt->reportWrite(status, handle, flags, reg, count);

  delay(1000);

  tst->afterTest();

  // =============================================================
  // Close.  Put device specific one-shot tests above this comment.
  // =============================================================

  tst->beforeTest("Close");
  flags = (int)(DAF::NONE);
  status = gDeviceTable->close(handle, flags);
  rpt->reportClose(status, handle, flags);
  tst->assertTrue("Close.", (status >= 0));
  tst->afterTest();

  // =============================================================
  // Put device specific continuous tests between here and the end.
  // =============================================================

  tst->beforeTest("ContinuousSweep");

  // Open

  flags  = (int)(DAF::NONE);
  openOpts = 0;
  strlcpy((char *)datablock,unitID,BUF_SIZE);

  status = gDeviceTable->open(openOpts, flags, (const char *)datablock);
  rpt->reportOpen(status, openOpts, flags, datablock);
  tst->assertTrue("Open.", (status >= 0));

  handle = status;

  // Write Intervals

  flags = (int)(DAF::NONE);
  reg = (int)(CDR::Intervals);
  fromHostTo32LE(0UL,datablock);
  fromHostTo32LE(500UL,datablock+4);
  count = 8;
  status = gDeviceTable->write(handle, flags, reg, count, datablock);
  rpt->reportWrite(status, handle, flags, reg, count);
  tst->assertEquals("Position write:", 8, status);

  // Write pin

  fromHostTo16LE(3, datablock);
  status = gDeviceTable->write(handle, 0, (int)(DDServo::REG::PIN), 2, datablock);
  rpt->reportWrite(status, handle, flags, reg, count);
  tst->assertEquals("Pin write:", 2, status);

  status = gDeviceTable->read(handle, 0, (int)(DDServo::REG::PIN), 2, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  tst->assertEquals("Pin read:", 2, status);

  pin = ((datablock[1] & 0xFF) << 8) | (datablock[0] & 0xFF);
  tst->assertEquals("Pin number.", 3, pin);

  log->info("Pin number: ", pin);

  tst->afterTest();

  // Initiate continuous write

  flags = (int)(DAF::MILLI_RUN);
  reg = (int)(DDServo::REG::POSITION_MICROSECONDS);
  count = 2;
  fromHostTo16LE((int)(1500),datablock);
  status = gDeviceTable->write(handle, flags, reg, count, datablock);
  rpt->reportWrite(status, handle, flags, reg, count);

  tst->afterTest();

  // --------------------------------------------------------

  tst->afterGroup();

}
