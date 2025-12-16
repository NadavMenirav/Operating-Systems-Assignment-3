# 🖥️ OS Process Scheduler & Signal Handler

## 📋 Project Overview

This project implements two distinct operating system concepts in C:

1. **🎯 Focus Mode** - A signal-based distraction blocker that demonstrates signal handling and masking
2. **⚙️ CPU Scheduler** - A process scheduling simulator implementing multiple scheduling algorithms

---

## 🏗️ Project Structure

```
📦 Project Root
├── 📄 ex3.c                  # Main program entry point
├── 📄 Focus-Mode.c           # Focus mode implementation
├── 📄 CPU-Scheduler.c        # CPU scheduler implementation
├── 📊 processes1.csv         # Test data for scheduler
├── 📊 processes2.csv         # Test data for scheduler
├── 📊 processes3.csv         # Test data for scheduler
└── 📊 processes4.csv         # Test data for scheduler
```

---

## 🎯 Part 1: Focus Mode

### 🔍 What It Does

Focus Mode simulates a productivity tool that **blocks distractions** using Unix signals. During focus rounds, incoming signals (representing distractions) are blocked and queued, then processed only after the focus round ends.

### 🛠️ Signal Mapping

| Signal | Represents | User Input |
|--------|-----------|------------|
| `SIGUSR1` | 📧 Email notification | `1` |
| `SIGUSR2` | 📦 Delivery reminder | `2` |
| `SIGINT` | 🔔 Doorbell ringing | `3` |

### ▶️ Usage

```bash
./ex3 Focus-Mode <num-of-rounds> <round-duration>
```

**Example:**
```bash
./ex3 Focus-Mode 2 3
```

This runs 2 focus rounds, each allowing 3 distraction simulations.

### 🎮 How It Works

1. **Blocks** signals (`SIGUSR1`, `SIGUSR2`, `SIGINT`) at the start of each round
2. User simulates distractions during the round by pressing `1`, `2`, `3`, or `q` to quit
3. Signals are queued in the pending signal set
4. At the end of the round, signals are **unblocked** and handled
5. Displays which distractions occurred and their outcomes

### 🔑 Key Functions

- `blockSignals()` - Blocks the three distraction signals
- `unblockSignals()` - Unblocks signals after focus round
- `sendSig()` - Raises the appropriate signal based on user input
- `findInPending()` - Checks which new signals are pending
- `printMessages()` - Displays all distractions that occurred

---

## ⚙️ Part 2: CPU Scheduler

### 🔍 What It Does

Implements and simulates **four CPU scheduling algorithms**:

1. **FCFS** (First-Come-First-Served) 🥇
2. **SJF** (Shortest Job First) ⚡
3. **Priority Scheduling** 🎖️
4. **Round Robin** 🔄

### 📊 Input Format

Processes are defined in CSV files with the following format:

```csv
Name,Description,ArrivalTime,BurstTime,Priority
```

**Example:**
```csv
P1,Web Browser,0,5,2
P2,Text Editor,1,3,1
P3,Music Player,2,8,3
```

### ▶️ Usage

```bash
./ex3 CPU-Scheduler <processes.csv> <time-quantum>
```

**Example:**
```bash
./ex3 CPU-Scheduler processes1.csv 2
```

The time quantum is used for the Round Robin algorithm.

### 📈 Scheduling Algorithms

#### 🥇 FCFS (First-Come-First-Served)
- Processes are executed in order of arrival
- Non-preemptive
- Reports: **Average Waiting Time**

#### ⚡ SJF (Shortest Job First)
- Selects process with shortest burst time
- Non-preemptive
- Reports: **Average Waiting Time**

#### 🎖️ Priority Scheduling
- Executes processes based on priority value (lower = higher priority)
- Non-preemptive
- Reports: **Average Waiting Time**

#### 🔄 Round Robin
- Each process gets a fixed time quantum
- Preemptive - processes are cycled through
- Reports: **Total Turnaround Time**

### 🔑 Key Data Structures

**Process:**
```c
typedef struct {
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    int arrivalTime;
    int burstTime;
    int priority;
} Process;
```

**ReadyQueue:**
- Priority queue that can be sorted based on algorithm requirements
- Supports insertion and removal operations
- Uses comparison functions for different scheduling policies

### 🔑 Key Functions

- `initializeProcessesFromCSV()` - Parses CSV file into process array
- `printScheduler()` - Main simulation loop for each algorithm
- `insertNewProcesses()` - Adds arriving processes to ready queue
- `insertQ() / removeQ()` - Queue operations with priority sorting

---

## 🔧 Compilation & Execution

### Compile:
```bash
gcc -o ex3 ex3.c -lrt
```

The `-lrt` flag links the real-time library needed for `clock_gettime()`.

### Run Focus Mode:
```bash
./ex3 Focus-Mode 3 5
```

### Run CPU Scheduler:
```bash
./ex3 CPU-Scheduler processes1.csv 2
```

---

## 📝 Sample Output

### Focus Mode Example:
```
Entering Focus Mode. All distractions are blocked.
══════════════════════════════════════════════
                Focus Round 1                
──────────────────────────────────────────────

Simulate a distraction:
  1 = Email notification
  2 = Reminder to pick up delivery
  3 = Doorbell Ringing
  q = Quit
>> 1

Simulate a distraction:
...
──────────────────────────────────────────────
        Checking pending distractions...      
──────────────────────────────────────────────
 - Email notification is waiting.
[Outcome:] The TA announced: Everyone get 100 on the exercise!
──────────────────────────────────────────────
             Back to Focus Mode.              
══════════════════════════════════════════════
```

### CPU Scheduler Example:
```
══════════════════════════════════════════════
>> Scheduler Mode : FCFS
>> Engine Status  : Initialized
──────────────────────────────────────────────

0 → 5: P1 Running Web Browser.
5 → 8: P2 Running Text Editor.
8 → 16: P3 Running Music Player.

──────────────────────────────────────────────
>> Engine Status  : Completed
>> Summary        :
   └─ Average Waiting Time : 4.33 time units
>> End of Report
══════════════════════════════════════════════
```

---

## 🎓 Learning Objectives

### Signals & Signal Handling 📡
- Understanding signal blocking and unblocking with `sigprocmask()`
- Working with signal sets (`sigset_t`)
- Handling pending signals with `sigpending()`
- Custom signal handlers with `sigaction()`

### Process Scheduling 📊
- Implementing classical scheduling algorithms
- Managing ready queues and process states
- Calculating performance metrics (waiting time, turnaround time)
- Preemptive vs non-preemptive scheduling

### Systems Programming 💻
- File I/O and CSV parsing
- Real-time clock usage (`clock_gettime()`)
- Signal-based timing with `ualarm()`
- Dynamic data structure management

---

## ⚠️ Important Notes

- Focus Mode uses `SIGINT`, so pressing `Ctrl+C` will be captured during focus rounds
- The CPU Scheduler uses real-time delays to simulate process execution
- Round Robin requires a time quantum parameter
- All CSV files must follow the exact format specified

---

## 👨‍💻 Technical Details

**Language:** C  
**Standards:** POSIX signals, C99  
**Key Libraries:** `<signal.h>`, `<time.h>`, `<stdio.h>`, `<stdlib.h>`  
**Compilation:** Requires `-lrt` for linking real-time library

---

Made with ⚡ for Operating Systems Course
