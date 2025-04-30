#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/wait.h>

#define SHM_SIZE 1024  // Shared memory size
#define SHM_KEY 2345   // Shared memory key

int main() {
    int shmid;
    void *shared_memory;
    char *data;
    char last_data[SHM_SIZE];  // To store the last read data from shared memory
    memset(last_data, 0, sizeof(last_data)); // Initialize last_data to empty

    // Get the shared memory segment using the key
    shmid = shmget((key_t)SHM_KEY, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Attach to the shared memory segment
    shared_memory = shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }
    data = (char *)shared_memory;

    printf("Monitoring shared memory. Press Ctrl+C to stop...\n");

    // Continuously monitor the shared memory
    while (1) {
        // Compare the current data with the last read data
        if (strcmp(data, last_data) != 0) {
            // If the content has changed, print the updated content
            printf("Shared memory updated: %s\n", data);

            // Update last_data to the current content in shared memory
            strcpy(last_data, data);
        }

        // Sleep for 1 second before checking again
        sleep(1);
    }

    // Detach from the shared memory segment
    shmdt(shared_memory);

    return 0;
}
