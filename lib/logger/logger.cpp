#include "logger.h"

void LOG(String logMsg, LogLevel logLevel, bool addNewLine) {
    // Add your log logic here
    if (logLevel <= LOG_DEBUG) {
        if (addNewLine) {
            Serial.println(logMsg.c_str());
        } else {
            Serial.printf(logMsg.c_str());
        }
    }
}
