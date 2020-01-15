# CCMakersWSL

The CCMakersWSL class is a wrapper for [esp8266](https://github.com/esp8266/) WiFi and Web Server functions. It's meant to be used for teaching purpouses only.

## Installation

Download the latest version of the library [here](https://github.com/ccmakers/CCMakersWSL/releases/latest). In the Arduino IDE, navigate to *Sketch > Include Library > Add .ZIP Library*.

You will be prompted to select the library you would like to add. Navigate to the .zip file's location and open it.

Return to the Sketch > Include Library menu. menu. You should now see the library at the bottom of the drop-down menu. It is ready to be used in your sketch. The zip file will have been expanded in the libraries folder in your Arduino sketches directory.

Restart the Arduino IDE and happy coding!

## Usage

### Class Constructor

Declare the webserver in global scope

```cpp
CCMakersWSL webserver("ssid", "hostname");
```

Creates the CCMakersWSL class object.

*Parameters:*

WiFi Name: `String ssid`
Hostname: `String hostname`

### Basic Usage

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

### API

#### void bootWiFi()
Boot up the WiFi. Has to be called first, inside `setup()`.
```cpp
webserver.bootWiFi();
```

#### void bootWebUI();
Boot up the web interface. Has to be called inside `setup()`, after booting the Wifi.
```cpp
webserver.bootWebUI();
```

#### void on(String route, Function handler);
Add custom routes to the web interface. Has to be called inside `setup()`, after booting the Wifi, but before booting the web interface.
```cpp
webserver.on("/example", handleFunction);
```

#### void handleClient();
Handling incoming client requests. Has to be called inside `loop()`.
```cpp
webserver.handleClient();
```

#### void send(int code, char* content_type, String content);
Sending responses to the client. Typically used inside route's function handler.
```cpp
webserver.send(200, "text/text", "Hello World");
```

#### void sendHeader(const String& name, const String& value, bool first);
Perform server side redirect
```cpp
  webserver.sendHeader("Location", "/your-redirected-route", true);
  webserver.send(302, "text/plain","");
```

#### bool hasArg(const String& name);
Check if query string argument is present
```cpp
  if (webserver.hasArg("msg"))
  {
    // has "msg" argument
    ...
  }
```

#### String arg(const String& name);
Retrieve query string argument
```cpp
  if (webserver.hasArg("msg"))
  {
    String msg = webserver.arg("msg");
    ...
  }
```

#### String createPage(String body, [Optional: String javascript]);
Help format HTML interfaces. Typically used in conjunction with `send()` function.
```cpp
String webpage = R"=====(
  <h3>Example</h3>
)=====";
webserver.send(200, "text/html", webserver.createPage(webpage));
```

Or if you want to write some javacript
```cpp
String webpage = R"=====(
  <h3>Javascript Example</h3>
)=====";

String js = R"=====(
  console.log('hi!');
)=====";
webserver.send(200, "text/html", webserver.createPage(webpage, js));
```

The markup is as follows:

```html
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>CCMakersWSL</title>
    <link rel="icon" href="/favicon.png">
    <style>
      body {
        font-family: sans-serif;
      }
      .container {
        max-width: 800px;
        margin: 1rem auto;
      }

      header svg {
        width: 50%;
        margin: 1rem 0;
      }

      header nav {
        background-color: #2780e3;
      }

      header nav ul {
        display: flex;
        flex-wrap: nowrap;
        margin: 0;
        padding: 0;
        overflow-x: auto;
        color: rgba(255, 255, 255, 0.75);
        white-space: nowrap;
        list-style: none;
      }
      header nav ul li {
        padding-top: 0.75rem;
        padding-bottom: 0.75rem;
        padding-right: 0.5rem;
        padding-left: 0.5rem;
        transition: color 0.4s ease 0s, background-color 0.4s ease 0s;
      }

      header nav ul li:last-child {
        margin-left: auto;
        color: yellow;
      }

      header nav ul li a {
        color: inherit;
        text-decoration: none;
      }

      header nav ul li a:hover {
        text-decoration: underline;
      }

      .well {
        min-height: 20px;
        padding: 19px;
        margin-bottom: 20px;
        background-color: #f5f5f5;
        border: 1px solid #e3e3e3;
        border-radius: 4px;
        -webkit-box-shadow: inset 0 1px 1px rgba(0,0,0,.05);
        box-shadow: inset 0 1px 1px rgba(0,0,0,.05);
      }
    </style>
  </head>
  <body>
    <header class="container">
      %LOGO%
      <nav>
        <ul>
          <li><a href="/">Home</a></li>
          <li><a href="/configure">Configure WiFi</a></li>
          <li><a href="/restart">Restart Node</a></li>
          <li>%CONNECTMSG%</li>
        </ul>
      </nav>
    </header>
    <div class="container">
      <div class="well">
      %BODY%
      </div>
    </div>
    <footer class="container"><a href="https://capecodmakers.org/"  target="_blank">Cape Code Makers</a> | <a href="https://github.com/ccmakers/CCMakersWSL" target="_blank">Github</a></footer>
    <script>%SCRIPTS%</script>
  </body>
</html>
```
