
#ifndef CH_ETHZ_AH_BLEG_DAEMON_H_
#define CH_ETHZ_AH_BLEG_DAEMON_H_


#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <syslog.h>

#define LOGFILE_PATH "BLE_Daemon.log"
#define MAX_DEVICE_NR 20
#define MSG "exit"
#define MSG_SIZE 5


#define DEBUG 1

#define DebugPrint(fmt, ...) \
            do { if (DEBUG){ FILE* log; log=fopen(LOGFILE_PATH, "a"); fprintf(log, fmt, __VA_ARGS__); fclose(log);}} while (0)



typedef struct Map {
    char* key;
    pid_t pid;
    int used;
}NodeToPid;





#endif /* CH_ETHZ_AH_BLEDG_CLIENT_H_ */
