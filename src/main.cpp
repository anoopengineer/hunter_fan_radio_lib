
#include "config.h"
#include "logger.h"
#include "mqtt_handler.h"
#include "rf_handler.h"
#include "wifi_handler.h"

// counter to keep track of transmitted packets
int count = 0;

WiFiHandler wiFiHandler;
MQTTHandler mqttHandler;
RFHandler rfHandler(HUNTER_CSN, HUNTER_GDO0, HUNTER_GDO2);

MQTTTopics* mqttTopics;

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    String topicStr = String(topic);
    LOG("[Main] Message received on topic: " + topicStr, LOG_INFO, true);
    // Convert the payload to a String
    String message = String((char*)payload).substring(0, length);
    LOG("[Main] Payload: " + message, LOG_INFO, true);

    bool processed = false;

    if (topicStr == mqttTopics->getLightCommandTopic()) {
        if (message == "on") {
            rfHandler.lightOn();
        } else {
            rfHandler.lightOff();
        }
        processed = true;
    } else if (topicStr == mqttTopics->getFanCommandTopic()) {
        if (message == "on") {
            rfHandler.fanOn();
        } else {
            rfHandler.fanOff();
        }
        processed = true;
    }
    if (processed) {
        // add a delay so that the RF receiver doesn't hang
        delay(2000);
    }
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
    if (RADIO_INIT_SUCCESS != rfHandler.setup(FREQUENCY)) {
        LOG("[Main] CC1101 Radio Initialization failed. Hanging forever",
            LOG_ERROR, true);
        while (true) {
            delay(10);
        }
    }

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
    wiFiHandler.checkStatus();
    delay(100);
    mqttHandler.checkStatus();
    delay(100);
}
