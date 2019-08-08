#include <Arduino.h>
#include <Wire.h>
#include <CCMakersWSL.h>
#include <Temperature_LM75_Derived.h>



CCMakersWSL webserver("MakerBeacon", "MakerNode");
TI_TMP102_Compatible temperature(0x48);

void sensorHandler() {
  String webpage = "<h3>Sensors</h3>\n";  
  webpage += "Temperature: "+String(temperature.readTemperatureF())+"F<br>";
  if (WiFi.status() == WL_CONNECTED) {
    webpage += "WiFi Signal Strength: " + String(WiFi.RSSI()) + " dBm<br>";
  }
  webserver.send(200, "text/html", webserver.createPage(webpage));
}

void setup() {
  Wire.begin(D2, D1); //D2 is SDA, D1 is SCL

  webserver.bootWiFi();
  webserver.on("/sensors", sensorHandler);
  webserver.bootWebUI();
}

void loop() {
  webserver.handleClient();
}
