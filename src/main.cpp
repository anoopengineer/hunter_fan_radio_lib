#include <RadioLib.h>

#include "config.h"
#include "logger.h"
#include "mqtt_handler.h"
#include "wifi_handler.h"

CC1101 radio = new Module(HUNTER_CSN, HUNTER_GDO0, RADIOLIB_NC, HUNTER_GDO2);

// counter to keep track of transmitted packets
int count = 0;

WiFiHandler wiFiHandler;
MQTTHandler mqttHandler;

MQTTTopics* mqttTopics;

// Function to send a packet and log the result
void sendPacket(const byte* packet, size_t packetSize, uint32_t delayTime) {
    // Set packet length for the radio
    radio.fixedPacketLengthMode(packetSize);

    // Transmit the packet
    int state = radio.transmit(packet, packetSize);
    if (state == RADIOLIB_ERR_NONE) {
        // LOG("[Main] Packet transmitted successfully", LOG_INFO, true);
    } else if (state == RADIOLIB_ERR_PACKET_TOO_LONG) {
        LOG("[Main] Packet was too long. Transmission failed", LOG_ERROR, true);
    } else {
        LOG("[Main] Transmission failed with error: " + String(state),
            LOG_ERROR, true);
    }

    // Wait for the specified delay after transmission
    delayMicroseconds(delayTime);
}

void fanOn() {
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

    for (int i = 0; i < 6; i++) {
        sendPacket(packets[i], packetSizes[i], postTxDelay[i]);
    }
}

void fanOff() {
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

    for (int i = 0; i < 8; i++) {
        // LOG("[Main] Sending packet number " + String(i + 1), LOG_INFO, true);
        sendPacket(packets[i], packetSizes[i], postTxDelay[i]);
    }
}

void lightOn() {
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

    for (int i = 0; i < 8; i++) {
        // LOG("[Main] Sending packet number " + String(i + 1), LOG_INFO, true);
        sendPacket(packets[i], packetSizes[i], postTxDelay[i]);
    }
}

void lightOff() {
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

    for (int i = 0; i < 8; i++) {
        // LOG("[Main] Sending packet number " + String(i + 1), LOG_INFO, true);
        sendPacket(packets[i], packetSizes[i], postTxDelay[i]);
    }
}

void toggleLight_working_version() {
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

    const uint32_t postTxDelay[] = {5210, 25666, 5210, 28519,
                                    5209, 25666, 5209, 10000};

    for (int i = 0; i < 8; i++) {
        LOG("[Main] Sending packet number " + String(i + 1), LOG_INFO, true);
        sendPacket(packets[i], packetSizes[i], postTxDelay[i]);
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String topicStr = String(topic);
    LOG("[Main] Message received on topic: " + topicStr, LOG_INFO, true);
    // Convert the payload to a String
    String message = String((char*)payload).substring(0, length);
    LOG("[Main] Payload: " + message, LOG_INFO, true);

    if (topicStr == mqttTopics->getLightCommandTopic()) {
        if (message == "on") {
            lightOn();
        } else {
            lightOff();
        }
    } else if (topicStr == mqttTopics->getFanCommandTopic()) {
        if (message == "on") {
            fanOn();
        } else {
            fanOff();
        }
    }
}

// this function is called when a complete packet
// is transmitted by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
ICACHE_RAM_ATTR
#endif
void setFlag(void) {
    // transmittedFlag = true;
}

char* getChipId() {
    uint32_t chipId = 0;
    for (int i = 0; i < 17; i += 8) {
        chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }

    // Allocate memory for the hex representation of chipId (8 characters + 1
    // for null terminator)
    static char chipIdStr[9];
    snprintf(chipIdStr, sizeof(chipIdStr), "%08X", chipId);

    return chipIdStr;
}

String getWiFiSSID() { return wiFiHandler.getWiFiSSID(); }

String getWiFiStrength() { return wiFiHandler.getWiFiStrength(); }

String getIPAddress() { return wiFiHandler.getIPAddress(); }

void setup() {
    // Start Serial Connection
    Serial.begin(115200);
    while (!Serial);
    delay(200);
    LOG("Launching Hunter Fan Remote on" + String(ARDUINO_BOARD), LOG_INFO,
        true);
    // Initialize the LED digital pin as an output.
    pinMode(HEARTBEAT_LED, OUTPUT);

    Serial.println("[CC1101] Initializing ... ");
    int state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
    } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true) {
            delay(10);
        }
    }

    radio.setFrequency(FREQUENCY);
    radio.setOOK(true);
    radio.setCrcFiltering(false);
    radio.disableSyncWordFiltering();
    radio.setBitRate(2.54);
    radio.setOutputPower(-15);
    // set the function that will be called
    // when packet transmission is finished
    radio.setPacketSentAction(setFlag);

    wiFiHandler.setup(HUNTER_WIFI_SSID, HUNTER_WIFI_PASSWORD,
                      HUNTER_WIFI_CHECK_INTERVAL);
    if (wiFiHandler.connect()) {
        String chipId = getChipId();
        mqttTopics = new MQTTTopics(MQTT_BASE_TOPIC, MQTT_DEVICE_NAME, chipId);

        DeviceDetails* deviceDetails = new DeviceDetails();
        deviceDetails->setIdentifiers(chipId);
        deviceDetails->setManufacturer("Anoop");
#if defined(ESP8266)
        deviceDetails->setModel("ESP8266");
#elif defined(ESP32)
        deviceDetails->setModel("ESP32");
#else
        deviceDetails->setModel("Unknown Model");
#endif
        deviceDetails->setName(MQTT_DEVICE_NAME);
        deviceDetails->setSwVersion("1.0");
        deviceDetails->setIpAddress(getIPAddress);
        deviceDetails->setNetworkNameFunc(getWiFiSSID);
        deviceDetails->setWifiSignalStrengthFunc(getWiFiStrength);

        mqttHandler.setup(MQTT_HOST_NAME, MQTT_PORT, HUNTER_MQTT_CHECK_INTERVAL,
                          MQTT_DEVICE_NAME, MQTT_USERNAME, MQTT_PASSWORD,
                          mqttTopics, deviceDetails, mqttCallback);
        mqttHandler.connect();  // Try to connect to MQTT server
    }
}

void loop() {
    // check if the previous transmission finished
    // if (transmittedFlag) {
    //     // reset flag
    //     transmittedFlag = false;

    //     // // send another one
    //     // Serial.print(F("[CC1101] Sending another packet ... "));

    //     // // you can transmit C-string or Arduino string up to
    //     // // 256 characters long
    //     // String str = "Hello World! #" + String(count++);
    //     // radio.setOOK(true);
    //     // radio.setCrcFiltering(false);
    //     // radio.disableSyncWordFiltering();
    //     // transmissionState = radio.startTransmit(str);
    // }
    wiFiHandler.checkStatus();
    delay(100);
    mqttHandler.checkStatus();
    delay(100);
}
