# ascii-character-counter

# Problem assigned by Instructor John Baldwin for Operating System Principles (CSC 415)
For this assignment, you will implement a multi-threaded ASCII character count program. Your program will receive the name of an ASCII text file as a command-line argument and output the number of each kind of ASCII character in the file. In your code, set up a 64MB (67,108,864 bytes) buffer in global memory and read the contents of the file into the buffer (or as much of it as will fit). Then partition the buffer and spawn a number of threads to count the number of occurrences of each ASCII character in each partition. Use a #define preprocessor directive to specify the number of threads. Have each thread record these counts in a separate 128-element int array in global memory (stored collectively as a 2-dimensional int array). The partition bounds can be calculated in the main thread to be roughly equal in size and then passed to each worker thread using a struct. You will also need to pass a thread index to each thread so that it knows where to write its local count in the global 2-dimensional int array. Once the worker threads finish writing their local counts, they should exit. After spawning the worker threads, the main thread should wait for them all to complete. It should then add the partition counts for each ASCII character and print each overall count. For non- printable and white space ASCII characters, you should print the hexadecimal character code. Here is a sample execution:

~$ ./ASCIICount text_file
0 occurrences of 0x0
0 occurrences of 0x1
...
1032 occurrences of 0x20
5 occurrences of ‘!’
...

Your program should use the POSIX threads API including the pthread_create() and pthread_join() functions. Note that you will need to link your program against the POSIX threads library by specifying “-pthread” on the compiler command line.

# Other things
* large.txt provided by instructor for testing
