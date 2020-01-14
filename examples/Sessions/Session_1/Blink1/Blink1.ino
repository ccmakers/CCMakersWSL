#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL webserver("MakerBeacon", "MakerNode");

void blinkHandler() {
  String webpage = R"=====(
    <h3>Blink Example</h3>
    <a href="/do-blink">blink</a>
  )=====";

  webserver.send(200, "text/html", webserver.createPage(webpage));
}

void doBlink() {
  webserver.sendHeader("Location", "/blink", true);   //Redirect to our html web page
  webserver.send(302, "text/plane","");
  digitalWrite(2, 0);
  delay(1000);
  digitalWrite(2, 1);
}

void setup() {

  digitalWrite(2, 1);
  pinMode(2, OUTPUT);

  webserver.bootWiFi();
  webserver.on("/blink", blinkHandler);
  webserver.on("/do-blink", doBlink);
  webserver.bootWebUI();
}

void loop() {
  webserver.handleClient();
}