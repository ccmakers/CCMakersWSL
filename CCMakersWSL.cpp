#include <Arduino.h>
#include <CCMakersWSL.h>

#define WIFI_SSID_ADDR          0x008
#define WIFI_PASSWORD_ADDR      0x088

ESP8266WebServer _server(80);

int ledPin = 2;

CCMakersWSL::CCMakersWSL(String ssid, String hostname) {
  _ssid = ssid;
  _hostname = hostname;

  boot();
}

void CCMakersWSL::boot() {
  char ssid[128], password[128];
  IPAddress local_IP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);

  digitalWrite(ledPin, 1);
  pinMode(ledPin, OUTPUT);

  // Initializing serial port for debugging purposes
  Serial.begin(9600);
  delay(5);

  EEPROM.begin(512);

  ReadWiFiSSID(ssid);
  ReadWiFiPassword(password);

  //Setup WiFi Access Point and Station
  // Connecting to WiFi network
  Serial.println();
  Serial.println("Starting WiFi service.");
  delay(5);

  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname(_hostname);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(_ssid);
  delay(25);

  if (strlen(ssid) > 0 && strlen(password) > 0) {
    int timeout = 0;
    Serial.println("Attempting to connect to local WiFi.");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && timeout < 50) { //50 loops at 100ms per loop is 5 secs
      delay(100);
      ++timeout;
    }
  }

  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi. SSID: |" + String(ssid) + "| Password: |" + String(password) + "|");
    Serial.println("Local IP: " + WiFi.localIP().toString());
  } else {
    Serial.println("Could not connect to WiFi. SSID: |" + String(ssid) + "| Password: |" + String(password) + "|");
  }
  Serial.println("Connect to http://" + WiFi.softAPIP().toString() + "/ to configure.");

  _server.on("/", [&]() {
    _server.send(200, "text/html", "home page"); //Send web page
  });
  _server.onNotFound([&]() {
    _server.send(404, "text/plain", "Page Not Found");
  });
  _server.begin();

  digitalWrite(ledPin, 0);//signal setup is complete
}

void CCMakersWSL::start() {
  _server.handleClient();
}

int CCMakersWSL::ReadWiFiSSID(char* buffer) {
  int index = 0, done = 0;
  unsigned char data;

  while (!done && index < 127) {
    data = EEPROM.read(WIFI_SSID_ADDR + index);
    if (data >= 32 && data <= 126) {
      buffer[index] = data;
    } else {
      buffer[index] = '\0';
      done = 1;
    }
    ++index;
  }

  return index - 1;
}

int CCMakersWSL::ReadWiFiPassword(char* buffer) {
  int index = 0, done = 0;
  unsigned char data;

  while (!done && index < 127) {
    data = EEPROM.read(WIFI_PASSWORD_ADDR + index);
    if (data >= 32 && data <= 126) {
      buffer[index] = data;
    } else {
      buffer[index] = '\0';
      done = 1;
    }
    ++index;
  }

  return index - 1;
}