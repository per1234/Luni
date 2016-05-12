#ifndef TestManagerTemplates_h
#define TestManagerTemplates_h

template <typename T0, typename T1>
void TestManager::assertEquals(const char *msg, T0 expected, T1 actual) {
  if (expected == actual) return;
  testFailureCount++;
  logger->fatal(msg, expected, actual);
}

#endif
