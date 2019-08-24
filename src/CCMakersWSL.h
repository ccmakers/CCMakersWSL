/*
  CCMakersWSL.h - Web server library used in the IoT arduino class.
  Created by Jesse Craig and Rodrigo Passos, May 22, 2019.
  Released into the public domain.
*/
#ifndef CCMakersWSL_h
#define CCMakersWSL_h

#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

class CCMakersWSL
{
  private:
    void initRoutes();

  public:
    String  _ssid;
    String  _hostname;
    CCMakersWSL(String ssid, String hostname);
    void bootWiFi();
    void bootWebUI();
    void handleClient();
    void on(const String &uri, ESP8266WebServer::THandlerFunction handler);
    void send(int code, char* content_type, const String& content);
    String createPage(String body);
    String createPage(String body, String javascript);
};

#endif