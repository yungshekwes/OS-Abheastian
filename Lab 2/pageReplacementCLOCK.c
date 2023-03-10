#include <stdio.h>
#include <stdlib.h>

int main(){
    // Initialize max size inputStream and variables used.
    int inputStream[100];
    int streamSize = 0, pageFault = 0, physMem, temp;

    // Take as input the physical memory slots
    scanf("%d\n", &physMem);

    // Initialize clock ptr pointer and frames for pages and reference bits
    int ptr = 0;
    int frames[physMem];
    int secChance[physMem];

    // Initialize frames and reference bits to -1 and 0, respectively
    for (int i = 0; i < physMem; i++){
        frames[i] = -1;
        secChance[i] = 0;
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

    // Loop through all pages in input stream
    for (int i = 0; i < streamSize; i++){
        int page = inputStream[i];

        // Check if the page is already in the frames
        int inFrames = 0;
        for (int j = 0; j < physMem; j++){
            if (frames[j] == page){
                inFrames = 1;
                secChance[j] = 1;
                break;
            }
        }

        // If the page is not in the frames, replace the page pointed to by the ptr
        if (!inFrames){
            while (secChance[ptr]){
                secChance[ptr] = 0;
                ptr = (ptr + 1) % physMem;
            }
            frames[ptr] = page;
            ptr = (ptr + 1) % physMem;
            pageFault++;
        }
    }

    // Print final pageFault counter
    printf("%d\n", pageFault);

    return 0;
}