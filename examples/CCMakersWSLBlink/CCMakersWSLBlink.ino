#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL webserver("MakerBeacon", "MakerNode");

void blinkHandler() {
  String webpage = R"=====(
    <h3>Blink Test</h3>
    <button id="blinkButton">blink</button>
  )=====";

  String js = R"=====(
    var element = document.getElementById('blinkButton');
    element.addEventListener('click', function() {
      fetch('/do-blink')
      .then(function(response) {
        return response.json();
      })
      .then(function(myJson) {
        console.log(JSON.stringify(myJson));
      });
    });
  )=====";
  webserver.send(200, "text/html", webserver.createPage(webpage, js));
}

void doBlink() {
  webserver.send(200, "text/plain", "ok");
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