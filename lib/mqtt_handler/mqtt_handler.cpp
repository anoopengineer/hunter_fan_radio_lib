#include "mqtt_handler.h"

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
      mqttPassword(nullptr) {}

void MQTTHandler::setup(const char* serverHostName, uint16_t serverPort,
                        unsigned long checkIntervalMillis,
                        const char* clientName, char* mqttUsername,
                        char* mqttPassword) {
    this->serverHostName = serverHostName;
    this->serverPort = serverPort;
    this->checkIntervalMillis = checkIntervalMillis;
    this->clientName = clientName;
    this->mqttUsername = mqttUsername;
    this->mqttPassword = mqttPassword;

    mqttClient.setServer(serverHostName, serverPort);
}

void MQTTHandler::connect() {
    LOG("[MQTT] Connecting to MQTT " + String(this->serverHostName) + ":" +
            String(this->serverPort),
        LOG_INFO, true);
    if (!mqttClient.connected()) {
        while (!mqttClient.connected()) {
            if (mqttClient.connect(this->clientName, this->mqttUsername,
                                   this->mqttPassword)) {
                LOG("[MQTT] Successfully connected", LOG_INFO, true);
                // Successfully connected
                // TODO send the initial broadcast message
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

void MQTTHandler::checkStatus() {
    unsigned long currentMillis = millis();

    if (currentMillis - lastCheckMillis >= checkIntervalMillis) {
        LOG("[MQTT] Checking MQTT status", LOG_DEBUG, true);
        lastCheckMillis = currentMillis;
        if (!isConnected()) {
            connect();
        }
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
