#define HEARTBEAT_LED 15

#ifdef ESP32
#define HUNTER_GDO0 2
#define HUNTER_GDO2 4
#define HUNTER_SCK 18
#define HUNTER_MOSI 23
#define HUNTER_MISO 19
#define HUNTER_CSN 5  // CS aka chip select

#elif ESP8266  // for esp8266! Receiver on pin 4 = D2. Transmit on pin 5 = D1.

#else  // for Arduino! Receiver on interrupt 0 => that is pin #2. Transmit on
       // pin 6.

#endif

#define DEBUG_LVL 4

#define FREQUENCY 433.92

#define HUNTER_WIFI_CHECK_INTERVAL 5 * 60 * 1000
#define HUNTER_MQTT_CHECK_INTERVAL 5 * 60 * 1000

// # git pre-commit hook prevents git commits if the below two values are
// non-empty

#define MQTT_HOST_NAME "homeassistant.lan"
#define MQTT_PORT 1883
#define MQTT_CLIENT_NAME "hunterfan"