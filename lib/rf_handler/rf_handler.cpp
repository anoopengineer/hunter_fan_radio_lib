#include "rf_handler.h"

#include "logger.h"

// CC1101 radio = new Module(HUNTER_CSN, HUNTER_GDO0, RADIOLIB_NC, HUNTER_GDO2);

// Constructor
RFHandler::RFHandler(int csnPin, int gdo0Pin, int gdo2Pin,
                     int messageRepeatCount)
    : csnPin(csnPin),
      gdo0Pin(gdo0Pin),
      gdo2Pin(gdo2Pin),
      messageRepeatCount(messageRepeatCount),
      module(csnPin, gdo0Pin, RADIOLIB_NC, gdo2Pin) {
    radio = new CC1101(&module);
}

// Destructor to clean up
RFHandler::~RFHandler() { delete radio; }

int16_t RFHandler::setup(float frequency) {
    Serial.println("[CC1101] Initializing ... ");
    int state = radio->begin();
    if (state != RADIOLIB_ERR_NONE) {
        return state;
    }
    radio->setFrequency(frequency);
    radio->setOOK(true);
    radio->setCrcFiltering(false);
    radio->disableSyncWordFiltering();
    // Magic numbers reverse engineered from URH capture
    // radio->setBitRate(2.54);
    // i think it worked on 3 as well
    radio->setBitRate(2);
    // radio->setOutputPower(-15);
    return RADIO_INIT_SUCCESS;
}

void RFHandler::fanOn() {
    LOG("[Main] fanOn starting...", LOG_INFO, true);

    const byte payload[] = {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6,
                            0xd3, 0x6d, 0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d,
                            0xa6, 0x93, 0x49, 0xb6, 0xd2, 0x49, 0x24, 0x93,
                            0x49, 0xb6, 0xdb, 0x6d, 0xa6, 0xd0};

    radio->fixedPacketLengthMode(sizeof(payload));

    for (int j = 0; j < messageRepeatCount; j++) {
        radio->transmit(payload, sizeof(payload));
        delayMicroseconds(25668);

        radio->transmit(payload, sizeof(payload));
        delayMicroseconds(25668);

        radio->transmit(payload, sizeof(payload));
        delayMicroseconds(25668);
    }

    LOG("[Main] fanOn Completed", LOG_INFO, true);
}
void RFHandler::fanOff() {
    LOG("[Main] fanOff starting...", LOG_INFO, true);
    const byte payload1[] = {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6,
                             0xd3, 0x6d, 0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d,
                             0xa6, 0x93, 0x49, 0xb6, 0xd2, 0x49, 0x24, 0x92,
                             0x4d, 0xb6, 0xdb, 0x6d, 0xb6, 0x90};

    const byte payload2[] = {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6,
                             0xd3, 0x6d, 0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d,
                             0xa6, 0x93, 0x49, 0xb6, 0xd2, 0x49, 0x26, 0x92,
                             0x69, 0xb6, 0xdb, 0x4d, 0xb4, 0xd0};

    radio->fixedPacketLengthMode(sizeof(payload2));

    for (int j = 0; j < messageRepeatCount; j++) {
        radio->transmit(payload1, sizeof(payload1));
        delayMicroseconds(25668);

        radio->transmit(payload1, sizeof(payload1));
        delayMicroseconds(25668);

        radio->transmit(payload2, sizeof(payload2));
        delayMicroseconds(25668);

        // radio->fixedPacketLengthMode(sizeof(payload2));
        radio->transmit(payload2, sizeof(payload2));
        delayMicroseconds(25668);
    }
    LOG("[Main] fanOff completed", LOG_INFO, true);
}

void RFHandler::setFanSpeed(int speed) {
    LOG("[Main] setFanSpeed started. speed = " + String(speed), LOG_INFO, true);
    if (speed < 1 || speed > 4) {
        LOG("[Main] Invalid speed value", LOG_ERROR, true);
        return;
    }

    const byte payloads[][30] = {
        {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6, 0xd3, 0x6d,
         0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49, 0xb6,
         0xd2, 0x49, 0x24, 0x92, 0x69, 0xb6, 0xdb, 0x6d, 0xb4, 0xd0},
        {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6, 0xd3, 0x6d,
         0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49, 0xb6,
         0xd2, 0x49, 0x24, 0x93, 0x49, 0xb6, 0xdb, 0x6d, 0xa6, 0xd0},
        {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6, 0xd3, 0x6d,
         0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49, 0xb6,
         0xd2, 0x49, 0x26, 0x92, 0x49, 0xb6, 0xdb, 0x4d, 0xb6, 0xd0},
        {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6, 0xd3, 0x6d,
         0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49, 0xb6,
         0xd2, 0x49, 0x34, 0x92, 0x49, 0xb6, 0xda, 0x6d, 0xb6, 0xd0}};

    radio->fixedPacketLengthMode(sizeof(payloads[speed - 1]));

    for (int j = 0; j < messageRepeatCount; j++) {
        radio->transmit(payloads[speed - 1], sizeof(payloads[speed - 1]));
        delayMicroseconds(25668);

        radio->transmit(payloads[speed - 1], sizeof(payloads[speed - 1]));
        delayMicroseconds(25668);

        radio->transmit(payloads[speed - 1], sizeof(payloads[speed - 1]));
        delayMicroseconds(25668);
    }

    LOG("[Main] setFanSpeed completed", LOG_INFO, true);
}
void RFHandler::lightOn() {
    LOG("[Main] lightOn started...", LOG_INFO, true);

    const byte payload1[] = {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6,
                             0xd3, 0x6d, 0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d,
                             0xa6, 0x93, 0x49, 0xb6, 0xd2, 0x49, 0xa4, 0x92,
                             0x49, 0xb6, 0xd3, 0x6d, 0xb6, 0xd0};

    const byte payload2[] = {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6,
                             0xd3, 0x6d, 0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d,
                             0xa6, 0x93, 0x49, 0xb6, 0xd2, 0x49, 0x26, 0x93,
                             0x69, 0xb6, 0xdb, 0x4d, 0xa4, 0xd0};

    radio->fixedPacketLengthMode(sizeof(payload2));

    for (int j = 0; j < messageRepeatCount; j++) {
        radio->transmit(payload1, sizeof(payload1));
        delayMicroseconds(25668);

        radio->transmit(payload1, sizeof(payload1));
        delayMicroseconds(25668);

        radio->transmit(payload2, sizeof(payload2));
        delayMicroseconds(25668);

        radio->transmit(payload2, sizeof(payload2));
        delayMicroseconds(25668);
    }
    LOG("[Main] lightOn completed", LOG_INFO, true);
}
void RFHandler::lightOff() {
    LOG("[Main] lightOff starting...", LOG_INFO, true);
    const byte payload1[] = {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6,
                             0xd3, 0x6d, 0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d,
                             0xa6, 0x93, 0x49, 0xb6, 0xd2, 0x4d, 0x24, 0x92,
                             0x49, 0xb6, 0x9b, 0x6d, 0xb6, 0xd0};
    const byte payload2[] = {0xaa, 0xaa, 0xaa, 0xa0, 0x00, 0xd2, 0x69, 0xa6,
                             0xd3, 0x6d, 0xa6, 0x92, 0x4d, 0xa4, 0x9b, 0x4d,
                             0xa6, 0x93, 0x49, 0xb6, 0xd2, 0x49, 0x26, 0x93,
                             0x69, 0xb6, 0xdb, 0x4d, 0xa4, 0xd0};

    radio->fixedPacketLengthMode(sizeof(payload2));

    for (int j = 0; j < messageRepeatCount; j++) {
        radio->transmit(payload1, sizeof(payload1));
        delayMicroseconds(25668);

        radio->transmit(payload1, sizeof(payload1));
        delayMicroseconds(25668);

        radio->transmit(payload2, sizeof(payload2));
        delayMicroseconds(25668);

        radio->transmit(payload2, sizeof(payload2));
        delayMicroseconds(25668);
    }
    LOG("[Main] lightOff completed", LOG_INFO, true);
}