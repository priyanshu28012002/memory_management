#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/ipc.h>

#define SHM_SIZE 1024 // Shared memory size
#define SHM_KEY 2345  // Shared memory key

int main() {
    int shmid;
    void *shared_memory;
    char *data;

    // Get the shared memory segment using the key (same as the writer process)
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

    // Cast shared memory pointer to a string
    data = (char *)shared_memory;

    // Print the data from shared memory
    printf("Data read from shared memory: %s\n", data);

    // Detach from the shared memory segment
    shmdt(shared_memory);

    return 0;
}
