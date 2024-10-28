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

    Module module;
    CC1101* radio;

    bool sendPacket(const byte* packet, size_t packetSize, uint32_t delayTime);

   public:
    RFHandler(int csnPin, int gdo0Pin, int gdo2Pin);
    ~RFHandler();

    int16_t setup(float frequency);

    bool fanOn();
    bool fanOff();
    bool lightOn();
    bool lightOff();
};

#endif