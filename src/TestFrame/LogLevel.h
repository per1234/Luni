#ifndef LogLevel_h
#define LogLevel_h

#include <limits.h>

namespace LogLevel {
  const int OFF = 0;
  const int FATAL = 100;
  const int ERROR = 200;
  const int WARN = 300;
  const int INFO = 400;
  const int DEBUG = 500;
  const int TRACE = 600;
  const int ALL = INT_MAX;

  const int COUNT = 8;

  extern const char *levelName[COUNT];
  extern const int levelValue[COUNT];
}

#endif
