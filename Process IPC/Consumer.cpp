#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

using namespace std;

#define SHM_SIZE 1024 // Size of shared memory

int main() {
    // Open existing named semaphores for producer-consumer synchronization
    sem_t *sem_empty = sem_open("/sem_empty", 0); // Tracks empty slots
    sem_t *sem_full = sem_open("/sem_full", 0);  // Tracks filled slots

    if (sem_empty == SEM_FAILED || sem_full == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Generate a unique key for shared memory
    key_t key = ftok("/tmp", 'S');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Access the shared memory segment
    int shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory segment
    char *shared_memory = (char *)shmat(shmid, NULL, 0);
    if (shared_memory == (char *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < 5; ++i) {
        sem_wait(sem_full); // Wait for a filled slot

        // Critical section: Read from shared memory
        cout << "Consumer read: " << shared_memory << endl;

        sem_post(sem_empty); // Signal an empty slot
        sleep(1); // Simulate processing time
    }

    // Detach from shared memory
    if (shmdt(shared_memory) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // Cleanup: Close semaphores
    if (sem_close(sem_empty) == -1 || sem_close(sem_full) == -1) {
        perror("sem_close");
        exit(EXIT_FAILURE);
    }

    return 0;
}