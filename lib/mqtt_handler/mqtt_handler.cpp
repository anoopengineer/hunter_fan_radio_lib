#include "mqtt_handler.h"

#include <ArduinoJson.h>

#include "logger.h"

MQTTHandler::MQTTHandler()
    : bufferSize(2048),
      keepAlive(60),
      socketTimeout(60),
      serverHostName(nullptr),
      serverPort(8123),
      checkIntervalMillis(5000),
      lastCheckMillis(0),
      mqttClient(wifiClient),
      clientName(nullptr),
      mqttUsername(nullptr),
      mqttPassword(nullptr),
      deviceDetails(nullptr),
      callback(nullptr) {}

MQTTHandler::~MQTTHandler() { delete mqttTopics; }

void MQTTHandler::setup(const char* serverHostName, uint16_t serverPort,
                        unsigned long checkIntervalMillis,
                        const char* clientName, const char* mqttUsername,
                        const char* mqttPassword, MQTTTopics* mqttTopics,
                        DeviceDetails* deviceDetails,
                        void (*callback)(char* topic, byte* payload,
                                         unsigned int length)) {
    this->serverHostName = serverHostName;
    this->serverPort = serverPort;
    this->checkIntervalMillis = checkIntervalMillis;
    this->clientName = clientName;
    this->mqttUsername = mqttUsername;
    this->mqttPassword = mqttPassword;
    this->mqttTopics = mqttTopics;
    this->deviceDetails = deviceDetails;

    mqttClient.setServer(serverHostName, serverPort);
    mqttClient.setCallback(callback);
    mqttClient.setBufferSize(1024);
}

void MQTTHandler::initializeConnection() {
    mqttClient.publish(mqttTopics->getAvailabilityTopic().c_str(), "online");
    // mqttClient.subscribe(mqttTopics->getLightStateTopic().c_str());
    mqttClient.subscribe(mqttTopics->getLightCommandTopic().c_str());
    mqttClient.subscribe(mqttTopics->getFanCommandTopic().c_str());
    // mqttClient.subscribe(mqttTopics->getFanStateTopic().c_str());
    // mqttClient.subscribe(mqttTopics->getFanPercentStateTopic().c_str());
    mqttClient.subscribe(mqttTopics->getFanPercentCommandTopic().c_str());
    // mqttClient.subscribe(mqttTopics->getFanModeStateTopic().c_str());
    mqttClient.subscribe(mqttTopics->getFanModeCommandTopic().c_str());
}

void MQTTHandler::connect() {
    LOG("[MQTT] Connecting to MQTT " + String(this->serverHostName) + ":" +
            String(this->serverPort),
        LOG_INFO, true);
    if (!mqttClient.connected()) {
        while (!mqttClient.connected()) {
            if (mqttClient.connect(this->clientName, this->mqttUsername,
                                   this->mqttPassword,
                                   mqttTopics->getAvailabilityTopic().c_str(),
                                   1, true, "offline")) {
                LOG("[MQTT] Successfully connected", LOG_INFO, true);
                // Successfully connected
                initializeConnection();
                sendAutoDiscovery();
            } else {
                // Connection failed; wait before retrying
                LOG("[MQTT] Connection failed. " + String(mqttClient.state()),
                    LOG_ERROR, true);
                delay(5000);
            }
        }
    }
}

void MQTTHandler::disconnect() {
    if (mqttClient.connected()) {
        mqttClient.disconnect();
    }
}

bool MQTTHandler::isConnected() { return mqttClient.connected(); }

void MQTTHandler::sendAutoDiscovery() {
    String syspayload = "";
    String lightPayload = "";
    String fanPayload = "";
    String infoSensorPayload = "";

    JsonDocument deviceJSON;
    JsonObject deviceObj = deviceJSON["device"].to<JsonObject>();
    deviceObj["identifiers"] = deviceDetails->getIdentifiers();
    deviceObj["manufacturer"] = deviceDetails->getManufacturer();
    deviceObj["model"] = deviceDetails->getModel();
    deviceObj["name"] =
        deviceDetails->getName() + "-" + deviceDetails->getIdentifiers();
    deviceObj["sw_version"] = "1.0";
    //   serializeJson(deviceObj, deviceStr);

    // JsonDocument sysinfoJSON;
    // sysinfoJSON["device"] = deviceObj;
    // sysinfoJSON["name"] = "System Info";
    // sysinfoJSON["Uptime"] = deviceDetails->getSystemUptime();
    // sysinfoJSON["Network"] = deviceDetails->getNetworkName();
    // sysinfoJSON["Signal Strength"] = deviceDetails->getWifiSignalStrength();
    // sysinfoJSON["IP Address"] = deviceDetails->getIpAddress();
    // serializeJson(sysinfoJSON, syspayload);

    // JsonDocument infoSensorJSON;
    // infoSensorJSON["device"] = deviceObj;
    // infoSensorJSON["name"] = "Connectivity Sensor";
    // infoSensorJSON["icon"] = "mdi:chip";
    // infoSensorJSON["unique_id"] = deviceDetails->getName() + "-" +
    //                               deviceDetails->getIdentifiers() + "-info";
    // infoSensorJSON["state_topic"] = mqttTopics->getAvailabilityTopic();
    // infoSensorJSON["json_attributes_topic"] = mqttTopics->getInfoTopic();
    // serializeJson(infoSensorJSON, infoSensorPayload);

    JsonDocument lightJSON;
    lightJSON["device"] = deviceObj;
    lightJSON["name"] = "Light";
    lightJSON["unique_id"] = deviceDetails->getName() + "-" +
                             deviceDetails->getIdentifiers() + "-light";
    // lightJSON["state_topic"] = mqttTopics->getLightStateTopic();
    lightJSON["command_topic"] = mqttTopics->getLightCommandTopic();
    lightJSON["payload_on"] = "on";
    lightJSON["payload_off"] = "off";
    // lightJSON["availability_topic"] = mqttTopics->getAvailabilityTopic();
    // lightJSON["payload_available"] = "online";
    // lightJSON["payload_not_available"] = "offline";
    serializeJson(lightJSON, lightPayload);

    LOG("[MQTT] Size of lightPayload = " + String(lightPayload.length()),
        LOG_INFO, true);
    JsonDocument fanJSON;
    fanJSON["device"] = deviceObj;
    fanJSON["name"] = "Fan";
    fanJSON["unique_id"] = deviceDetails->getName() + "-" +
                           deviceDetails->getIdentifiers() + "-fan";
    // fanJSON["state_topic"] = FAN_STATE_TOPIC;
    fanJSON["command_topic"] = mqttTopics->getFanCommandTopic();
    fanJSON["payload_on"] = "on";
    fanJSON["payload_off"] = "off";
    // fanJSON["percentage_state_topic"] = FAN_PERCENT_STATE_TOPIC;
    fanJSON["percentage_command_topic"] =
        mqttTopics->getFanPercentCommandTopic();
    fanJSON["speed_range_min"] = 1;
    fanJSON["speed_range_max"] = 4;
    // fanJSON["preset_mode_state_topic"] = FAN_MODE_STATE_TOPIC;
    fanJSON["preset_mode_command_topic"] = mqttTopics->getFanModeCommandTopic();
    JsonArray FAN_PRESET_MODES = fanJSON["preset_modes"].to<JsonArray>();
    FAN_PRESET_MODES.add("Summer");
    FAN_PRESET_MODES.add("Winter");
    // fanJSON["availability_topic"] = AVAILABILITY_TOPIC;
    // fanJSON["payload_available"] = "online";
    // fanJSON["payload_not_available"] = "offline";
    serializeJson(fanJSON, fanPayload);
    LOG("[MQTT] Size of fanPayload = " + String(fanPayload.length()), LOG_INFO,
        true);

    if (mqttClient.connected()) {
        LOG("Publishing to " + mqttTopics->getAvailabilityTopic() +
                " data: online",
            LOG_DEBUG, true);
        mqttClient.publish(mqttTopics->getAvailabilityTopic().c_str(),
                           "online");
        delay(100);
        LOG("Publishing to " + mqttTopics->getLightConfigTopic() +
                " data: " + String(lightPayload),
            LOG_DEBUG, true);
        mqttClient.publish(mqttTopics->getLightConfigTopic().c_str(),
                           lightPayload.c_str(), true);
        delay(100);
        LOG("Publishing to " + mqttTopics->getFanConfigTopic() +
                " data: " + String(fanPayload),
            LOG_DEBUG, true);
        mqttClient.publish(mqttTopics->getFanConfigTopic().c_str(),
                           fanPayload.c_str(), true);
        delay(100);
        // LOG("Publishing to " + mqttTopics->getInfoConfigTopic() +
        //         " data: " + String(infoSensorPayload),
        //     LOG_DEBUG, true);
        // mqttClient.publish(mqttTopics->getInfoConfigTopic().c_str(),
        //                    infoSensorPayload.c_str(), true);
        // delay(100);
        // LOG("Publishing to " + mqttTopics->getInfoTopic() +
        //         " data: " + String(syspayload),
        //     LOG_DEBUG, true);
        // mqttClient.publish(mqttTopics->getInfoTopic().c_str(),
        //                    syspayload.c_str(), true);
        // delay(100);
    } else {
        LOG("[MQTT] Not connected", LOG_ERROR, true);
    }
}

void MQTTHandler::checkStatus() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastCheckMillis >= checkIntervalMillis) {
        LOG("[MQTT] Checking MQTT status", LOG_DEBUG, true);
        lastCheckMillis = currentMillis;
        if (!isConnected()) {
            connect();
        }
        sendAutoDiscovery();
    }

    mqttClient.loop();
}

void MQTTHandler::setBufferSize(uint16_t bufferSize) {
    this->bufferSize = bufferSize;
    mqttClient.setBufferSize(bufferSize);
}

void MQTTHandler::setKeepAlive(uint16_t keepAlive) {
    this->keepAlive = keepAlive;
    mqttClient.setKeepAlive(keepAlive);
}

void MQTTHandler::setSocketTimeout(uint16_t socketTimeout) {
    this->socketTimeout = socketTimeout;
}

void MQTTHandler::setServerHostName(char* serverHostName) {
    this->serverHostName = serverHostName;
    mqttClient.setServer(serverHostName, serverPort);
}

void MQTTHandler::setServerPort(uint16_t serverPort) {
    this->serverPort = serverPort;
    mqttClient.setServer(serverHostName, serverPort);
}

void MQTTHandler::setCheckIntervalMillis(unsigned long interval) {
    this->checkIntervalMillis = interval;
}
