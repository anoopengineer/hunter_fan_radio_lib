#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>

#include "config.h"

enum LogLevel { LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG };

void LOG(String logMsg, LogLevel logLevel, bool addNewLine);

#endif  // LOGGER_H