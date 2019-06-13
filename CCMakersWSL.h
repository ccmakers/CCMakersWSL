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
    int _pin;
  public:
    CCMakersWSL(int pin);
    void ping();
};

#endif