#include <stdio.h>
#include <string.h>

struct process {
    int pid;
    int CPUTime;
    int IOTime;
    int arrivalTime;
};

int main(){
    struct process processes[100];
    int pidIdx = 0, curCPU, curIO;
    while(scanf("%d", &processes[pidIdx].arrivalTime) != EOF){

        printf("arrival time = %d\n", processes[pidIdx].arrivalTime);
        int sumCPU = 0;
        int sumIO = 0;
        while (1){
            scanf("%d", &curCPU);
            if (curCPU == -1 || curCPU == EOF){
                break;
            }
            printf("curCPU = %d\n", curCPU);
            sumCPU += curCPU;
            scanf("%d", &curIO);
            printf("curIO = %d\n", curIO);
            if (curIO != -1){
                sumIO += curIO;
            } else {
                printf("BREAK OCCURED OMG\n");
                break;
            }
        }
        printf("LEFT THE LOOP OMG\n");
        processes[pidIdx].CPUTime = sumCPU;
        processes[pidIdx].IOTime = sumIO;
        pidIdx++; 
    }
    // for (int i = 0; i < pidIdx; i++){
    //     printf("processes id = %d, CPUTime = %d, IOTime = %d, arrivalTime = %d\n", processes[i].pid, processes[i].CPUTime, processes[i].IOTime, processes[i].arrivalTime);
    // }
    return 0;
}