#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL webserver("MakerBeacon", "MakerNode");

void testHandler() {
  String webpage = R"=====(
    <h3>Test Route</h3>
  )=====";
  webserver.send(200, "text/html", webserver.createPage(webpage));
}

void setup() {
  webserver.bootWiFi();
  webserver.on("/test", testHandler);
  webserver.bootWebUI();
}

void loop() {
  webserver.handleClient();
}