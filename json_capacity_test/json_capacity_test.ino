#include <ArduinoJson.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println();
  int capacity = JSON_OBJECT_SIZE(4);
  Serial.println(capacity);
}

void loop() {
  // put your main code here, to run repeatedly:

}
