#include "rf_handler.h"

#include "logger.h"

// CC1101 radio = new Module(HUNTER_CSN, HUNTER_GDO0, RADIOLIB_NC, HUNTER_GDO2);

// Constructor
RFHandler::RFHandler(int csnPin, int gdo0Pin, int gdo2Pin)
    : csnPin(csnPin),
      gdo0Pin(gdo0Pin),
      gdo2Pin(gdo2Pin),
      module(csnPin, gdo0Pin, RADIOLIB_NC, gdo2Pin) {
    radio = new CC1101(&module);
}

// Destructor to clean up
RFHandler::~RFHandler() { delete radio; }

// Function to send a packet and log the result
bool RFHandler::sendPacket(const byte* packet, size_t packetSize,
                           uint32_t delayTime) {
    // Set packet length for the radio
    radio->fixedPacketLengthMode(packetSize);

    // Transmit the packet
    int state = radio->transmit(packet, packetSize);
    if (state == RADIOLIB_ERR_NONE) {
        return true;
    } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
        LOG("[Main] Packet was too long. Transmission failed", LOG_ERROR, true);
        return false;
    } else {
        LOG("[Main] Transmission failed with error: " + String(state),
            LOG_ERROR, true);
        return false;
    }

    // Wait for the specified delay after transmission
    delayMicroseconds(delayTime);
}

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
    radio->setBitRate(2.54);
    radio->setOutputPower(-15);
    return RADIO_INIT_SUCCESS;
}

bool RFHandler::fanOn() {
    LOG("[Main] Fan On", LOG_INFO, true);
    const byte preamble[] = {0xaa, 0xaa, 0xaa};
    const byte payload1[] = {0xd2, 0x69, 0xa6, 0xd3, 0x6d, 0xa6, 0x92,
                             0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49,
                             0xb6, 0xd2, 0x49, 0x24, 0x93, 0x49, 0xb6,
                             0xdb, 0x6d, 0xa6, 0xd0};
    const byte* packets[] = {preamble, payload1, preamble,
                             payload1, preamble, payload1};
    const size_t packetSizes[] = {sizeof(preamble), sizeof(payload1),
                                  sizeof(preamble), sizeof(payload1),
                                  sizeof(preamble), sizeof(payload1)};
    const uint32_t postTxDelay[] = {5209, 25666, 5210, 28503, 5208, 200000};

    bool retVal = true;
    for (int i = 0; i < 6; i++) {
        retVal &= sendPacket(packets[i], packetSizes[i], postTxDelay[i]);
    }
    return retVal;
}
bool RFHandler::fanOff() {
    LOG("[Main] Fan Off", LOG_INFO, true);
    const byte preamble[] = {0xaa, 0xaa, 0xaa};
    const byte payload1[] = {0xd2, 0x69, 0xa6, 0xd3, 0x6d, 0xa6, 0x92,
                             0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49,
                             0xb6, 0xd2, 0x49, 0x24, 0x92, 0x4d, 0xb6,
                             0xdb, 0x6d, 0xb6, 0x90};

    const byte payload2[] = {0xd2, 0x69, 0xa6, 0xd3, 0x6d, 0xa6, 0x92,
                             0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49,
                             0xb6, 0xd2, 0x49, 0x26, 0x92, 0x69, 0xb6,
                             0xdb, 0x4d, 0xb4, 0xd0};

    const byte* packets[] = {preamble, payload1, preamble, payload1,
                             preamble, payload2, preamble, payload2};
    const size_t packetSizes[] = {
        sizeof(preamble), sizeof(payload1), sizeof(preamble), sizeof(payload1),
        sizeof(preamble), sizeof(payload2), sizeof(preamble), sizeof(payload2)};

    const uint32_t postTxDelay[] = {5212, 25669, 5211, 27456,
                                    5211, 25665, 5210, 200000};

    bool retVal = true;
    for (int i = 0; i < 8; i++) {
        // LOG("[Main] Sending packet number " + String(i + 1), LOG_INFO, true);
        retVal &= sendPacket(packets[i], packetSizes[i], postTxDelay[i]);
    }
    return retVal;
}
bool RFHandler::lightOn() {
    LOG("[Main] Light On", LOG_INFO, true);
    const byte preamble[] = {0xaa, 0xaa, 0xaa};
    const byte payload1[] = {0xd2, 0x69, 0xa6, 0xd3, 0x6d, 0xa6, 0x92,
                             0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49,
                             0xb6, 0xd2, 0x49, 0xa4, 0x92, 0x49, 0xb6,
                             0xd3, 0x6d, 0xb6, 0xd0};
    const byte payload2[] = {0xd2, 0x69, 0xa6, 0xd3, 0x6d, 0xa6, 0x92,
                             0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49,
                             0xb6, 0xd2, 0x49, 0x26, 0x93, 0x69, 0xb6,
                             0xdb, 0x4d, 0xa4, 0xd0};

    const byte* packets[] = {preamble, payload1, preamble, payload1,
                             preamble, payload2, preamble, payload2};
    const size_t packetSizes[] = {
        sizeof(preamble), sizeof(payload1), sizeof(preamble), sizeof(payload1),
        sizeof(preamble), sizeof(payload2), sizeof(preamble), sizeof(payload2)};

    const uint32_t postTxDelay[] = {5208, 25666, 5210, 28503,
                                    5208, 25666, 5208, 200000};

    bool retVal = true;
    for (int i = 0; i < 8; i++) {
        // LOG("[Main] Sending packet number " + String(i + 1), LOG_INFO, true);
        retVal &= sendPacket(packets[i], packetSizes[i], postTxDelay[i]);
    }
    return retVal;
}
bool RFHandler::lightOff() {
    LOG("[Main] Light Off", LOG_INFO, true);
    const byte preamble[] = {0xaa, 0xaa, 0xaa};
    const byte payload1[] = {0xd2, 0x69, 0xa6, 0xd3, 0x6d, 0xa6, 0x92,
                             0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49,
                             0xb6, 0xd2, 0x4d, 0x24, 0x92, 0x49, 0xb6,
                             0x9b, 0x6d, 0xb6, 0xd0};
    const byte payload2[] = {0xd2, 0x69, 0xa6, 0xd3, 0x6d, 0xa6, 0x92,
                             0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49,
                             0xb6, 0xd2, 0x49, 0x26, 0x93, 0x69, 0xb6,
                             0xdb, 0x4d, 0xa4, 0xd0};

    const byte* packets[] = {preamble, payload1, preamble, payload1,
                             preamble, payload2, preamble, payload2};
    const size_t packetSizes[] = {
        sizeof(preamble), sizeof(payload1), sizeof(preamble), sizeof(payload1),
        sizeof(preamble), sizeof(payload2), sizeof(preamble), sizeof(payload2)};

    const uint32_t postTxDelay[] = {5210, 25666, 5210, 25666,
                                    5209, 25666, 5209, 200000};

    bool retVal = true;
    for (int i = 0; i < 8; i++) {
        // LOG("[Main] Sending packet number " + String(i + 1), LOG_INFO, true);
        retVal &= sendPacket(packets[i], packetSizes[i], postTxDelay[i]);
    }
    return retVal;
}