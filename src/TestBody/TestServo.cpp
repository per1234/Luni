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

  status = globalDeviceTable->open(openOpts, flags, (const char *)datablock);
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
  status = globalDeviceTable->read(handle, 0, (int)(DDServo::REG::PIN), 2, datablock);
  tst->assertEquals("Pin read unexpected status.", ENODATA, status);

  fromHostTo16LE(3, datablock);
  status = globalDeviceTable->write(handle, 0, (int)(DDServo::REG::PIN), 2, datablock);
  tst->assertEquals("Pin write error:", 2, status);

  status = globalDeviceTable->read(handle, 0, (int)(DDServo::REG::PIN), 2, datablock);
  tst->assertEquals("Pin read error:", 2, status);

  int pin = ((datablock[1] & 0xFF) << 8) | (datablock[0] & 0xFF);
  tst->assertEquals("Pin read unexpected pin number.", 3, pin);

  log->info("Pin number: ", pin);

  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("ReadRegisters");

  status = globalDeviceTable->read(handle, 0, (int)(DDServo::REG::POSITION_DEGREES), 2, datablock);
  tst->assertEquals("Pos_D read error:", 2, status);
  int pos = from16LEToHost(datablock + 0);
  log->info("Position in degrees: {}", pos);

  status = globalDeviceTable->read(handle, 0, (int)(DDServo::REG::POSITION_MICROSECONDS), 2, datablock);
  tst->assertEquals("Pos_M read error:", 2, status);
  pos = from16LEToHost(datablock + 0);
  log->info("Position in microseconds: {}", pos);

  status = globalDeviceTable->read(handle, 0, (int)(DDServo::REG::RANGE_MICROSECONDS), 4, datablock);
  tst->assertEquals("Range read error:", 4, status);
  int lo = from16LEToHost(datablock + 0);
  int hi = from16LEToHost(datablock + 2);
  log->info("Read {} range bytes. lo: {}, hi: {}", status, lo, hi);

  tst->afterTest();

// --------------------------------------------------------

  tst->beforeTest("ReadPulseRange");

  status = globalDeviceTable->read(handle, 0, (int)(DDServo::REG::RANGE_MICROSECONDS), 4, datablock);
  if (status >= 0) {
    int lo = ((datablock[1]) << 8) | (datablock[0] << 0);
    int hi = ((datablock[3]) << 8) | (datablock[2] << 0);

    log->info("Read {} pulse range bytes. lo: {}, hi: {}", status, lo, hi);
  }

  tst->assertEquals("Range read error:", 4, status);

  tst->afterTest();

// --------------------------------------------------------

  tst->beforeTest("Sweep");

  status = globalDeviceTable->read(handle, 0, (int)(DDServo::REG::PIN), 2, datablock);
  tst->assertEquals("Pin read error:", 2, status);

  pin = ((datablock[1] & 0xFF) << 8) | (datablock[0] & 0xFF);
  tst->assertEquals("Pin read unexpected pin number.", 3, pin);

  datablock[0] = 0;
  datablock[1] = 0;
  status = globalDeviceTable->write(handle, 0, (int)(DDServo::REG::POSITION_DEGREES), 2, datablock);
  tst->assertEquals("Position write error:", 2, status);

  delay(1000);

  datablock[0] = (byte) 180;
  datablock[1] = 0;
  status = globalDeviceTable->write(handle, 0, (int)(DDServo::REG::POSITION_DEGREES), 2, datablock);
  tst->assertEquals("Position write error:", 2, status);

  delay(1000);

  tst->afterTest();

// --------------------------------------------------------

  tst->beforeTest("Center");

  status = globalDeviceTable->read(handle, 0, (int)(DDServo::REG::PIN), 2, datablock);
  tst->assertEquals("Pin read error:", 2, status);

  pin = ((datablock[1] & 0xFF) << 8) | (datablock[0] & 0xFF);
  tst->assertEquals("Pin read unexpected pin number.", 3, pin);

  datablock[0] = 90;
  datablock[1] = 0;
  status = globalDeviceTable->write(handle, 0, (int)(DDServo::REG::POSITION_DEGREES), 2, datablock);

  delay(1000);

  tst->afterTest();

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

  tst->beforeTest("ContinuousSweep");

  // Open

  flags  = (int)(DAF::NONE);
  openOpts = 0;
  strlcpy((char *)datablock,unitID,BUF_SIZE);

  status = globalDeviceTable->open(openOpts, flags, (const char *)datablock);
  rpt->reportOpen(status, openOpts, flags, datablock);
  handle = status;

  // Write Intervals

  flags = (int)(DAF::NONE);
  reg = (int)(CDR::Intervals);
  fromHostTo32LE(0UL,datablock);
  fromHostTo32LE(500UL,datablock+4);
  count = 8;
  status = globalDeviceTable->write(handle, flags, reg, count, datablock);
  rpt->reportWrite(status, handle, flags, reg, count);

  // Read Intervals

  flags = (int)(DAF::NONE);
  reg = (int)(CDR::Intervals);
  count = 8;
  status = globalDeviceTable->read(handle, flags, reg, count, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);

  tst->assertTrue("Read error.", (status >= 0));

  // Initiate continuous write

  flags = (int)(DAF::MILLI_RUN);
  reg = (int)(DDServo::REG::POSITION_MICROSECONDS);
  count = BUF_SIZE;
  fromHostTo16LE((int)(1500),datablock);
  status = globalDeviceTable->write(handle, flags, reg, count, datablock);
  rpt->reportWrite(status, handle, flags, reg, count);

  tst->afterTest();

  // --------------------------------------------------------

  tst->afterGroup();

}
