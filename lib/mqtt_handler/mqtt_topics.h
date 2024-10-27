#ifndef MQTT_TOPICS_H
#define MQTT_TOPICS_H

#include <Arduino.h>

class MQTTTopics {
   private:
    String baseTopic;
    String deviceName;
    String deviceId;

   public:
    MQTTTopics(const String& baseTopic, const String& deviceName,
               const String& deviceId);

    String getInfoTopic() const;
    String getInfoConfigTopic() const;
    String getAvailabilityTopic() const;
    String getLightStateTopic() const;
    String getLightCommandTopic() const;
    String getLightConfigTopic() const;
    String getFanStateTopic() const;
    String getFanCommandTopic() const;
    String getFanPercentStateTopic() const;
    String getFanPercentCommandTopic() const;
    String getFanModeStateTopic() const;
    String getFanModeCommandTopic() const;
    String getFanConfigTopic() const;
};

#endif  // MQTT_TOPICS_H
