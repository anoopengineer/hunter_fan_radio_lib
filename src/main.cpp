#include <RadioLib.h>

#include "config.h"
#include "logger.h"
#include "mqtt_handler.h"
#include "wifi_handler.h"

CC1101 radio = new Module(HUNTER_CSN, HUNTER_GDO0, RADIOLIB_NC, HUNTER_GDO2);

// save transmission state between loops
int transmissionState = RADIOLIB_ERR_NONE;

// flag to indicate that a packet was sent
volatile bool transmittedFlag = false;

// counter to keep track of transmitted packets
int count = 0;

bool once = true;

WiFiHandler wiFiHandler;
MQTTHandler mqttHandler;

void toggleLight() {
    // TODO: set a local variable indicating this is togglelight button keypress
    //  that should be validated against in loop()

    // start transmitting the first packet
    Serial.print(F("[CC1101] Sending first packet ... "));

    // you can transmit C-string or Arduino string up to
    // 64 characters long
    byte byteArr[] = {0xaa, 0xaa, 0xaa};
    radio.fixedPacketLengthMode(sizeof(byteArr));
    LOG("[CC1101] First packet size = " + String(sizeof(byteArr)), LOG_INFO,
        true);
    transmissionState = radio.startTransmit(byteArr, sizeof(byteArr));
}

// this function is called when a complete packet
// is transmitted by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
#if defined(ESP8266) || defined(ESP32)
ICACHE_RAM_ATTR
#endif
void setFlag(void) {
    // we sent a packet, set the flag
    transmittedFlag = true;
}

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
        mqttHandler.setup(MQTT_HOST_NAME, MQTT_PORT, HUNTER_MQTT_CHECK_INTERVAL,
                          MQTT_CLIENT_NAME, MQTT_USERNAME, MQTT_PASSWORD);
        mqttHandler.connect();  // Try to connect to MQTT server
    }
}

void loop() {
    // check if the previous transmission finished
    if (transmittedFlag) {
        // reset flag
        transmittedFlag = false;

        if (transmissionState == RADIOLIB_ERR_NONE) {
            // packet was successfully sent
            Serial.println(F("transmission finished!"));

            // NOTE: when using interrupt-driven transmit method,
            //       it is not possible to automatically measure
            //       transmission data rate using getDataRate()

        } else {
            Serial.print(F("failed, code "));
            Serial.println(transmissionState);
        }

        // clean up after transmission is finished
        // this will ensure transmitter is disabled,
        // RF switch is powered down etc.
        radio.finishTransmit();

        if (once) {
            once = false;
            delay(5);
            byte byteArr2[] = {0xd2, 0x69, 0xa6, 0xd3, 0x6d, 0xa6, 0x92,
                               0x4d, 0xa4, 0x9b, 0x4d, 0xa6, 0x93, 0x49,
                               0xb6, 0xd2, 0x49, 0xa4, 0x92, 0x49, 0xb6,
                               0xd3, 0x6d, 0xb6, 0xd0};
            radio.fixedPacketLengthMode(sizeof(byteArr2));
            LOG("[CC1101] Second packet size = " + String(sizeof(byteArr2)),
                LOG_INFO, true);
            radio.startTransmit(byteArr2, sizeof(byteArr2));
        }

        // // send another one
        // Serial.print(F("[CC1101] Sending another packet ... "));

        // // you can transmit C-string or Arduino string up to
        // // 256 characters long
        // String str = "Hello World! #" + String(count++);
        // radio.setOOK(true);
        // radio.setCrcFiltering(false);
        // radio.disableSyncWordFiltering();
        // transmissionState = radio.startTransmit(str);
    }
    wiFiHandler.checkStatus();
    delay(100);
    mqttHandler.checkStatus();
    delay(100);
}
