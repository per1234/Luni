#include <LuniLib.h>
#include <DDHello/DDHello.h>
#include "TestHello.h"

TestHello::TestHello(const char * unitID) : DeviceDriverTest(__func__, unitID) {

}

void TestHello::doTest(TestManager *tst, ClientReporter *rpt) {

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

  // --------------------------------------------------------

  tst->beforeTest("ReadInterjection");

  flags = (int)(DAF::NONE);
  reg = (int)(DDHello::REG::INTERJECTION);
  status = globalDeviceTable->read(handle, flags, reg, BUF_SIZE, datablock);
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
  status = globalDeviceTable->read(handle, flags, reg, BUF_SIZE, datablock);
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
  status = globalDeviceTable->read(handle, flags, reg, BUF_SIZE, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  if (status > 0) {
    rpt->reportString(datablock);
  }

  tst->assertTrue("Read error.", (status >= 0));

  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("Close");

  flags = (int)(DAF::NONE);
  status = globalDeviceTable->close(handle, flags);
  rpt->reportClose(status, handle, flags);

  tst->assertTrue("Close error.", (status >= 0));

  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("ReadStreamContinuous");

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
  fromHostTo32LE(5000UL,datablock+4);
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

  // Initiate continuous read

  flags = (int)(DAF::MILLI_RUN);
  reg = (int)(CDR::Stream);
  count = BUF_SIZE;
  status = globalDeviceTable->read(handle, flags, reg, count, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  if (status > 0) {
    rpt->reportString(datablock);
  }

  tst->afterTest();


  // --------------------------------------------------------

  tst->afterGroup();

}


