#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL webserver("MakerBeacon", "MakerNode");

void testHandler() {
  String webpage = "<h3>testing</h3>";
  webserver.send(200, "text/html", webserver.createPage(webpage)); //Send web page
}

void setup() {
  webserver.on("/test", testHandler);
  webserver.begin();
}

void loop() {
  webserver.handleClient();
}