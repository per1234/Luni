#include <LuniLib.h>
#include "TestCommon.h"

TestCommon::TestCommon(const char * unitID) : DeviceDriverTest(__func__, unitID) {

}

void TestCommon::doTest(TestManager *tst, ClientReporter *rpt, Logger *log) {

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
  rpt->reportString(datablock);

  tst->assertTrue("Open error.", (status >= 0));

  handle = status;
  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("CDRDriverVersion");

  flags = (int)(DAF::NONE);
  reg = (int)(CDR::DriverVersion);
  status = gDeviceTable->read(handle, flags, reg, BUF_SIZE, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  if (status > 0) {
    rpt->reportString(datablock);
  }
  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("ReadUnitNamePrefix");

  flags = (int)(DAF::NONE);
  reg = (int)(CDR::UnitNamePrefix);
  status = gDeviceTable->read(handle, flags, reg, BUF_SIZE, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);
  if (status > 0) {
    rpt->reportString(datablock);
  }

  tst->assertTrue("Read error.", (status >= 0));

  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("ReadIntervals");

  flags = (int)(DAF::NONE);
  reg = (int)(CDR::Intervals);
  status = gDeviceTable->read(handle, flags, reg, BUF_SIZE, datablock);
  rpt->reportRead(status, handle, flags, reg, count, datablock);

  tst->assertTrue("Read error.", (status >= 0));

  tst->afterTest();

  // --------------------------------------------------------

  tst->beforeTest("Close");

  flags = (int)(DAF::NONE);
  status = gDeviceTable->close(handle, flags);
  rpt->reportClose(status, handle, flags);

  tst->assertTrue("Close error.", (status >= 0));

  tst->afterTest();


  // --------------------------------------------------------

  tst->afterGroup();

}


