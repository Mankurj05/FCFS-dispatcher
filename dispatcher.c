// dispatcher.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAXARGS 4
#define TIME_QUANTUM 2   


typedef struct pcb {
    pid_t pid;                      // System process ID
    char *args[MAXARGS];            // Program name and arguments
    int arrivaltime;                // Arrival time of process
    int remainingcputime;           // Remaining CPU time
    int priority;                   // Not used in FCFS/RR basic version
    struct pcb *next;               // Linked list pointer
} Pcb;

typedef Pcb* PcbPtr;


void enqueue(PcbPtr *head, PcbPtr process) {
    process->next = NULL;
    if (*head == NULL) {
        *head = process;
    } else {
        PcbPtr temp = *head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = process;
    }
}

PcbPtr dequeue(PcbPtr *head) {
    if (*head == NULL)
        return NULL;
    PcbPtr temp = *head;
    *head = (*head)->next;
    temp->next = NULL;
    return temp;
}


void start_process(PcbPtr process) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {

        execvp(process->args[0], process->args);
        perror("execvp failed");
        exit(1);
    } else {

        process->pid = pid;
        printf("Time: starting process PID=%d, CPU=%d, AT=%d\n",
               pid, process->remainingcputime, process->arrivaltime);
        fflush(stdout);
    }
}

void terminate_process(PcbPtr *process) {
    if (*process == NULL)
        return;

    printf("Time: terminating PID=%d\n", (*process)->pid);
    fflush(stdout);


    kill((*process)->pid, SIGINT);


    waitpid((*process)->pid, NULL, 0);


    if ((*process)->args[1])
        free((*process)->args[1]);

    free(*process);
    *process = NULL;
}


PcbPtr load_dispatch(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening dispatch list file");
        exit(1);
    }

    PcbPtr head = NULL;
    char line[256];

    while (fgets(line, sizeof(line), file)) {

        if (line[0] == '\n' || line[0] == '#')
            continue;

        int arrival, priority, cputime;
        int mem, r1, r2, r3, r4;

        int fields = sscanf(
            line,
            " %d , %d , %d , %d , %d , %d , %d , %d",
            &arrival, &priority, &cputime, &mem,
            &r1, &r2, &r3, &r4
        );

        if (fields != 8) {
            fprintf(stderr, "Invalid line in dispatch file: %s", line);
            continue;
        }

        PcbPtr newPcb = (PcbPtr)malloc(sizeof(Pcb));
        if (!newPcb) {
            perror("malloc failed");
            exit(1);
        }

        newPcb->arrivaltime = arrival;
        newPcb->priority = priority;
        newPcb->remainingcputime = cputime;
        newPcb->pid = 0;
        newPcb->next = NULL;


        newPcb->args[0] = "./sleepy";

        char *arg_time = (char *)malloc(16);
        if (!arg_time) {
            perror("malloc failed");
            exit(1);
        }
        sprintf(arg_time, "%d", cputime);

        newPcb->args[1] = arg_time;
        newPcb->args[2] = NULL;
        newPcb->args[3] = NULL;


        enqueue(&head, newPcb);
    }

    fclose(file);
    return head;
}


int main() {
    PcbPtr dispatchQueue = load_dispatch("dispatchlist.txt"); 
    PcbPtr readyQueue = NULL;                                 
    PcbPtr currentProcess = NULL;

    int timer = 0;
    int quantumCounter = 0;

    printf("=== Batch Process Monitor: FCFS + Round Robin (Q=%d) ===\n",
           TIME_QUANTUM);


    while (dispatchQueue != NULL || readyQueue != NULL || currentProcess != NULL) {


        while (dispatchQueue != NULL &&
               dispatchQueue->arrivaltime <= timer) {
            PcbPtr p = dequeue(&dispatchQueue);
            printf("Time %d: Process admitted (AT=%d, CPU=%d)\n",
                   timer, p->arrivaltime, p->remainingcputime);
            enqueue(&readyQueue, p);
        }


        if (currentProcess == NULL && readyQueue != NULL) {
            currentProcess = dequeue(&readyQueue);

            if (currentProcess->pid == 0) {

                start_process(currentProcess);
            } else {

                printf("Time %d: Resuming PID=%d\n", timer, currentProcess->pid);
                fflush(stdout);
                kill(currentProcess->pid, SIGCONT);
            }

            quantumCounter = 0;
        }


        sleep(1);
        timer++;


        if (currentProcess != NULL) {
            currentProcess->remainingcputime--;
            quantumCounter++;

            printf("Time %d: PID=%d running, remaining=%d, quantumUsed=%d\n",
                   timer,
                   currentProcess->pid,
                   currentProcess->remainingcputime,
                   quantumCounter);
            fflush(stdout);


            if (currentProcess->remainingcputime <= 0) {
                terminate_process(&currentProcess);

            }

            else if (quantumCounter >= TIME_QUANTUM) {
                printf("Time %d: Quantum expired for PID=%d, preempting.\n",
                       timer, currentProcess->pid);
                fflush(stdout);


                kill(currentProcess->pid, SIGSTOP);


                enqueue(&readyQueue, currentProcess);
                currentProcess = NULL;
            }
        }
    }

    printf("=== All processes completed at time %d ===\n", timer);
    return 0;
}

