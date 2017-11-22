

#ifndef CH_ETHZ_AH_BLEG_DEBUG_H_
#define CH_ETHZ_AH_BLEG_DEBUG_H_

#define DEBUG 1

#define DebugPrint(fmt, ...) \
            do { if (DEBUG){ FILE* log; log=fopen("clientlogfile.log", "a"); fprintf(log, fmt, __VA_ARGS__); fclose(log);}} while (0)

#endif
