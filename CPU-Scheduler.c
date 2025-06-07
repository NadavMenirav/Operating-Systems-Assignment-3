#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NAME 51
#define MAX_DESC 101
#define MAX_PROC 1000
#define MAX_LINE 257

#define CSV_DELIMS ","

typedef struct
{
    char name[MAX_NAME];
    char desc[MAX_DESC];
    int arrival_time;
    int burst_time;
    int priority;
} Process;

typedef struct {
    Process processes[MAX_PROC];
    int processesCount;
    int (*compare)(Process, Process);
} readyQueue;

void InitProcessesFromCSV(const char* path, Process oprocs[], int* oprocsCount);
Process ParseProcess(const char* line);
void HandleCPUScheduler(const char* processesCsvFilePath, int timeQuantum);
void sortProcesses(Process* processes, const int* processesCount, int(*compare)(Process, Process));
int compareArrivalTime(Process a, Process b);


void HandleCPUScheduler(const char* processesCsvFilePath, int timeQuantum)
{
    int procsCount = 0;
    Process procs[MAX_PROC];


    /*
     * GET PROCS FROM FILE
     */
    InitProcessesFromCSV(processesCsvFilePath, procs, &procsCount);
}

void InitProcessesFromCSV(const char* path, Process oprocs[], int* oprocsCount)
{
    FILE* file = NULL;

    if ((file = fopen(path, "r")) == NULL)
    {
        perror("fopen() error:");
        exit(EXIT_FAILURE);
    }



    /*
     * GETTING PROC INFORMATION
     */
    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, file) != NULL)
    {
        Process process = ParseProcess(line);
        oprocs[*oprocsCount] = process;
        (*oprocsCount)++;
    }
    if (ferror(file))
    {
        perror("fgets() error:");
        exit(EXIT_FAILURE);
    }
}

Process ParseProcess(const char* line)
{
    char* save_ptr = NULL;
    char* dup = strdup(line);
    char* currentValue = NULL;
    Process proc = { 0 };


    /*
     * GETTING NAME
     */
    if ((currentValue = strtok_r(dup, CSV_DELIMS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    strcpy(proc.name, currentValue);


    /*
     * GETTING DESC
     */
    if ((currentValue = strtok_r(NULL, CSV_DELIMS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    strcpy(proc.desc, currentValue);



    /*
     * GETTING ARRIVAL TIME
     */
    if ((currentValue = strtok_r(NULL, CSV_DELIMS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    proc.arrival_time = atoi(currentValue);



    /*
     * GETTING BURST TIME
     */
    if ((currentValue = strtok_r(NULL, CSV_DELIMS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    proc.burst_time = atoi(currentValue);



    /*
     * GETTING PRIORITY
     */
    if ((currentValue = strtok_r(NULL, CSV_DELIMS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    proc.priority = atoi(currentValue);



    free(dup);



    return proc;
}

void sortProcesses(Process* processes, const int* processesCount, int(*compare)(Process, Process)) {
    int compResult = 0;
    bool isSwap = false;
    Process current, next;


    do {
        isSwap = false;
        for (int i = 0; i < *processesCount - 1; i++) {
            current = processes[i];
            next = processes[i + 1];
            compResult = compare(current, next);

            if (compResult > 0) {
                processes[i] = next;
                processes[i + 1] = current;
                isSwap = true;
            }
        }
    } while (isSwap);
}

int compareArrivalTime(Process a, Process b) {
    return b.arrival_time - a.arrival_time;
}