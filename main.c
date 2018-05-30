#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#define BUFSIZE 67108864 // 64MB buffer
#define NUMTHREADS 4 // test with 1, 2, 4, and 8

// global fields
char buf[BUFSIZE];
int charCount[NUMTHREADS][128];

struct ThreadRange {
    int threadNum, start, end;
};

void *countCharacters(void *vargs) {
    struct ThreadRange *threadinfo = (struct ThreadRange *)vargs;
    int threadNum = threadinfo->threadNum;
    int start = threadinfo->start;
    int end = threadinfo->end;
    
    for (int bufIndex = start; bufIndex <= end; bufIndex++) {
        int charVal = (int)buf[bufIndex];
        charCount[threadNum][charVal]++;
    }
    return NULL;
}

int main(int argc, const char * argv[]) {
    
    int source; // file
    ssize_t readSize; // filesize
    pthread_t tid[NUMTHREADS];
    
    /* CHECK FOR CORRECT NUMBER OF ARGS */
    if (argc != 2) {
        printf("Error: incorrect number of arguments\n");
        printf("Correct format: ./programName fileName\n");
        return (1);
    }
    
    /* ATTEMPT TO OPEN FILE */
    source = open(argv[1], O_RDONLY); // attempt to open source filename
    
    // error if file does not exist
    if (source < 0) {
        fprintf( stderr, "Error: %s\n", strerror(errno) );
        return EXIT_FAILURE;
    }
    
    /* CREATE STRUCTS TO HOLD INFORMATION NEEDED FOR THREADS */
    struct ThreadRange * threadrange[NUMTHREADS];
    for (int threadNum = 0; threadNum < NUMTHREADS; threadNum++) {
        // allocate memory for each struct
        threadrange[threadNum] = malloc(sizeof(struct ThreadRange));
    }
    
    /* PROCESS FILE PER BUFFER SIZE */
    while ( ( readSize = read(source, buf, BUFSIZE) ) > 0 ) {
        
        // create threads
        int splitSize = (int)(readSize / NUMTHREADS); // base number of bytes for each thread
        int leftoverBytes = readSize % NUMTHREADS; // leftover bytes to distribute
        int currentByte = -1;
        
        // create each thread with needed information
        for(int threadNum = 0; threadNum < NUMTHREADS; threadNum++) {
            
            // assign needed values in current struct
            threadrange[threadNum]->threadNum = threadNum; // thread number
            
            currentByte++;
            threadrange[threadNum]->start = currentByte; // starting byte in buffer
            
            // add one to current byte if there are leftover bytes
            if (leftoverBytes > 0)
                currentByte += ( (splitSize + 1) - 1);
            // don't add anything to current byte if there are no leftover bytes
            else
                currentByte += (splitSize - 1);
            
            threadrange[threadNum]->end = currentByte; // ending byte in buffer
            
            pthread_create(&tid[threadNum], NULL, countCharacters, (void*)threadrange[threadNum]);
            
            // decrement leftover bytes until there aren't anymore
            if (leftoverBytes > 0)
                leftoverBytes--;

        } // end of worker thread creation
    } // end of file processing
    
    // join all worker threads
    for (int threadNum = 0; threadNum < NUMTHREADS; threadNum++) {
        pthread_join(tid[threadNum], NULL);
    }
    
    /* ADD UP TOTAL COUNTS AND PRINT RESULTS */
    int totalCount[128];
    for (int asciiNum = 0; asciiNum < 128; asciiNum++) {
        totalCount[asciiNum] = 0;
        for (int threadNum = 0; threadNum < NUMTHREADS; threadNum++) {
            totalCount[asciiNum] += charCount[threadNum][asciiNum];
        }
        // print hex for: 0-32, 127
        if (asciiNum > 32 && asciiNum <127)
            printf("%d occurrences of %c\n", totalCount[asciiNum], (char)asciiNum);
        else
            printf("%d occurrences of 0x%x\n", totalCount[asciiNum], asciiNum);
    }
    
    close(source);
    
    return 0;
}
