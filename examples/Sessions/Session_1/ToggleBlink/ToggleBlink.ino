#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL webserver("MakerBeacon", "MakerNode");

bool ledState = false;

void blinkHandler() {
  String webpage = R"=====(
    <h3>Toggle Blink</h3>
    <button id="toggleButton">%STATE%</button>
  )=====";

  webpage.replace("%STATE%", ledState ? "on" : "off");

  String js = R"=====(
    var button = document.getElementById('toggleButton');
    button.addEventListener('click', function() {
      fetch('/toggle-blink')
      .then(function(response) {
        return response.json();
      })
      .then(function(jsonResponse) {
        button.innerText = jsonResponse.state;
      });
    });
  )=====";
  webserver.send(200, "text/html", webserver.createPage(webpage, js));
}

void toggleBlink() {

  ledState = !ledState;

  if (ledState)
  {
    webserver.send(200, "text/plain", "{\"state\": \"on\"}");
    digitalWrite(2, 0);
  }
  else
  {
    webserver.send(200, "text/plain", "{\"state\": \"off\"}");
    digitalWrite(2, 1);
  }
}

void setup() {

  digitalWrite(2, 1);
  pinMode(2, OUTPUT);

  webserver.bootWiFi();
  webserver.on("/blink", blinkHandler);
  webserver.on("/toggle-blink", toggleBlink);
  webserver.bootWebUI();
}

void loop() {
  webserver.handleClient();
}