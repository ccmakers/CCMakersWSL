#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL webserver("MakerBeacon", "MakerNode");

bool redLedState = false;
bool greenLedState = false;
bool blueLedState = false;

void pageHandler() {
  String webpage = R"=====(
    <h3>RGB</h3>
    <button id="redButton" style="background-color: red;">%RED%</button>
    <button id="greenButton" style="background-color: green;">%GREEN%</button>
    <button id="blueButton" style="background-color: blue;">%BLUE%</button>
  )=====";

  webpage.replace("%RED%", redLedState ? "on" : "off");
  webpage.replace("%GREEN%", greenLedState ? "on" : "off");
  webpage.replace("%BLUE%", blueLedState ? "on" : "off");

  String js = R"=====(

    var redButton = document.getElementById('redButton');
    redButton.addEventListener('click', function() {
      fetch('/toggle-red')
      .then(function(response) {
        return response.json();
      })
      .then(function(jsonResponse) {
        redButton.innerText = jsonResponse.state;
      });
    });

  )=====";
  webserver.send(200, "text/html", webserver.createPage(webpage, js));
}

void toggleRed() {

  redLedState = !redLedState;

  if (redLedState)
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
  webserver.on("/rgb", pageHandler);
  webserver.on("/toggle-red", toggleRed);
  webserver.bootWebUI();
}

void loop() {
  webserver.handleClient();
}