#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>
#include <WiFiClient.h>

class MQTTHandler {
   private:
    uint16_t bufferSize;
    uint16_t keepAlive;
    uint16_t socketTimeout;
    const char* serverHostName;
    uint16_t serverPort;
    unsigned long checkIntervalMillis;
    unsigned long lastCheckMillis;
    const char* clientName;
    char* mqttUsername;
    char* mqttPassword;

    WiFiClient wifiClient;
    PubSubClient mqttClient;

   public:
    MQTTHandler();

    void setup(const char* serverHostName, uint16_t serverPort,
               unsigned long checkIntervalMillis, const char* clientName,
               char* mqttUsername = nullptr, char* mqttPassword = nullptr);

    void connect();
    void disconnect();

    bool isConnected();
    void checkStatus();

    void setBufferSize(uint16_t bufferSize);
    void setKeepAlive(uint16_t keepAlive);
    void setSocketTimeout(uint16_t socketTimeout);
    void setServerHostName(char* serverHostName);
    void setServerPort(uint16_t serverPort);
    void setCheckIntervalMillis(unsigned long interval);
    void setCredentials(char* username, char* password);
};

#endif  // MQTT_HANDLER_H
