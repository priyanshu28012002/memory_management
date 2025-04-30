#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/wait.h>  // Include this for wait() function


#define SHM_SIZE 1024 // Shared memory size
#define SEM_KEY 1234  // Semaphore key
#define SHM_KEY 2345  // Shared memory key

// Semaphore operations
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// Function to perform semaphore wait (P operation)
void semaphore_wait(int semid) {
    struct sembuf sem_op;
    sem_op.sem_num = 0;      // Semaphore number (only one semaphore in this case)
    sem_op.sem_op = -1;      // Decrement the semaphore value (P operation)
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}

// Function to perform semaphore signal (V operation)
void semaphore_signal(int semid) {
    struct sembuf sem_op;
    sem_op.sem_num = 0;      // Semaphore number
    sem_op.sem_op = 1;       // Increment the semaphore value (V operation)
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}

int main() {
    int shmid, semid;
    void *shared_memory;
    char *data;
    pid_t pid;
    union semun sem_union;

    // Create shared memory segment
    shmid = shmget((key_t)SHM_KEY, SHM_SIZE, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Attach the shared memory to the process address space
    shared_memory = shmat(shmid, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat failed");
        exit(1);
    }
    data = (char *)shared_memory;

    // Create a semaphore set with one semaphore (binary semaphore)
    semid = semget((key_t)SEM_KEY, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    // Initialize the semaphore to 1 (unlocked)
    sem_union.val = 1;
    if (semctl(semid, 0, SETVAL, sem_union) == -1) {
        perror("semctl failed");
        exit(1);
    }

    // Create 3 child processes
    for (int i = 0; i < 3; i++) {
        pid = fork();
        if (pid == -1) {
            perror("fork failed");
            exit(1);
        }

        if (pid == 0) {  // Child process
            char message[100];
            snprintf(message, sizeof(message), "Process %d writing to shared memory.\n", getpid());

            // Synchronize access to shared memory using the semaphore
            semaphore_wait(semid);

            // Write to shared memory
            printf("Process %d is writing to shared memory.\n", getpid());
            strcpy(data, message);

            // Print the written message
            printf("Shared memory contains: %s\n", (char *)shared_memory);

            // Release the semaphore
            semaphore_signal(semid);

            // Exit the child process after writing
            exit(0);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    // Detach and destroy shared memory
    shmdt(shared_memory);
    shmctl(shmid, IPC_RMID, 0);

    // Destroy semaphore
    semctl(semid, 0, IPC_RMID, sem_union);

    return 0;
}