#include <Arduino.h>
#include <Wire.h>
#include <CCMakersWSL.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>

#define IFTTT_API_KEY "you_key_goes_here"


CCMakersWSL webserver("MakerBeacon", "MakerNode");

void triggerHandler() {
  String event_name = "example_alert";
  TriggerIFTTTEvent(IFTTT_API_KEY, event_name);
  String webpage = "<h3>IFTTT Triggered</h3>\n";
  webserver.send(200, "text/html", webserver.createPage(webpage));
}

void setup() {
  Wire.begin(D2, D1); //D2 is SDA, D1 is SCL

  webserver.bootWiFi();
  webserver.on("/trigger", triggerHandler);
  webserver.bootWebUI();
}

void loop() {
  webserver.handleClient();
}



void TriggerIFTTTEvent(String api_key, String event_name){
  WiFiClient client;
  HTTPClient http;
  
  http.begin(client, "http://maker.ifttt.com/trigger/" + event_name + "/with/key/" + api_key);
  int http_code = http.GET();
  http.end();
}
