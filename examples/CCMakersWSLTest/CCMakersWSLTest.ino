#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL webserver(2);

void setup() {
}

void loop() {
  webserver.ping();
}
