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

#define SCHEDULER_OUTRO_WITH_WAITING "\n──────────────────────────────────────────────\n"\
">> Engine Status  : Completed\n"\
">> Summary        :\n"\
"   └─ Average Waiting Time : %.2f time units\n"\
">> End of Report\n"\
"══════════════════════════════════════════════\n\n"

#define SCHEDULER_OUTRO_WITH_TURNAROUND "\n──────────────────────────────────────────────\n"\
">> Engine Status  : Completed\n"\
">> Summary        :\n"\
"   └─ Total Turnaround Time : %d time units\n\n"\
">> End of Report\n"\
"══════════════════════════════════════════════\n\n"

#define SCHEDULE_LINE "%d → %d: %s Running %s.\n"
#define CPU_IDLE "%d → %d: Idle.\n"



#define FCFS "FCFS"
#define RR "Round Robin"
#define SJF "SJF"
#define PRIORITY "Priority"


typedef struct {
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

typedef struct {
    char* name;
    bool shouldPrintAverageWaitingTime;
    bool shouldPrintTotalTurnaroundTime;
    int (*comparePriority)(Process, Process);
    int maxCPUTime; // Used for RR
} Algorithm;



// Main control
void HandleCPUScheduler(const char* processesCsvFilePath, int timeQuantum);

// Process output
void printScheduler(Algorithm algorithm, Process processes[], int processesCount);

// Process input
void initializeProcessesFromCSV(const char* path, Process outputProcs[], int* outputProcsCount);
Process parseProcess(const char* line);

// Time and arrival
double getTimeElapsed(struct timespec start);
void insertNewProcesses(ReadyQueue* queue, Process processes[], int* startingIDX, int processesCount, struct timespec start, double timeElapsed);

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
int SJFCompare(Process a, Process b);
int PriorityCompare(Process a, Process b);

// Signal handler
void dumby();




void HandleCPUScheduler(const char* processesCsvFilePath, int timeQuantum)
{
    int procsCount = 0;
    Process procs[MAX_PROCESSES];


    //First-Come-First-Served
    Algorithm firstComeFirstServed = { 0 };
    firstComeFirstServed.name = "FCFS";
    firstComeFirstServed.comparePriority = dummyComparePriority;
    firstComeFirstServed.shouldPrintAverageWaitingTime = true;
    firstComeFirstServed.shouldPrintTotalTurnaroundTime = false;
    firstComeFirstServed.maxCPUTime = -1;

    //Shortest-Job-First
    Algorithm shortestJobFirst = { 0 };
    shortestJobFirst.name = "SJF";
    shortestJobFirst.comparePriority = SJFCompare;
    shortestJobFirst.shouldPrintAverageWaitingTime = true;
    shortestJobFirst.shouldPrintTotalTurnaroundTime = false;
    shortestJobFirst.maxCPUTime = -1;

    //Priority
    Algorithm priority = { 0 };
    priority.name = "Priority";
    priority.comparePriority = PriorityCompare;
    priority.shouldPrintAverageWaitingTime = true;
    priority.shouldPrintTotalTurnaroundTime = false;
    priority.maxCPUTime = -1;

    //Round-Robin
    Algorithm roundRobin = { 0 };
    roundRobin.name = "Round Robin";
    roundRobin.comparePriority = dummyComparePriority;
    roundRobin.shouldPrintAverageWaitingTime = false;
    roundRobin.shouldPrintTotalTurnaroundTime = true;
    roundRobin.maxCPUTime = timeQuantum;

    /*
     * GET PROCS FROM FILE
     */
    initializeProcessesFromCSV(processesCsvFilePath, procs, &procsCount);
    sortProcesses(procs, procsCount, compareArrivalTime);

    // printScheduler(firstComeFirstServed, procs, procsCount);
    // printScheduler(shortestJobFirst, procs, procsCount);
    // printScheduler(priority, procs, procsCount);
    printScheduler(roundRobin, procs, procsCount);

}

void initializeProcessesFromCSV(const char* path, Process outputProcs[], int* outputProcsCount)
{
    FILE* file = NULL;

    if ((file = fopen(path, "r")) == NULL)
    {
        perror("fopen() error:");
        exit(EXIT_FAILURE);
    }


    char* line = NULL;
    size_t len = MAX_LINE_LENGTH;
        while ((getline(&line, &len, file)) != -1) {
        const Process process = parseProcess(line);
        outputProcs[*outputProcsCount] = process;
        (*outputProcsCount)++;
    }

    if (ferror(file)) {
        perror("getline() error:");
        free(line);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    free(line);
    fclose(file);
}

Process parseProcess(const char* line)
{
    char* save_ptr = NULL;
    char* dup = strdup(line);
    const char* currentValue = NULL;
    Process processes = { 0 };


    /*
     * GETTING NAME
     */
    if ((currentValue = strtok_r(dup, CSV_DELIMITERS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    strcpy(processes.name, currentValue);


    /*
     * GETTING description
     */
    if ((currentValue = strtok_r(NULL, CSV_DELIMITERS, &save_ptr)) == NULL)
    {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }
    strcpy(processes.description, currentValue);

    if ((currentValue = strtok_r(NULL, CSV_DELIMITERS, &save_ptr)) == NULL) {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }

    processes.arrivalTime = atoi(currentValue);

    if ((currentValue = strtok_r(NULL, CSV_DELIMITERS, &save_ptr)) == NULL) {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }

    processes.burstTime = atoi(currentValue);

    if ((currentValue = strtok_r(NULL, CSV_DELIMITERS, &save_ptr)) == NULL) {
        perror("strtok_r() error");
        exit(EXIT_FAILURE);
    }

    processes.priority = atoi(currentValue);


    free(dup);
    return processes;
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

int compareArrivalTime(const Process a, const Process b) {
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

    const Process nullProcess = { 0 };
    queue->processes[queue->size - 1] = nullProcess;

    queue->size = queue->size - 1;
    return firstInstance;
}

void insertQ(ReadyQueue* queue, const Process process) {
    if (queue->size < MAX_PROCESSES) {
        printf("inserting process %s\n", process.name);
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

int SJFCompare(const Process a, const Process b) {
    // Shortest-Job-First
    return a.burstTime - b.burstTime;
}

int PriorityCompare(const Process a, const Process b) {
    return a.priority - b.priority;
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

void insertNewProcesses(ReadyQueue* queue, Process processes[], int* startingIDX, const int processesCount, const struct timespec start, double timeElapsed) {
    // at a given time, adds the new arriving processes
    // const double currentTime = getTimeElapsed(start);

    if ((*startingIDX) >= processesCount) {
        fprintf(stderr, "Invalid arguments\n");
        exit(EXIT_FAILURE);
    }


    while ((*startingIDX < processesCount) && (processes[*startingIDX].arrivalTime <=  timeElapsed)) {
        //currentTime = getTimeElapsed(start);
        //printf("Insert process no. %d\n", *startingIDX + 1);
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


void printScheduler(const Algorithm algorithm, Process processes[], const int processesCount) {
    int startingIDX = 0;
    int turnaroundTime = 0;
    int totalWaitingTime = 0;
    int waitingTime = 0;
    int processStartSeconds = 0;
    int idleStartSeconds = 0;
    int idleEndSeconds = 0;
    int timeElapsed = 0;
    int processEndSeconds = 0;
    bool isProcessRunning = false;
    bool isIdle = false;

    struct timespec processStart;
    ReadyQueue queue = createReadyQueue(algorithm.comparePriority); //FCFS
    Process currentProcess = { 0 };

    struct sigaction sa;
    struct timespec start;

    sa.sa_handler = dumby;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);

    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        perror("clock_gettime error");
        exit(EXIT_FAILURE);
    }


    printf(SCHEDULER_INTRO, algorithm.name);
    while (startingIDX < processesCount || !isEmpty(&queue) || isProcessRunning) {
        // every ---- seconds we run this.

        processEndSeconds = (int) getTimeElapsed(start); // how long the scheduler is running

        if (startingIDX < processesCount && !isProcessRunning) {
            insertNewProcesses(&queue, processes, &startingIDX, processesCount, start, processEndSeconds);
        }

        if (isProcessRunning) {
            timeElapsed = (int)getTimeElapsed(processStart);

            if (timeElapsed >= currentProcess.burstTime) {
                isProcessRunning = false;



                printf(
                    SCHEDULE_LINE
                    ,processStartSeconds
                    ,processEndSeconds
                    ,currentProcess.name
                    ,currentProcess.description
                );

                if (isEmpty(&queue) && startingIDX < processesCount) {
                    isIdle = true;
                    idleStartSeconds = (int)getTimeElapsed(start);
                }

                waitingTime = processEndSeconds - currentProcess.arrivalTime - currentProcess.burstTime;
                totalWaitingTime += waitingTime;

                if (isEmpty(&queue) && startingIDX >= processesCount) {
                    //finished
                    turnaroundTime = (int)getTimeElapsed(start);
                    break;
                }

            }

            else if (algorithm.maxCPUTime != -1) {
                if (timeElapsed >= algorithm.maxCPUTime) {
                    // If he finished his time Quantum

                    waitingTime = processEndSeconds - currentProcess.arrivalTime - algorithm.maxCPUTime;
                    totalWaitingTime += waitingTime;
                    isProcessRunning = false;



                    printf(
                        SCHEDULE_LINE
                        ,processEndSeconds - algorithm.maxCPUTime
                        ,processEndSeconds
                        ,currentProcess.name
                        ,currentProcess.description
                    );

                    // entering to end of the queue
                    Process modifiedProcess = currentProcess;
                    modifiedProcess.arrivalTime = processEndSeconds;
                    modifiedProcess.burstTime -= algorithm.maxCPUTime;
                    insertQ(&queue, modifiedProcess);

                    if (startingIDX < processesCount) {
                        insertNewProcesses(&queue, processes, &startingIDX, processesCount, start, processEndSeconds);
                    }
                }

            }
        }
        if (!isProcessRunning && !isEmpty(&queue)) {
            //process starting to run
            if (isIdle) {
                // print
                idleEndSeconds = (int)getTimeElapsed(start);
                if (idleEndSeconds > 0)
                    printf(CPU_IDLE, idleStartSeconds, idleEndSeconds);
                idleStartSeconds = 0;
            }
            isIdle = false;
            isProcessRunning = true;
            if (clock_gettime(CLOCK_MONOTONIC, &processStart) != 0) {
                perror("clock_gettime error");
                exit(EXIT_FAILURE);
            }

            processStartSeconds = (int)getTimeElapsed(start);


            printf("The head of queue: %s\n", queue.processes[0].name);
            currentProcess = removeQ(&queue);
        }

        if (!isProcessRunning && isEmpty(&queue) && startingIDX < processesCount) {
            // Idle
            isIdle = true;
        }

        if (startingIDX < processesCount) {
            insertNewProcesses(&queue, processes, &startingIDX, processesCount, start, processEndSeconds);
        }
        ualarm((int)1e5, 0);
    }

    if (algorithm.shouldPrintAverageWaitingTime) {
        double averageWaitingTime = 0;
        averageWaitingTime = (double)totalWaitingTime / processesCount;
        printf(SCHEDULER_OUTRO_WITH_WAITING, averageWaitingTime);
    }
    if (algorithm.shouldPrintTotalTurnaroundTime) {
        printf(SCHEDULER_OUTRO_WITH_TURNAROUND, turnaroundTime);
    }


}

    void restoreSignalsToDefault() {
        struct sigaction sa;
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        sigaction(SIGALRM, &sa, NULL);

    }
