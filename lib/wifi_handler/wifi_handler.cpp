#include "wifi_handler.h"

#include "logger.h"

// Constructor
WiFiHandler::WiFiHandler()
    : ssid(nullptr), password(nullptr), lastCheck(0), intervalMillis(0) {}

// Setup WiFi credentials and check interval
void WiFiHandler::setup(const char* wifiSSID, const char* wifiPassword,
                        unsigned long checkIntervalMillis) {
    ssid = wifiSSID;
    password = wifiPassword;
    intervalMillis = checkIntervalMillis;
}

// Attempt to connect to WiFi
bool WiFiHandler::connect() {
    LOG("[WiFi] Attempting to connect...", LOG_DEBUG, false);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        LOG(".", LOG_DEBUG, false);
    }
    // put a new line after the above dots to not screw up subsequent logs
    LOG("", LOG_DEBUG, true);

    if (WiFi.status() == WL_CONNECTED) {
        LOG("[WiFi] Connected. IP address: " + WiFi.localIP().toString(),
            LOG_DEBUG, true);
        return true;
    } else {
        LOG("[WiFi] Connection failed.", LOG_ERROR, true);
        return false;
    }
}

// Handle WiFi status and attempt reconnection if needed, callable from loop()
WiFiStatus WiFiHandler::checkStatus() {
    unsigned long currentMillis = millis();
    if (currentMillis - lastCheck >= intervalMillis) {
        LOG("[WiFi] Checking WiFi status", LOG_DEBUG, true);
        lastCheck = currentMillis;
        if (WiFi.status() == WL_CONNECTED) {
            return WIFI_CONNECTED;  // WiFi is already connected
        } else if (connect()) {
            return WIFI_RECONNECTED_SUCCESS;  // Reconnection successful
        } else {
            return WIFI_RECONNECTED_FAILED;  // Reconnection failed
        }
    }
    return WIFI_CHECK_PENDING;  // No action taken yet (still within interval)
}

String WiFiHandler::getWiFiStrength() { return String(WiFi.RSSI()); }

String WiFiHandler::getWiFiSSID() { return WiFi.SSID(); }

String WiFiHandler::getIPAddress() { return WiFi.localIP().toString();    }
