#include "LogLevel.h"

const char *LogLevel::levelName[LogLevel::COUNT] = {
  "OFF","FATAL","ERROR","WARN ",
  "INFO ","DEBUG","TRACE","ALL  "
};

const int LogLevel::levelValue[LogLevel::COUNT] = {
  0,100,200,300,400,500,600,INT_MAX
};
