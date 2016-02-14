#include "SelectedDeviceDrivers.h"
#include <Device/DeviceTable.h>

/**
   Test the operation of several device drivers loaded together.
*/

#define BUF_SIZE 256
byte buf[BUF_SIZE];
char names[BUF_SIZE];
int handle;
int vReg[] = {static_cast<int>(CDR::DriverVersion), static_cast<int>(CDR::LibraryVersion)};

DeviceTable dt(selectedDevices);

void setup() {

  char *unitNames[] = {"HW:0", "PK:0", "TC:0" };
  int unitNameCount = 3;
  int bufIndex = 0;

  Serial.begin(115200);
  delay(3000);
  for (int idx = 5; idx > 0; idx--) {
    Serial.println(idx);
    delay(1000);
  }

  for (int idx = 0; idx < unitNameCount; idx++) {
    Serial.print("Open: ");
    Serial.print(unitNames[idx]);
    Serial.print(" ");
    int status = dt.open(unitNames[idx], 0);
    Serial.println(status);

    //  Serial.print("Config: ");
    //  handle = status;
    //  int bufIndex = 0;
    //  buf[bufIndex++] = 2;  // 4-wire interface
    //  buf[bufIndex++] = getLSBInt16(StepsPerRevolution);
    //  buf[bufIndex++] = getMSBInt16(StepsPerRevolution);
    //  for (int idx = 0; idx < 4; idx++) {
    //    buf[bufIndex++] = getLSBInt16(pin[idx]);
    //    buf[bufIndex++] = getMSBInt16(pin[idx]);
    //  }
    //  status = dd->control(handle, CDR_Configure, bufIndex, buf);
    //  Serial.println(status);
    //
    //  Serial.print("Speed: ");
    //  bufIndex = 0;
    //  buf[bufIndex++] = getLSBInt16(RevolutionsPerMinute);
    //  buf[bufIndex++] = getMSBInt16(RevolutionsPerMinute);
    //  buf[bufIndex++] = 0;
    //  buf[bufIndex++] = 0;
    //  status = dd->control(handle, STP_RPMSpeed, bufIndex, buf);
    //  Serial.println(status);

    if (status < 0) {
      continue;
    } else {
      handle = status;
    }
    for (int regIndex = 0; regIndex < 2; regIndex++) {
      Serial.print("Version: ");
      status = dt.status(handle, vReg[regIndex], BUF_SIZE, buf);
      Serial.print(status);
      if (status >= 0) {
        bufIndex = 0;
        int packetSize = buf[bufIndex++];
        for (int idx = 0; idx < packetSize; idx++) {
          switch (idx) {
            case 0:
              Serial.print(' ');
              break;
            case 1:
              Serial.print('.');
              break;
            case 2:
              Serial.print('.');
              break;
            case 3:
              Serial.print('-');
              break;
            case 4:
              Serial.print('.');
              break;
            case 5:
              Serial.print('.');
              break;
            default:
              Serial.print('?');
          }
          Serial.print(buf[bufIndex++]);
        }
        Serial.print(' ');
        Serial.write(reinterpret_cast<char *>(&buf[bufIndex]));
      }
      Serial.println();
    }
  }
}

void loop() {
  dt.dispatchTimers();
  //
  //  Serial.print("Move+: ");
  //  int msgIndex = 0;
  //  buf[msgIndex++] = getLSBInt16(15);
  //  buf[msgIndex++] = getMSBInt16(15);
  //  buf[msgIndex++] = 0;
  //  buf[msgIndex++] = 0;
  //  buf[msgIndex++] = 0;
  //  int status = dd->control(handle, STP_MoveR, msgIndex, buf);
  //  Serial.println(status);
  //
  //  //delay(1000);
  //
  //  Serial.print("Move-: ");
  //  msgIndex = 0;
  //  buf[msgIndex++] = getLSBInt16(-15);
  //  buf[msgIndex++] = getMSBInt16(-15);
  //  buf[msgIndex++] = -1;
  //  buf[msgIndex++] = -1;
  //  buf[msgIndex++] = 0;
  //  status = dd->control(handle, STP_MoveR, msgIndex, buf);
  //  Serial.println(status);
  //
  //  //delay(1000);

}
