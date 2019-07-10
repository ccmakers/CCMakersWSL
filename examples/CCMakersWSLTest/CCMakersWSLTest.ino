#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL webserver("MakerBeacon", "MakerNode");

void setup() {
}

void loop() {
  webserver.start();
}
