#include <Arduino.h>
#include <CCMakersWSL.h>

CCMakersWSL::CCMakersWSL(int pin) {
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void CCMakersWSL::ping() {
  digitalWrite(_pin, HIGH);
  delay(250);
  digitalWrite(_pin, LOW);
  delay(250);
}

