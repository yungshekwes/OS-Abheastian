#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_SIZE 10

typedef struct {
    int items[MAX_SIZE];
    int front;
    int rear;
} Queue;

Queue* createQueue() {
    Queue* queue = (Queue*)malloc(sizeof(Queue));
    queue->front = -1;
    queue->rear = -1;
    return queue;
}

int isEmpty(Queue* queue) {
    return queue->rear == -1;
}

int isFull(Queue* queue) {
    return queue->rear == MAX_SIZE - 1;
}

void enqueue(Queue* queue, int item) {
    if (isFull(queue)) {
        printf("Queue is full\n");
        return;
    }
    if (queue->front == -1) {
        queue->front = 0;
    }
    queue->rear++;
    queue->items[queue->rear] = item;
}

int dequeue(Queue* queue) {
    int item;
    if (isEmpty(queue)) {
        printf("Queue is empty\n");
        return -1;
    }
    item = queue->items[queue->front];
    queue->front++;
    if (queue->front > queue->rear) {
        queue->front = queue->rear = -1;
    }
    return item;
}

void display(Queue* queue) {
    int i;
    if (isEmpty(queue)) {
        printf("Queue is empty\n");
    } else {
        printf("Queue elements are:\n");
        for (i = queue->front; i <= queue->rear; i++) {
            printf("%d ", queue->items[i]);
        }
        printf("\n");
    }
}

struct Jobs {
    int job;
    int ref;
};

struct Process {
    int pid;
    int *CPUJobs;
    int *CPUJobsRef;
    int *IOJobs;
    int *IOJobsRef;
    int IOJobIdx;
    int CPUJobIdx;
    int TotalJobIdx;
    int arrivalTime;
    int IOJobCount;
    int CPUJobCount;
    int TotalJobCount;
};

// TODO: Read input correctly, make queue, implement algo

int main() {
    int proIdx = 0;
    int ticker = 0;
    Queue* cpu = createQueue();
    Queue* cpuref = createQueue();
    Queue* io = createQueue();
    Queue* ioref = createQueue();
    struct Jobs curCpu;
    struct Jobs curIo;
    curCpu.job = -1;
    curCpu.ref = -1;
    curIo.job = -1;
    curIo.ref = -1;
    int globalCPU, globalIO;
    struct Process processes[100];
    
    while(scanf("%d", &processes[proIdx].arrivalTime) != EOF){
        processes[proIdx].IOJobIdx = 0;
        processes[proIdx].CPUJobIdx = 0;
        processes[proIdx].IOJobCount = 0;
        processes[proIdx].CPUJobCount = 0;
        processes[proIdx].TotalJobCount = 0;
        processes[proIdx].CPUJobs = (int *)malloc(1000 * sizeof(int));
        processes[proIdx].CPUJobsRef = (int *)malloc(1000 * sizeof(int));
        processes[proIdx].IOJobs = (int *)malloc(1000 * sizeof(int));
        processes[proIdx].IOJobsRef = (int *)malloc(1000 * sizeof(int));
        int arrival_time, cpuTemp, ioTemp;
        int remaining_cpuTemp = 0, remaining_ioTemp = 0;
        while (scanf("%d", &cpuTemp) != EOF && cpuTemp != -1) {
            processes[proIdx].CPUJobs[processes[proIdx].CPUJobIdx] = cpuTemp;
            processes[proIdx].CPUJobsRef[processes[proIdx].CPUJobIdx] = proIdx;
            processes[proIdx].CPUJobIdx++;
            if (scanf("%d", &ioTemp) != -1 && ioTemp != EOF) {
                processes[proIdx].IOJobs[processes[proIdx].IOJobIdx] = ioTemp;
                processes[proIdx].IOJobsRef[processes[proIdx].IOJobIdx] = proIdx;
                processes[proIdx].IOJobIdx++;               
            } else {
                break;
            }
        }
        // printf("TOTAL JOB IDX FOR PROCESS %d IS %d\n", proIdx, processes[proIdx].TotalJobIdx);
        processes[proIdx].TotalJobIdx = processes[proIdx].IOJobIdx + processes[proIdx].CPUJobIdx;
        proIdx++;
        
    }
    for (int i = 0; i < proIdx; i++){
    printf("arrival time = %d\n", processes[i].arrivalTime);
    for (int y  = 0; y < processes[i].CPUJobIdx; y++){
        printf("CPUJob%d = %d\n", y, processes[i].CPUJobs[y]);
    }
    for (int y  = 0; y < processes[i].IOJobIdx; y++){
        printf("IOJob%d = %d\n", y, processes[i].IOJobs[y]);
    }
    }
    int arrivalIdx = 0;
    double totalTime = 0;
    int tempo = 0;
    while(1){
        if (tempo == proIdx){
            break;
        }
       //printf("TICKER IS %d\n", ticker);
        if (processes[arrivalIdx].arrivalTime == ticker){
            struct Jobs temp;
            temp.job = processes[arrivalIdx].CPUJobs[processes[arrivalIdx].CPUJobCount];
            temp.ref = processes[arrivalIdx].CPUJobsRef[processes[arrivalIdx].CPUJobCount];
            processes[arrivalIdx].CPUJobCount++;
            processes[arrivalIdx].TotalJobCount++;
            printf("ENQUEUED CPU JOB %d AT REFERENCE %d at TICK %d\n", temp.job, temp.ref, ticker);
            enqueue(cpu, temp.job);
            enqueue(cpuref, temp.ref);
            arrivalIdx++;
        }
        if (curIo.job == -1){
            if(!isEmpty(io)){
                curIo.job = dequeue(io);
                curIo.ref = dequeue(ioref);
                printf("DEQUEUED IO JOB %d at REFERENCE %d\n", curIo.job, curIo.ref);
            }
        }
        if (curCpu.job == -1){
            curCpu.job = dequeue(cpu);
            curCpu.ref = dequeue(cpuref);
            printf("DEQUEUED FIRST CPU JOB %d AT REF %d AT TICK %d\n", curCpu.job, curCpu.ref, ticker);

        }
        if (curCpu.job == 0){
            printf("CPU JOB IS DONE AT TICK %d\n", ticker);
            if (processes[curCpu.ref].TotalJobCount == processes[curCpu.ref].TotalJobIdx){
                
                totalTime += ticker - processes[curCpu.ref].arrivalTime;
                processes[curCpu.ref].TotalJobCount++;
                tempo++;
                printf("TEMPO IS NOW %d", tempo);
            }
            printf("TOTAL JOB COUNT = %d\n", processes[curCpu.ref].TotalJobCount);
            if (processes[curCpu.ref].IOJobCount != processes[curCpu.ref].IOJobIdx){
                printf("ENQUEUED IOJOB %d FOR PROCESS %d AT TICK %d\n", processes[curCpu.ref].IOJobs[processes[curCpu.ref].IOJobCount], 
                processes[curCpu.ref].IOJobCount, ticker);
                enqueue(io, processes[curCpu.ref].IOJobs[processes[curCpu.ref].IOJobCount]);
                enqueue(ioref, processes[curCpu.ref].IOJobsRef[processes[curCpu.ref].IOJobCount]);
                processes[curCpu.ref].IOJobCount++;
                processes[curCpu.ref].TotalJobCount++;
            }
            printf("TOTAL JOB COUNT FOR PROCESS %d = %d. TOTAL IS %d\n", curCpu.ref, processes[curCpu.ref].TotalJobCount, processes[curCpu.ref].TotalJobIdx);
            if(!isEmpty(cpu)){
                curCpu.job = dequeue(cpu);
                curCpu.ref = dequeue(cpuref);
                printf("DEQUEUED CPU JOB %d AT REF %d AT TICK %d\n", curCpu.job, curCpu.ref, ticker);
            }

        }
        if (curIo.job == 0){
            printf("IO JOB IS DONE AT TICK %d\n", ticker);
            if (processes[curIo.ref].TotalJobCount == processes[curIo.ref].TotalJobIdx){
                totalTime += ticker - processes[curIo.ref].arrivalTime;
                tempo++;
            }
            if(!isEmpty(io)){
                curIo.job = dequeue(io);
                curIo.ref = dequeue(ioref);
                printf("DEQUEUED IOJOB %d with REFERENCE %d\n", curIo.job, curIo.ref);
            }
            if (processes[curIo.ref].CPUJobCount != processes[curCpu.ref].CPUJobIdx+1){
                printf("Enqueued CPUJOB %d with CPUJOBCOUNT %d FOR PROCESS %d AT TICK %d\n", processes[curIo.ref].CPUJobs[processes[curIo.ref].CPUJobCount]
                 , processes[curIo.ref].CPUJobCount, curIo.ref, ticker);
                enqueue(cpu, processes[curIo.ref].CPUJobs[processes[curIo.ref].CPUJobCount]);
                enqueue(cpuref, processes[curIo.ref].CPUJobsRef[processes[curIo.ref].CPUJobCount]);
                processes[curIo.ref].CPUJobCount++;
                processes[curIo.ref].TotalJobCount++;                    
            }
            printf("TOTAL JOB COUNT FOR PROCESS %d = %d. TOTAL IS %d\n", curIo.ref, processes[curIo.ref].TotalJobCount, 
            processes[curIo.ref].TotalJobIdx);
        }
        // if (processes[curIo.ref].TotalJobCount == processes[curIo.ref].TotalJobIdx){
        //         printf("ADDED TO TOTAL TIME\n");
        //         totalTime += ticker - processes[curIo.ref].arrivalTime;
        //         processes[curIo.ref].TotalJobCount++;
        // }
        // if (processes[curCpu.ref].TotalJobCount == processes[curCpu.ref].TotalJobIdx){
        //         printf("ADDED TO TOTAL TIME\n");
        //         totalTime += ticker - processes[curCpu.ref].arrivalTime;
        //         processes[curCpu.ref].TotalJobCount++;
        // }
        ticker++;
        curCpu.job--;
        if (curIo.job != -1){
            curIo.job--;
        }
    }
    totalTime = totalTime/proIdx;
    printf("%.0lf\n", totalTime);

    for (int i = 0; i < proIdx; i++){
        free(processes[i].CPUJobs);
        free(processes[i].IOJobs);
    }

    return 0;
}

