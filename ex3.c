#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Focus-Mode.c"
#include "CPU-Scheduler.c"


#define FOCUS_MODE_ARG_COUNT 3
#define CPU_SCHEDULER_ARG_COUNT 3
#define FOCUS_MODE "Focus-Mode"
#define CPU_Scheduler "CPU-Scheduler"
#define WRONG_ARG_COUNT_MESSAGE "Usage: %s <Focus-Mode/CPU-Schedule> <Num-Of-Rounds/Processes.csv> <Round-Duration/Time-Quantum>"
#define WRONG_ARG_COUNT_EXIT_STATUS 1

int main(const int argc, const char** argv) {
    if (argc < 2) {
        printf(WRONG_ARG_COUNT_MESSAGE, argv[0]);
        exit(WRONG_ARG_COUNT_EXIT_STATUS);
    }

    const char* mode = argv[1]; //The mode of the program
    if (strcmp(mode, FOCUS_MODE) == 0) {
        if (argc != FOCUS_MODE_ARG_COUNT + 1) { // +1 for name of file
            printf(WRONG_ARG_COUNT_MESSAGE, argv[0]);
            exit(WRONG_ARG_COUNT_EXIT_STATUS);
        }
        const int numOfRounds = atoi(argv[2]);
        const int duration = atoi(argv[3]);
        runFocusMode(numOfRounds, duration); //starting focus mode
        exit(EXIT_SUCCESS);
    }
    else if (strcmp(mode, CPU_Scheduler) == 0) {
        if (argc != CPU_SCHEDULER_ARG_COUNT + 1) { // +1 for file name
            printf(WRONG_ARG_COUNT_MESSAGE, argv[0]);
            exit(WRONG_ARG_COUNT_EXIT_STATUS);
        }
        const char* processesCsvFilePath = argv[2];
        int timeQuantum = atoi(argv[3]);

        HandleCPUScheduler(processesCsvFilePath, timeQuantum);
        exit(EXIT_SUCCESS);
    }
    return 0;
}