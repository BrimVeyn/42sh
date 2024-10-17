#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

// Signal handler for SIGINT
void handle_sigint(int sig) {
    write(STDOUT_FILENO, "SIGINT caught!\n", 15);
}

int main() {
    // Set up the signal handler for SIGINT
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sa.sa_flags = 0;  // Do not use SA_RESTART
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    // Attempt to read from stdin
    char buffer[128];
    printf("Reading... (Press Ctrl+C to interrupt)\n");
    
    ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer));

    if (n == -1) {
        if (errno == EINTR) {
            // Read was interrupted by the SIGINT signal
            printf("Read was interrupted by SIGINT (EINTR)\n");
        } else {
            // Some other error
            perror("read");
        }
    } else {
        // Successfully read some data
        printf("Read succeeded: %.*s\n", (int)n, buffer);
    }

    return 0;
}
