#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL webserver("MakerBeacon", "MakerNode");

void sensorHandler() {
  String webpage = "<h3>Sensors</h3>\n";
  if (digitalRead(D5) == 1) webpage += "Door Alarm: OPEN<br>";
  else webpage += "Door Alarm: CLOSED<br>";
  webserver.send(200, "text/html", webserver.createPage(webpage));
}

void setup() {
  pinMode(D5, INPUT);     // Initialize the door alarm pin as an input

  webserver.bootWiFi();
  webserver.on("/sensors", sensorHandler);
  webserver.bootWebUI();
}

void loop() {
  webserver.handleClient();
}
