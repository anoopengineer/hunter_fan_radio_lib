#include "mqtt_topics.h"

MQTTTopics::MQTTTopics(const String& baseTopic, const String& deviceName,
                       const String& deviceId)
    : baseTopic(baseTopic), deviceName(deviceName), deviceId(deviceId) {}

String MQTTTopics::getInfoTopic() const {
    return baseTopic + "/sensor/" + deviceName + "-" + deviceId +
           "/info/attributes";
}

String MQTTTopics::getInfoConfigTopic() const {
    return baseTopic + "/sensor/" + deviceName + "-" + deviceId +
           "/info/config";
}

String MQTTTopics::getAvailabilityTopic() const {
    return baseTopic + "/sensor/" + deviceName + "-" + deviceId + "/status";
}

String MQTTTopics::getLightStateTopic() const {
    return baseTopic + "/light/" + deviceName + "-" + deviceId + "/state";
}

String MQTTTopics::getLightCommandTopic() const {
    return baseTopic + "/light/" + deviceName + "-" + deviceId + "/set";
}

String MQTTTopics::getLightConfigTopic() const {
    return baseTopic + "/light/" + deviceName + "-" + deviceId + "/config";
}

String MQTTTopics::getFanStateTopic() const {
    return baseTopic + "/fan/" + deviceName + "-" + deviceId + "/state";
}

String MQTTTopics::getFanCommandTopic() const {
    return baseTopic + "/fan/" + deviceName + "-" + deviceId + "/set";
}

String MQTTTopics::getFanPercentStateTopic() const {
    return baseTopic + "/fan/" + deviceName + "-" + deviceId +
           "/speed/percentage_state";
}

String MQTTTopics::getFanPercentCommandTopic() const {
    return baseTopic + "/fan/" + deviceName + "-" + deviceId +
           "/speed/percentage";
}

String MQTTTopics::getFanModeStateTopic() const {
    return baseTopic + "/fan/" + deviceName + "-" + deviceId +
           "/preset/preset_mode_state";
}

String MQTTTopics::getFanModeCommandTopic() const {
    return baseTopic + "/fan/" + deviceName + "-" + deviceId + "/preset_mode";
}

String MQTTTopics::getFanConfigTopic() const {
    return baseTopic + "/fan/" + deviceName + "-" + deviceId + "/config";
}
