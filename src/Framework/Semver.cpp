#include "SemVer.h"

// SemVer::toCharString(uint8_t buf) {
//   char *result = new char[ ?? ];
//   int bufIndex = 0;
//   int resultIndex = 0;

//   int packetSize = buf[bufIndex++];
//   for (int idx = 0; idx < min(6, packetSize); idx++) {
//     switch (idx) {
//     case 0: break;
//     case 1: result[resultIndex++] = '.'; break;
//     case 2: result[resultIndex++] = '.'; break;
//     case 3: result[resultIndex++] = '-'; break;
//     case 4: result[resultIndex++] = '.'; break;
//     case 5: result[resultIndex++] = '.'; break;
//     default: result[resultIndex] = '?'; break;
//     }
//     result[resultIndex++] = buf[bufIndex++];
//   }
//   result[resultIndex++] = ' ';
//   strlcpy(result[resultIndex], reinterpret_cast<char *>&buf[bufIndex], result, bufsize - resultIndex);

//   return result;

// }
