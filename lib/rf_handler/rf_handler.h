#ifndef RFHANDLER_H
#define RFHANDLER_H

#include <Arduino.h>
#include <RadioLib.h>

#define RADIO_INIT_SUCCESS 100

class RFHandler {
   private:
    int csnPin;
    int gdo0Pin;
    int gdo2Pin;
    int messageRepeatCount;

    Module module;
    CC1101* radio;

   public:
    RFHandler(int csnPin, int gdo0Pin, int gdo2Pin, int messageRepeatCount);
    ~RFHandler();

    int16_t setup(float frequency);

    void fanOn();
    void fanOff();
    void setFanSpeed(int speed);
    void lightOn();
    void lightOff();
};

#endif