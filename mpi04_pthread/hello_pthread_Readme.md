gcc -g -Wall -pthread -o hello_pthread hello_pthread.c

./pth_hello <number of threads>
./pth_hello 1
Hello from the main thread
Hello from thread 0 of 1

./pth_hello 4
Hello from the main thread
Hello from thread 0 of 4
Hello from thread 1 of 4
Hello from thread 2 of 4
Hello from thread 3 of 4