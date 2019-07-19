# CCMakersWSL

The CCMakersWSL class is a wrapper for [esp8266](https://github.com/esp8266/) WiFi and Web Server functions. It's meant to be used for teaching purpouses only.

## Installation

## Usage

### Class Constructor

```cpp
CCMakersWSL webserver("ssid", "hostname");
```

Creates the CCMakersWSL class object.

*Parameters:*

WiFi Name: `String ssid`
Hostname: `String hostname`

### Simple start

#### Starting the server

```cpp
#include <CCMakersWSL.h>

// Declare in global scope
CCMakersWSL webserver("MakerBeacon", "MakerNode");

// define route handler function
void testHandler() {
  webserver.send(200, "text/plain", "test route");
}

void setup() {
  // Boot up WiFi
  webserver.bootWiFi();

  // Register your custom routes
  webserver.on("/test", testHandler);

  // Boot up Web User Interface
  webserver.bootWebUI();
}

void loop() {
  // Handling incoming client requests
  webserver.handleClient();
}
```