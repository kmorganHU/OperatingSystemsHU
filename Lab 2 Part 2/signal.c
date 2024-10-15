#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int signal_received = 0; 

void handler(int signum) {
    printf("Hello World!\n");
    signal_received = 1;  // Set the flag when signal is received
}

int main(int argc, char *argv[]) {
    signal(SIGALRM, handler);  // Register the signal handler

    while (1) {
        alarm(5);  // SIGALRM every 5 seconds

        // Wait for the signal
        while (!signal_received) {
            // Busy wait for the signal to be delivered
        }

        // After receiving the signal
        printf("Turing was right!\n");

        // Reset signal_received
        signal_received = 0;
    }

    return 0;
}