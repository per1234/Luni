#include <LuniLib.h>
#include <Silicon/ByteOrder.h>
#include <stdio.h>

uint8_t one[] = { 0, 1, 2, 3};

#define ITEMS 5
uint16_t in16[ITEMS] = {0x0, 0xFF, 0x1, 0x1234, 0xABCD};
uint8_t out8BE[ITEMS][2] = {{0, 0}, {0x0, 0xFF}, {0, 1}, {0x12, 0x34}, {0xAB, 0xCD}};
uint8_t out8LE[ITEMS][2] = {{0, 0}, {0xFF, 0x0}, {1, 0}, {0x34, 0x12}, {0xCD, 0xAB}};

uint32_t in32[ITEMS] = {0x0, 0xFFAAEEDDL, 0x05060708L, 0xF0E0D0C0L, 0x80000001L};

void setup() {
  uint8_t buf[4];
  char stringBuffer[256];

  Serial.begin(115200);
  delay(3000);
  for (int idx = 5; idx > 0; idx--) {
    Serial.println(idx);
    delay(1000);
  }

  Serial.println("\nBegin 16-bit Byte Order testing.");

  for (int idx = 0; idx < ITEMS; idx++) {
    sprintf(stringBuffer, "\nSource item: 0x%04X", in16[idx]);
    Serial.println(stringBuffer);

    Serial.print(" toLE:  ");
    fromHostTo16LE(in16[idx], buf);
    if (out8LE[idx][0] == buf[0] && out8LE[idx][1] == buf[1]) {
      sprintf(stringBuffer, " PASS [%02X  %02X]", buf[0], buf[1]);
    } else {
      sprintf(stringBuffer, " FAIL [expected: %02X, actual: %02X, expected: %02X, actual %02X]", out8LE[idx][0], buf[0], out8LE[idx][1], buf[1]);
    }
    Serial.println(stringBuffer);

    Serial.print(" fromLE:");
    uint16_t result = from16LEToHost(buf);
    if (in16[idx] == result) {
      sprintf(stringBuffer, " PASS [0x%04X]", result);
    } else {
      sprintf(stringBuffer, " FAIL [expected: 0x%04X, actual: 0x%04x]", in16[idx], result);
    }
    Serial.println(stringBuffer);


    Serial.print(" toBE:  ");
    fromHostTo16BE(result, buf);
    if (out8BE[idx][0] == buf[0] && out8BE[idx][1] == buf[1]) {
      sprintf(stringBuffer, " PASS [%02X  %02X]", buf[0], buf[1]);
    } else {
      sprintf(stringBuffer, " FAIL [expected: %02X, actual: %02X, expected: %02X, actual %02X]", out8BE[idx][0], buf[0], out8BE[idx][1], buf[1]);
    }
    Serial.println(stringBuffer);

    Serial.print(" fromBE:");
    result = from16BEToHost(buf);
    if (in16[idx] == result) {
      sprintf(stringBuffer, " PASS [0x%04X]", result);
    } else {
      sprintf(stringBuffer, "FAIL [expected: 0x%04X, actual: 0x%04X]", in16[idx], result);
    }
    Serial.println(stringBuffer);
  }

  Serial.println("\nBegin 32-bit Byte Order testing.\n");

  for (int idx = 0; idx < ITEMS; idx++) {
    sprintf(stringBuffer, "Source item: 0x%08lX", in32[idx]);
    Serial.print(stringBuffer);

    fromHostTo32BE(in32[idx], buf);
    uint32_t result32 = from32BEToHost(buf);
    fromHostTo32LE(result32, buf);
    result32 = from32LEToHost(buf);
    if (in32[idx] == result32) {
      sprintf(stringBuffer, " PASS [0x%08lX]", result32);
    } else {
      sprintf(stringBuffer, " FAIL [expected: 0x%08lX, actual: 0x%08lX]", in32[idx], result32);
    }
    Serial.println(stringBuffer);
  }
}


void loop() {

}

