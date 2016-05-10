#include "Logger.h"

Logger::Logger(const char *name) {
  loggerName = strdup(name);
  currentLogLevel = LogLevel::OFF;
}

Logger::~Logger() {
  if (loggerName != 0) {
    free((void*)loggerName);
  }
}

const char *Logger::getLoggerName(){
  return loggerName;
  }

int Logger::getCurrentLogLevel() {
  return currentLogLevel;
}

void Logger::setCurrentLogLevel(int newLevel) {
  currentLogLevel = abs(newLevel);
}

bool Logger::isEnabled(int lvl) {
  return (lvl <= currentLogLevel);
}

void Logger::valuePrinter() {}
