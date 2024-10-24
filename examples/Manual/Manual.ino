#include <ENS22.h>

ENS22 modem(1);

void setup() {
  Serial.begin(9600);
  delay(1000);

  modem.debug();
  modem.begin(16, 17, 18, 19, 115200);  // RX=16, TX=17, RTS=18, CTS=19
}

void loop() {
  modem.manual();
}