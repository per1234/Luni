#include <FirmataDeviceLibrary.h>
#include <Framework/Logger.h>
#include <Framework/Tester.h>
#include <Framework/ByteOrder.h>

/**
   Test the operation of the Logger class.
*/

Tester *tst;
Logger *logger;

extern const char *LogLevel::levelName[COUNT];
extern const int LogLevel::levelValue[COUNT];

// --------------------------------------------------------

void setup() {

  // Countdown before running to give time to open the monitor window.

  Serial.begin(115200);

  delay(3000);
  for (int idx = 0; idx < 5; idx++) {
    Serial.print(5 - idx);
    Serial.print(" ");
    delay(1000);
  }
  Serial.println();

  //  Create the objects needed for the testing framework.

  logger = new Logger("ToolTestLogger");
  logger->setCurrentLogLevel(LogLevel::INFO);

  tst = new Tester();

// Run the test group once

  for (int logLevelIndex = 0; logLevelIndex < LogLevel::COUNT; logLevelIndex++) {

    tst->beforeGroup(LogLevel::levelName[logLevelIndex]);
    logger->setCurrentLogLevel(LogLevel::levelValue[logLevelIndex]);

      // --------------------------------------------------------

      tst->beforeTest("LevelCheck");

      logger->trace("trace level");
      logger->debug("debug level");
      logger->info("info level");
      logger->warn("warn level");
      logger->error("error level");
      logger->fatal("fatal level");

      tst->afterTest();

      // --------------------------------------------------------

      tst->beforeTest("VariadicArguments");

      logger->trace("trace level", 0);
      logger->debug("debug level", 0, 1);
      logger->info("info level", 0, 1, 2);
      logger->warn("warn level", 0, 1, 2, 3);
      logger->error("error level", 0, 1, 2, 3, 4);
      logger->fatal("fatal level", 0, 1, 2, 3, 4, 5);

      tst->afterTest();

      // --------------------------------------------------------

      tst->afterGroup();

      // --------------------------------------------------------

    }
}

void loop(){}

