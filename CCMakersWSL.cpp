#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL* CCMakersWSL::singletonInstance = NULL;


#define WIFI_SSID_ADDR          0x008
#define WIFI_PASSWORD_ADDR      0x088

// EEPROM Manipulation functions
int ReadWiFiSSID(char* buffer);
int ReadWiFiPassword(char* buffer);
void WriteWifiCredentials(char* ssid, char* password);

// Page layout helpers
String _createPage(String body, String javascript);
String _createPage(String body);
String logoSVG();

// Route functions
void handleRoot();
void handleRestart();
void handleConfigure();
void handleWiFiStore();
void performRestart();
void handleNotFound();

ESP8266WebServer _server(80);

CCMakersWSL* CCMakersWSL::getInstance() {
   if (!singletonInstance)   // Only allow one instance of class to be generated.
      singletonInstance = new CCMakersWSL("MakerBeacon", "MakerNode");
   return singletonInstance;
}


CCMakersWSL::CCMakersWSL(String ssid, String hostname) {
  _ssid = ssid;
  _hostname = hostname;
}

void CCMakersWSL::on(const String &uri, ESP8266WebServer::THandlerFunction handler) {
  _server.on(uri, handler);
}

void CCMakersWSL::send(int code, char* content_type, const String& content) {
  _server.send(code, (const char*)content_type, content);
}

void CCMakersWSL::boot() {

  if (!Serial) {
    Serial.begin(9600);
  }

  Serial.println();

  char ssid[128], password[128];
  IPAddress local_IP(192, 168, 4, 1);
  IPAddress gateway(192, 168, 4, 1);
  IPAddress subnet(255, 255, 255, 0);

  // Initializing serial port for debugging purposes
  delay(5);

  EEPROM.begin(512);

  ReadWiFiSSID(ssid);
  ReadWiFiPassword(password);

  //Setup WiFi Access Point and Station
  // Connecting to WiFi network
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
    while (WiFi.status() != WL_CONNECTED && timeout < 30) {
      Serial.print(".");
      delay(2000);
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

  initRoutes();
}

void CCMakersWSL::begin() {
  _server.begin();
}

void CCMakersWSL::handleClient() {
  _server.handleClient();
}

void CCMakersWSL::initRoutes() {
  on("/", handleRoot);
  on("/restart", handleRestart);
  on("/configure", handleConfigure);
  on("/store", handleWiFiStore);
  on("/do-restart", performRestart);
  _server.onNotFound(handleNotFound);
}

String CCMakersWSL::createPage(String body, String javascript) {
  return _createPage(body, javascript);
}

String CCMakersWSL::createPage(String body) {
  return _createPage(body);
}

int ReadWiFiSSID(char* buffer) {
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

int ReadWiFiPassword(char* buffer) {
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

void WriteWifiCredentials(char* ssid, char* password) {
  int index, str_len;

  str_len = strlen(ssid);
  for (index = 0; index < str_len; ++index) {
    EEPROM.write(WIFI_SSID_ADDR + index, ssid[index]);
  }
  EEPROM.write(WIFI_SSID_ADDR + str_len, '\0');
  str_len = strlen(password);
  for (index = 0; index < str_len; ++index) {
    EEPROM.write(WIFI_PASSWORD_ADDR + index, password[index]);
  }
  EEPROM.write(WIFI_PASSWORD_ADDR + str_len, '\0');
  EEPROM.commit();
}


String _createPage(String body, String javascript) {
  String page = R"=====(
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
  )=====";

  page.replace("%CONNECTMSG%", WiFi.status() == WL_CONNECTED ? WiFi.localIP().toString() : "Not connected to a WiFi");

  page.replace("%LOGO%", logoSVG());
  page.replace("%BODY%", body);
  page.replace("%SCRIPTS%", javascript);
  return page;
}

String _createPage(String body) {
  return _createPage(body, "");
}

String logoSVG() {
  return R"(<svg id="CCMakersLogo" viewBox="0 0 346 45"><path id="Symbol" d="M18.7156,44.1903886 C8.38810239,44.1903886 0.0160114045,35.8182976 0.0160114045,25.4908 C0.0160114045,15.1633024 8.38810239,6.7912114 18.7156,6.7912114 C29.0430976,6.7912114 37.4151886,15.1633024 37.4151886,25.4908 C37.4151886,35.8182976 29.0430976,44.1903886 18.7156,44.1903886 Z M30.2,18.8 C29.6,17.8 28.3,17.6 27.3,18.2 L21.2,21.7 C20.2,22.3 18.8,22 18.2,21 L14.7,15 C14.3,14 13.1,13.5 12.1,13.9 C11.1,14.3 10.6,15.5 11,16.5 C11.1,16.7 11.2,16.8 11.3,17 L14.8,23.1 C15.4,24.1 15.1,25.5 14.1,26.1 L8,29.4 C7,29.8 6.5,31 6.9,32 C7.3,33 8.5,33.5 9.5,33.1 C9.7,33 9.9,33 10,32.8 L16.1,29.3 C17.1,28.7 18.5,29 19.1,30 L22.6,36 C23,37 24.2,37.5 25.2,37.1 C26.2,36.7 26.7,35.5 26.3,34.5 C26.2,34.3 26.1,34.2 26,34 L22.5,27.9 C21.9,26.9 22.2,25.5 23.2,24.9 L29.3,21.4 C30.3,21.1 30.7,19.8 30.2,18.8 Z"></path><path d="M75.6,28.7 L75.6,34.6 C75.6,42.4 70.9,42.7 65.9,42.7 L51.2,42.7 C46.2,42.7 42.5,41.4 42.5,34.6 L42.5,16.3 C42.5,8.5 47.6,8.2 53,8.2 L67.1,8.2 C71.7,8.2 75.5,9.4 75.5,16.3 L75.5,22.3 L62.8,22.3 L62.8,19.8 C62.8,18.5 62.7,17.1 61.6,16.2 C60.9,15.6 60,15.3 59.1,15.2 C58.5,15.2 55.4,15.2 55.4,19.8 L55.4,31.7 C55.4,34.3 56.7,36 59.1,36 C60.2,36 62.7,35.3 62.7,31.7 L62.7,28.8 L75.6,28.7 Z" id="C1" fill="#0093d0"></path><path d="M112.9,28.7 L112.9,34.6 C112.9,42.4 108.2,42.7 103.2,42.7 L88.6,42.7 C83.6,42.7 79.9,41.4 79.9,34.6 L79.9,16.3 C79.9,8.5 85,8.2 90.4,8.2 L104.5,8.2 C109.1,8.2 112.9,9.4 112.9,16.3 L112.9,22.3 L100.1,22.3 L100.1,19.8 C100.1,18.5 100,17.1 98.9,16.2 C98.2,15.6 97.3,15.3 96.4,15.2 C95.8,15.2 92.7,15.2 92.7,19.8 L92.7,31.7 C92.7,34.3 94,36 96.4,36 C97.5,36 100,35.3 100,31.7 L100,28.8 L112.9,28.7 Z" id="C2" fill="#0093d0"></path><path d="M132.6,13.7 L132.7,13.7 C134.2,8.7 136.4,8.2 143.6,8.2 C145.5,8.2 148.2,8.3 149.8,9.4 C151.1,10.2 151.9,11.5 152.2,12.9 C152.2,13.6 152.3,14.2 152.5,13.8 C153.8,9.9 155.5,8.1 161.1,8.1 L164.1,8.1 C169.1,8.1 172.7,9.5 172.7,16.1 L172.7,42.2 L160,42.2 L160,19.6 C160,18.7 159.9,15.1 156.2,15.1 C152.5,15.1 152.6,18.7 152.6,19.8 L152.6,42.4 L140,42.4 L140,19.7 C140,18.7 139.9,15.1 136.3,15.1 C132.7,15.1 132.6,18.8 132.6,19.6 L132.6,42.4 L119.9,42.4 L119.9,8.5 L132.6,8.5 L132.6,13.7 Z" id="M" fill="#000"></path><path d="M197.5,36.7 L197.4,36.7 C196.1,40.4 192.5,42.8 188.6,42.7 L184.8,42.7 C179.8,42.7 176.9,40.6 176.9,34.6 L176.9,30 C176.9,23.8 180.2,22 184.8,22 L197.3,22 L197.3,20 C197.3,19.3 197.3,15.1 193.7,15.1 C192.4,15.1 190,15.8 189.9,19.2 L177.1,19.2 L177.1,16.3 C177.1,8.6 182.4,8.3 186.2,8.3 L201.5,8.3 C206.5,8.3 210,9.8 210,16.6 L210,42.5 L197.4,42.5 L197.4,36.7 L197.5,36.7 Z M197.5,27.9 L194.2,27.9 C192.4,27.9 189.5,28.6 189.5,32.1 C189.5,34.2 191.1,35.9 193.2,35.9 C193.2,35.9 193.3,35.9 193.3,35.9 C194.1,35.9 197.4,35.5 197.4,31 L197.5,27.9 Z" id="A" fill="#000"></path><polygon id="K" fill="#000" points="214.8 0 227.7 0 227.7 21.7 227.8 21.7 235 8.6 248 8.6 239.5 23.6 250.4 42.5 236.4 42.5 227.8 27.4 227.7 27.4 227.7 42.4 214.9 42.4 214.9 0"></polygon><path d="M284,32.2 L284,34.7 C284,40.9 280.7,42.7 275.6,42.7 L259.3,42.7 C254.7,42.7 250.9,41.5 250.9,34.6 L250.9,16.3 C250.9,10 254.2,8.3 259.9,8.3 L275.4,8.3 C280.5,8.3 283.9,9.9 283.9,16.3 L283.9,28.9 L263.6,28.9 L263.6,31.3 C263.6,33.1 263.9,35.9 267.3,35.9 C269.3,36 270.9,34.4 271,32.4 C271,32.3 271,32.3 271,32.2 L284,32.2 Z M271.2,22.1 L271.2,19.8 C271.2,16.4 269.6,15.1 267.4,15.1 C265.2,15.1 263.8,16.7 263.8,19.2 L263.8,22 L271.2,22.1 Z" id="E" fill="#000"></path><path d="M301,13.9 L301.1,13.9 C301.7,12.1 303.4,7.6 309.2,8.2 L309.2,20.8 L308.4,20.8 C301.7,20.8 301,26.2 301,28.9 L301,42.4 L288.1,42.4 L288.1,8.5 L301,8.5 L301,13.9 Z" id="R" fill="#000"></path><path d="M332.4,29.8 L321.7,29.5 C316.6,29.4 312,29.2 312,20.9 L312,16.3 C312,10.1 315.1,8.2 319.9,8.2 L337,8.2 C341.9,8.2 344.9,10.2 344.9,16.3 L344.9,18.9 L332.1,18.9 C332.1,17.7 331.6,16.5 330.6,15.8 C330,15.4 329.2,15.1 328.5,15.1 C328.2,15.1 324.7,15.1 324.7,19.3 L324.7,21.9 L335.7,22.1 C340.1,22.2 345.2,22.4 345.2,30.1 L345.2,34.6 C345.2,40.6 341.9,42.8 336.5,42.8 L320.7,42.8 C316.4,42.8 311.7,42.3 311.7,34.6 L311.7,32.5 L324.4,32.5 C324.4,33.5 324.9,34.4 325.7,35 C326.4,35.5 327.3,35.8 328.2,35.8 C332.3,35.8 332.3,31.9 332.3,30.9 L332.4,29.8 Z" id="S" fill="#000"></path></svg>)";
}

// Built-in routes
void handleRoot() {
  String hostname = CCMakersWSL::getInstance()->_hostname;
  String webpage = "<h3>" + hostname + "</h3>";
  _server.send(200, "text/html", _createPage(webpage)); //Send web page
}

void handleRestart() {
  String webpage = "";
  webpage += "<h3>Restart Node</h3>";;
  webpage += "<p>Restarting the node will take a few minutes.</p>";
  webpage += "<button id='reboot-btn'>Restart</button>";

  String js = R"(
    var rebootBtn = document.getElementById('reboot-btn');
    rebootBtn.addEventListener('click', function() {
      rebootBtn.innerHTML = "Restarting";
      rebootBtn.disabled = true;
      fetch('/do-restart')
      .then(function(response) {
        return response.json();
      })
      .then(function(myJson) {
        console.log(JSON.stringify(myJson));
      });
    });
  )";
  _server.send(200, "text/html", _createPage(webpage, js)); //Send web page
}

void handleConfigure() {
  char ssid[128], password[128];

  ReadWiFiSSID(ssid);
  ReadWiFiPassword(password);

  String webpage = "<h3>Enter Local Wifi Credentials:</h3>";
  webpage += "<form action=\"/store\" method=\"GET\">";
  webpage += "SSID: <input type=\"text\" name=\"ssid\" value=\"";
  webpage += String(ssid) + "\"><br><br>";
  webpage += "Password: <input type=\"text\" name=\"password\" value=\"";
  webpage += String(password) + "\"><br><br>";
  webpage += "<input type=\"submit\" value=\"Configure\">";
  webpage += "</form>";
  _server.send(200, "text/html", _createPage(webpage)); //Send web page
}

void handleWiFiStore() {
  String webpage = "";
  if (_server.args() >= 2) {
    String ssid = _server.arg("ssid");
    String password = _server.arg("password");
    WriteWifiCredentials((char*)ssid.c_str(), (char*)password.c_str());
    webpage += "<h3>WiFi Credentials Updated. Node Restarting...</h3>";
    webpage += "<br><a href=\"/\">Node Main</a><br>";
  } else {
    webpage += "<h3>Failed to update WiFi. Form error.</h3>";
    webpage += "<br><a href=\"/\">Node Main</a><br>";
  }
  _server.send(200, "text/html", _createPage(webpage)); //Send web page
  delay(100);
  ESP.restart();
}

void performRestart() {
  _server.send(200, "text/json", "{status: \"ok\"}"); //Send web page
  ESP.restart();
}

void handleNotFound() {
  String webpage = "<h3>Page Not Found</h3>";
  _server.send(404, "text/html", _createPage(webpage));
}