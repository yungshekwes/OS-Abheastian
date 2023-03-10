void findTime(Process *p, int numProc) {
    int cpuSz = p[numProc-1].CPUsz, ioSz = p[numProc-1].IOsz;
    Queue cpuQ = newQueue(cpuSz), ioQ = newQueue(ioSz);
    int time = 0, totalTurnaroundTime = 0, currentProcIdx = 0, processed = 0;

    while (processed < numProc) {
        // Check for new processes
        while (p[currentProcIdx].arrivalTime >= time && currentProcIdx <= numProc-1) {
            enqueue(p[currentProcIdx], &cpuQ);            
            currentProcIdx++;
        }
        
        int justAddedToIO = 0;

        // Process the CPU queue
        if (!isEmptyQueue(cpuQ)) {
            Process currentProcess = cpuQ.array[cpuQ.front];
            currentProcess.CPUtimes[currentProcess.currentCPU]--;

            if (currentProcess.CPUtimes[currentProcess.currentCPU] <= 0) { // cpu not busy so we can enqueue IO
                currentProcess.currentCPU++;
                if (currentProcess.currentCPU == currentProcess.CPUcounter && currentProcess.currentIO == currentProcess.IOcounter) {
                    totalTurnaroundTime += (time+1) - currentProcess.arrivalTime;
                    processed ++;
                    if (!isEmptyQueue(cpuQ)) cpuQ.front = (cpuQ.front + 1) % cpuQ.size; //dequeue if not empty
                } else {
                    cpuQ.front = (cpuQ.front + 1) % cpuQ.size; // dequeueing
                    if(isEmptyQueue(ioQ)) {
                        justAddedToIO = 1;
                    }
                    enqueue(currentProcess, &ioQ);
                }
            } 
        }

        // Process the I/O queue
        if (!isEmptyQueue(ioQ) && !justAddedToIO) {
            Process currentProcess = ioQ.array[ioQ.front];
            currentProcess.IOtimes[currentProcess.currentIO]--;          
    
            if (currentProcess.IOtimes[currentProcess.currentIO] <= 0) {
                currentProcess.currentIO++;
                if (currentProcess.currentCPU == currentProcess.CPUcounter && currentProcess.currentIO == currentProcess.IOcounter) {
                    totalTurnaroundTime += (time+1) - currentProcess.arrivalTime;
                    processed ++;
                    if (!isEmptyQueue(ioQ)) ioQ.front = (ioQ.front + 1) % ioQ.size; //dequeue if not empty
                } else {
                    ioQ.front = (ioQ.front + 1) % ioQ.size; // dequeueing
                    enqueue(currentProcess, &cpuQ);
                }
            } 
        }
        time++;
    }
    
    freeQueue(cpuQ);
    freeQueue(ioQ);
    double avgTurnaroundTime = (double) totalTurnaroundTime / numProc;
    printf("%.0lf\n", avgTurnaroundTime);
}