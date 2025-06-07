#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define MAX_NAME_LENGTH 51
#define MAX_DESCRIPTION_LENGTH 101
#define MAX_PROCESSES 1000
#define MAX_LINE_LENGTH 257

#define CSV_DELIMITERS ","

#define SCHEDULER_INTRO "══════════════════════════════════════════════\n"\
">> Scheduler Mode : %s\n"\
">> Engine Status  : Initialized\n"\
"──────────────────────────────────────────────\n\n"

#define SCHEDULER_OUTRO "\n──────────────────────────────────────────────\n"\
">> Engine Status  : Completed\n"\
">> Summary        :\n"\
"   └─ Average Waiting Time : %.2f time units\n"\
">> End of Report\n"\
"══════════════════════════════════════════════\n\n"


#define FCFS "FCFS"


typedef struct
{
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    int arrivalTime;
    int burstTime;
    int priority;
} Process;

typedef struct {
    Process processes[MAX_PROCESSES];
    int size;
    int (*comparePriority)(Process, Process);
} ReadyQueue;




// Main control
void HandleCPUScheduler(const char* processesCsvFilePath, int timeQuantum);

// Process input
void InitProcessesFromCSV(const char* path, Process oprocs[], int* oprocsCount);
Process ParseProcess(const char* line);

// Time and arrival
double getTimeElapsed(struct timespec start);
void insertNewProcesses(ReadyQueue* queue, Process processes[], int* startingIDX, int processesCount, struct timespec start);

// Ready queue
ReadyQueue createReadyQueue(int (*comparePriority)(Process, Process));
bool isEmpty(const ReadyQueue* queue);
Process removeQ(ReadyQueue* queue);
void insertQ(ReadyQueue* queue, Process process); // or insert()

// Sorting and comparing
void sortProcesses(Process* processes, int processesCount, int(*compare)(Process, Process));
int compareArrivalTime(Process a, Process b);
int compareBurstTime(Process a, Process b);
int dummyComparePriority(Process a, Process b);

// Signal handler
void dumby();




void HandleCPUScheduler(const char* processesCsvFilePath, int timeQuantum)
{
    int procsCount = 0;
    int startingIDX = 0;
    int turnaroundTime = 0;
    int totalWaitingTime = 0;
    int waitingTime = 0;
    double averageWaitingTime = 0;
    bool isProcessRunning = false;
    bool isIdle = false;
    struct timespec start;
    struct timespec processStart;
    ReadyQueue queue = createReadyQueue(dummyComparePriority); //FCFS
    Process procs[MAX_PROCESSES];
    Process currentProcess = { 0 };
    struct sigaction sa;

    sa.sa_handler = dumby;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);


    /*
     * GET PROCS FROM FILE
     */
    InitProcessesFromCSV(processesCsvFilePath, procs, &procsCount);
    sortProcesses(procs, procsCount, compareArrivalTime);
    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("clock_gettime error");
        exit(EXIT_FAILURE);
    }

    printf(SCHEDULER_INTRO, FCFS);
    while (startingIDX < procsCount || isEmpty(&queue) || isProcessRunning) {
        // every ---- seconds we run this.

        if (startingIDX < procsCount) {
            insertNewProcesses(&queue, procs, &startingIDX, procsCount, start);
        }

        if (isProcessRunning) {
            const int timeElapsed = (int)getTimeElapsed(processStart);
            if (timeElapsed >= currentProcess.burstTime) {
                isProcessRunning = false;
                const int processEnd = (int) getTimeElapsed(start);
                waitingTime = processEnd - currentProcess.arrivalTime - currentProcess.burstTime;
                totalWaitingTime += waitingTime;
                if (isEmpty(&queue) && startingIDX >= procsCount) {
                    //finished
                    turnaroundTime = (int)getTimeElapsed(start);
                    break;
                }
            }
        }
        if (!isProcessRunning && !isEmpty(&queue)) {
            //process starting to run
            if (isIdle) {
                // print
            }
            isIdle = false;
            isProcessRunning = true;
            if (clock_gettime(CLOCK_MONOTONIC, &processStart) != 0) {
                perror("clock_gettime error");
                exit(EXIT_FAILURE);
            }
            currentProcess = removeQ(&queue);
        }

        if (!isProcessRunning && isEmpty(&queue) && startingIDX < procsCount) {
            // Idle
            isIdle = true;
        }
        ualarm((int)1e5, 0);
    }

    averageWaitingTime = (double)totalWaitingTime / procsCount;
    printf(SCHEDULER_OUTRO, averageWaitingTime);

    printf("%d", turnaroundTime);

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
    char line[MAX_LINE_LENGTH];
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
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
    if ((currentValue = strtok_r(dup, CSV_DELIMITERS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    strcpy(proc.name, currentValue);


    /*
     * GETTING description
     */
    if ((currentValue = strtok_r(NULL, CSV_DELIMITERS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    strcpy(proc.description, currentValue);



    /*
     * GETTING ARRIVAL TIME
     */
    if ((currentValue = strtok_r(NULL, CSV_DELIMITERS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    proc.arrivalTime = atoi(currentValue);



    /*
     * GETTING BURST TIME
     */
    if ((currentValue = strtok_r(NULL, CSV_DELIMITERS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    proc.burstTime = atoi(currentValue);



    /*
     * GETTING PRIORITY
     */
    if ((currentValue = strtok_r(NULL, CSV_DELIMITERS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    proc.priority = atoi(currentValue);



    free(dup);



    return proc;
}

void sortProcesses(Process* processes, const int processesCount, int(*compare)(Process, Process)) {
    int compResult = 0;
    bool isSwap = false;


    do {
        isSwap = false;
        for (int i = 0; i < processesCount - 1; i++) {
            const Process current = processes[i];
            const Process next = processes[i + 1];
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
    return a.arrivalTime - b.arrivalTime;
}

Process removeQ(ReadyQueue* queue) {
    const Process firstInstance = queue->processes[0];

    if (queue->size <= 0) {
        fprintf(stderr, "Empty queue\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < queue->size - 1; i++) {
        queue->processes[i] = queue->processes[i + 1];
    }

    queue->size = queue->size - 1;
    return firstInstance;
}

void insertQ(ReadyQueue* queue, const Process process) {
    if (queue->size < MAX_PROCESSES) {
        queue->processes[queue->size] = process;
        queue->size++;
        sortProcesses(queue->processes, queue->size, queue->comparePriority);
    }
}

int dummyComparePriority(Process a, Process b) {
    /*This function is used in algorithms such as FCFS where we wa nt the priority queue to
    *  be just a regular queue
    */
    return 0;
}

ReadyQueue createReadyQueue(int (*comparePriority)(Process, Process)) {
    //Creating an empty ReadyQueue
    ReadyQueue queue = { 0 };
    queue.size =  0;
    queue.comparePriority = comparePriority;
    return queue;
}

double getTimeElapsed(const struct timespec start) {
    // get length of our program
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    return (double)(now.tv_sec - start.tv_sec) + (double)(now.tv_nsec - start.tv_nsec) / 1e9;
}

void insertNewProcesses(ReadyQueue* queue, Process processes[], int* startingIDX, const int processesCount, const struct timespec start) {
    // at a given time, adds the new arriving processes
    const double currentTime = getTimeElapsed(start);

    if ((*startingIDX) >= processesCount) {
        fprintf(stderr, "Invalid arguments\n");
        exit(EXIT_FAILURE);
    }


    while ((*startingIDX < processesCount) && (processes[*startingIDX].arrivalTime <=  currentTime)) {
        //currentTime = getTimeElapsed(start);
        insertQ(queue, processes[*startingIDX]);
        (*startingIDX)++;
    }

}

bool isEmpty(const ReadyQueue* queue) {
    return queue->size == 0;
}

void dumby() {
    // i have noting
}