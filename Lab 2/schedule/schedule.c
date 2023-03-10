#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    while(1) {
        if (incomingqueue.num_processes && time == incomingqueue.process[0].arrival) {
            enqueue(&cpuqueue, dequeue(&incomingqueue));
        }
        if (cpuqueue.num_processes && cpuqueue.process[0].durations->data == 0) {
            process p = dequeue(&cpuqueue);
            if (p.duration->next == NULL) {
                total_duration += time - p.arrival;
                free(p.durations);
            } else {
                intLL *temp = p.durations->next;
                free(p.durations);
                p.durations = temp;
                enqueue(&ioqueue, p);
            }
        }

        if (ioqueue.num_processes && ioqueue.process[0].durations->data == 0) {
            process p = dequeue(&ioqueue);

            if(p.durations->next == NULL) {
                total_duration += time - p.arrival;
                free(p.durations);
            } else {
                intLL *temp = p.durations->next;
                free(p.durations);
                p.durations = temp;
                enqueue(&cpuqueue, p);
            }
        }

        if (incomingqueue.num_processes == 0 && cpuqueue.num_processes == 0 && ioqueue.num_processes == 0) {
            break;
        }

        time++;
        cpuqueue.num_processes && cpuqueue.processes[0].durations->data--;
        ioqueue.num_processes && ioqueue.processes[0].durations->data--;
    }

    printf("%.0f\n", total_duration / num_processes)
}