#include <FirmataDeviceLibrary.h>

/**
   Test the operation of the analog read
*/
  int loopCount = 0;

#define leftPin A6
#define rightPin A7

void setup() {
  Serial.begin(115200);
  delay(3000);
  for (int idx = 5; idx > 0; idx--) {
    Serial.println(idx);
    delay(1000);
  }
  Serial.println("\n\nsequence,left,right");
}

void loop() {

  int valLeft = analogRead(leftPin);    // read the input pin
  delay(500);
  int valRight = analogRead(rightPin);    // read the input pin

  Serial.print(loopCount);
  Serial.print(",");
  Serial.print(valLeft);  
  Serial.print(",");
  Serial.println(valRight);
       
  delay(500);

  loopCount += 1;
}
