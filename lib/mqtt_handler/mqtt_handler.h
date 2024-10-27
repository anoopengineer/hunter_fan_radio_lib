#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>
#include <WiFiClient.h>

#include "mqtt_topics.h"

class DeviceDetails {
   private:
    String identifiers;   // ESP.getChipId()
    String manufacturer;  // ESP.getFlashChipVendorId()
    String model;         // ESP8266
    String name;
    String swVersion;
    String (*getIPAddrFunc)();
    String (*getNetworkNameFunc)();
    String (*getWifiSignalStrengthFunc)();

   public:
    DeviceDetails() = default;

    DeviceDetails(String id, String manuf, String mod, String devName,
                  String version, String (*ipAddrFunc)(),
                  String (*networkFunc)(), String (*wifiSignalFunc)())
        : identifiers(id),
          manufacturer(manuf),
          model(mod),
          name(devName),
          swVersion(version),
          getIPAddrFunc(ipAddrFunc),
          getNetworkNameFunc(networkFunc),
          getWifiSignalStrengthFunc(wifiSignalFunc) {}

    // Getters
    String getIdentifiers() const { return identifiers; }
    String getManufacturer() const { return manufacturer; }
    String getModel() const { return model; }
    String getName() const { return name; }
    String getSwVersion() const { return swVersion; }

    // Getters for dynamic fields
    String getNetworkName() const {
        return getNetworkNameFunc ? getNetworkNameFunc() : "";
    }

    String getWifiSignalStrength() const {
        return getWifiSignalStrengthFunc ? getWifiSignalStrengthFunc() : "";
    }

    String getIpAddress() const { return getIPAddrFunc ? getIPAddrFunc() : ""; }

    // Setters
    void setIdentifiers(const String& id) { identifiers = id; }
    void setManufacturer(const String& manuf) { manufacturer = manuf; }
    void setModel(const String& mod) { model = mod; }
    void setName(const String& devName) { name = devName; }
    void setSwVersion(const String& version) { swVersion = version; }

    void setIpAddress(String (*ipAddrFunc)()) { getIPAddrFunc = ipAddrFunc; }
    void setNetworkNameFunc(String (*networkFunc)()) {
        getNetworkNameFunc = networkFunc;
    }
    void setWifiSignalStrengthFunc(String (*wifiSignalFunc)()) {
        getWifiSignalStrengthFunc = wifiSignalFunc;
    }

    // Method to get system uptime as a formatted string
    String getSystemUptime() const {
        unsigned long uptimeMillis = millis();

        int days = uptimeMillis / (1000L * 60L * 60L * 24L);
        uptimeMillis %= (1000L * 60L * 60L * 24L);
        int hours = uptimeMillis / (1000L * 60L * 60L);
        uptimeMillis %= (1000L * 60L * 60L);
        int minutes = uptimeMillis / (1000L * 60L);
        uptimeMillis %= (1000L * 60L);
        int seconds = uptimeMillis / 1000L;

        return String(days) + " days, " + String(hours) + " hours, " +
               String(minutes) + " minutes, " + String(seconds) + " seconds";
    }
};

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
    const char* mqttUsername;
    const char* mqttPassword;

    WiFiClient wifiClient;
    PubSubClient mqttClient;
    MQTTTopics* mqttTopics;
    DeviceDetails* deviceDetails;

    void (*callback)(char* topic, byte* payload, unsigned int length);

   public:
    MQTTHandler();
    ~MQTTHandler();

    void setup(const char* serverHostName, uint16_t serverPort,
               unsigned long checkIntervalMillis, const char* clientName,
               const char* mqttUsername = nullptr,
               const char* mqttPassword = nullptr,
               MQTTTopics* mqttTopics = nullptr,
               DeviceDetails* deviceDetails = nullptr,
               void (*callback)(char* topic, byte* payload,
                                unsigned int length) = nullptr);

    void connect();
    void disconnect();
    void initializeConnection();

    bool isConnected();
    void checkStatus();

    void setBufferSize(uint16_t bufferSize);
    void setKeepAlive(uint16_t keepAlive);
    void setSocketTimeout(uint16_t socketTimeout);
    void setServerHostName(char* serverHostName);
    void setServerPort(uint16_t serverPort);
    void setCheckIntervalMillis(unsigned long interval);
    void setCredentials(char* username, char* password);
    void sendAutoDiscovery();
};

#endif  // MQTT_HANDLER_H
