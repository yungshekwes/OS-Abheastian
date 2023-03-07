#include <stdio.h>
#include <string.h>
void dequeue(){

}

int main(){
    // Initialize max size inputStream and variables used.
    int inputStream[100];
    int streamSize = 0, back = 0, pageFault = 0, flag = 0, physMem, temp;

    // Take as input the physical memory slots
    scanf("%d\n", &physMem);

    // Initialize FIFO queue, tempArray which will be used to overwrite queue and front part of FIFO
    int queue[physMem];
    int tempArray[physMem];
    int front = physMem;

    // Set queue to -1 such that it can be checked with the input stream
    for (int i = 0; i < physMem; i++){
        queue[i] = -1;
    }

    // Scan input stream until newline
    while (1){
        scanf("%d", &temp);
        inputStream[streamSize] = temp;
        streamSize++;
        if (getchar() == '\n'){
            break;
        }
    }

    // Loop through all characters in input stream
    for (int i = 0; i < streamSize; i++){
        
        if (back != physMem){

            for (int y = 0; y < physMem; y++){
                if (queue[y] == inputStream[i]){
                    flag = 1;
                    break;
                }
            }

            if (!flag){
                pageFault++;
                front--;
                queue[front] = inputStream[i];
                back++;
            }

            flag = 0;

        } else {

            for (int y = 0; y < physMem; y++){
                if (queue[y] == inputStream[i]){
                    flag = 1;
                    break;
                }
            }

            if (!flag) {
                tempArray[0] = inputStream[i];
                for (int y = 1; y < physMem; y++){
                    tempArray[y] = queue[y-1];
                }
                for (int y = 0; y < physMem; y++){
                    queue[y] = tempArray[y];
                }
                pageFault++;
            }

            flag = 0;
        }

    }

    printf("%d\n", pageFault);

    return 0;
}
