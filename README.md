# Batch Process Monitor — FCFS and Round Robin Dispatcher

## 1. Overview
This project implements a simplified Batch Process Monitor similar to an operating system scheduler. It demonstrates the scheduling of batch processes using two mechanisms:

1. FCFS (First-Come-First-Served) for admitting processes into the system based on arrival time.
2. Round Robin for CPU execution using a fixed time quantum.

It uses process control blocks (PCBs) for representing process attributes and manages queues to schedule and run processes.

---

## 2. Project Files

```
dispatcher.c        -> Main scheduler implementation
sleepy.c            -> Simulated CPU-bound process
dispatchlist.txt    -> Input list of processes
README.md
```

---

## 3. Input Format

The input file `dispatchlist.txt` contains one process per line using the following format:

```
<arrival>,<priority>,<cpu time>,<memory>,<res1>,<res2>,<res3>,<res4>
```

Example:

```
0,0,3,64,0,0,0,0
2,0,6,64,0,0,0,0
4,0,4,64,0,0,0,0
```

Processes must be ordered by arrival time.

---

## 4. Compilation Instructions

Assuming the GCC compiler is available on the system:

```
gcc -Wall -g sleepy.c -o sleepy
gcc -Wall -g dispatcher.c -o dispatcher
```

Make both binaries executable if necessary:

```
chmod +x sleepy dispatcher
```

---

## 5. Running the Project

Place `sleepy`, `dispatcher`, and `dispatchlist.txt` in the same directory.

Run the dispatcher:

```
./dispatcher
```

During execution:
- The dispatcher reads processes from `dispatchlist.txt`
- Admits them based on arrival time (FCFS)
- Schedules execution using Round Robin
- Uses one-second time ticks to simulate CPU time

Output logs will show admission, execution, quantum expiration, and termination events.

---

## 6. System Calls Used

The project uses the following Unix system calls:

| Call | Purpose |
|------|---------|
| fork() | Create child process |
| execvp() | Load and execute a program |
| kill() | Send signals (terminate, stop, continue) |
| waitpid() | Reap terminated child processes |
| sleep() | Simulate system time ticks |

Signals used:
- SIGSTOP for preemption
- SIGCONT for resuming
- SIGINT for termination

---

## 7. Scheduling Behavior

### FCFS Admission
A process is admitted to the ready queue only when:
```
arrival_time <= simulation_time
```

### Round Robin Execution
- A fixed quantum (default = 2 seconds) is used
- A running process is preempted after quantum expiry
- Completed processes are terminated

This simulates time-sharing as implemented by modern operating systems.

---

## 8. Pure FCFS Mode (Optional)

This project can also be run as pure FCFS (non-preemptive) by removing:

- The time quantum logic
- SIGSTOP and SIGCONT signals

In pure FCFS mode:
- Each process runs to completion before the next process begins
- No preemption occurs

Example execution order:

```
P1 → P2 → P3
```

---

## 9. Testing Suggestions

Recommended test cases:
- All processes arrive at time 0
- Staggered arrival times
- Large CPU time differences
- Vary time quantum values
- Pure FCFS behavior comparison

---

## 10. Platform Requirements

This project requires:
- Linux or Unix-like OS
- GCC compiler
- POSIX-compliant process handling

---

## 11. Educational Purpose

The project demonstrates:
- Basic scheduling algorithms
- Time-sharing concepts
- Process lifecycle management
- PCB usage
- Context switching
- Unix process control via system calls

---

## 12. References

Operating System Concepts (Silberschatz, Galvin, Gagne)  
Linux manual pages: fork, exec, waitpid, kill, signal  

