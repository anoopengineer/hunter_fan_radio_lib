#ifndef WIFIHANDLER_H
#define WIFIHANDLER_H

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ARDUINO)  // For Arduino boards
#include <WiFiNINA.h>   // Example for Arduino WiFi shield/module
#endif

// Define enum for return values of handleWiFiConnection
enum WiFiStatus {
    WIFI_CONNECTED,            // WiFi is already connected
    WIFI_RECONNECTED_SUCCESS,  // Reconnection was successful
    WIFI_RECONNECTED_FAILED,   // Reconnection attempt failed
    WIFI_CHECK_PENDING         // Still within the check interval
};

class WiFiHandler {
   private:
    const char* ssid;         // WiFi SSID
    const char* password;     // WiFi Password
    unsigned long lastCheck;  // Timestamp of the last check
    unsigned long
        intervalMillis;  // Interval between WiFi checks (milliseconds)

   public:
    // Constructor
    WiFiHandler();

    // Setup WiFi credentials and interval for checking connection
    void setup(const char* wifiSSID, const char* wifiPassword,
               unsigned long checkIntervalMillis);

    // Attempt to connect to WiFi
    bool connect();

    // Handle WiFi status and reconnection, callable from loop
    WiFiStatus checkStatus();
};

#endif
