#ifndef Logger_h
#define Logger_h

#include <arduino.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "LogLevel.h"

class Logger {
public:
  Logger(const char *name);
  ~Logger();

  template <typename... ItemType>
  void fatal(const char *msg, ItemType ...valPack);

  template <typename... ItemType>
  void error(const char *msg, ItemType ...valPack);

  template <typename... ItemType>
  void warn(const char *msg, ItemType ...valPack);

  template <typename... ItemType>
  void info(const char *msg, ItemType ...valPack);

  template <typename... ItemType>
  void debug(const char *msg, ItemType ...valPack);

  template <typename... ItemType>
  void trace(const char *msg, ItemType ...valPack);

  const char *getLoggerName();
  int getCurrentLogLevel();
  void setCurrentLogLevel(int newLevel);
  bool isEnabled(int lvl);

private:
  const char *loggerName;
  int currentLogLevel;


template <typename... ItemType>
void recordLogEvent(int theLvl, const char *theMsg, ItemType ...valPack);

template <typename First, typename... Rest>
void valuePrinter(First valFirst, Rest ...valPack);

template <typename Only>
void valuePrinter(Only value);

void valuePrinter();


};

#include "LoggerTemplates.h"

#endif
