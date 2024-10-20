#define HEARTBEAT_LED 15

#ifdef ESP32 
  #define GDO0 2 
  #define GDO2 4
  #define SCK 18
  #define MOSI 23
  #define MISO 19
  #define CSN 5 //CS aka chip select
  
#elif ESP8266  // for esp8266! Receiver on pin 4 = D2. Transmit on pin 5 = D1.
  
#else // for Arduino! Receiver on interrupt 0 => that is pin #2. Transmit on pin 6.
  
#endif 

#define DEBUG_LVL 10

#define FREQUENCY     433.92
