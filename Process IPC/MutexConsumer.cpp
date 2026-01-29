#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

using namespace std;

#define BUFFER_SIZE 256 // Size of each message buffer

// Structure to hold shared data and mutex
struct SharedData {
    pthread_mutex_t mutex; // Mutex for synchronization
    char buffer[BUFFER_SIZE]; // Buffer to store messages
    int count; // Number of messages in buffer
    bool ready; // Flag to indicate data is ready
};

int main() {
    // Generate a unique key for shared memory
    key_t key = ftok("/tmp", 'P');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    // Access the existing shared memory segment
    int shmid = shmget(key, sizeof(SharedData), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory segment
    SharedData *shared_data = (SharedData *)shmat(shmid, NULL, 0);
    if (shared_data == (SharedData *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    cout << "Consumer started...\n";

    for (int i = 0; i < 5; ++i) {
        // Lock the mutex before accessing shared memory
        pthread_mutex_lock(&shared_data->mutex);

        // Critical section: Read from shared memory
        if (shared_data->ready) {
            cout << "Consumer read: " << shared_data->buffer << " (Count: " << shared_data->count << ")\n";
            shared_data->ready = false;
        } else {
            cout << "Consumer: No new data available\n";
        }

        // Unlock the mutex after accessing shared memory
        pthread_mutex_unlock(&shared_data->mutex);

        sleep(1); // Simulate processing time
    }

    // Detach from shared memory
    if (shmdt(shared_data) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    // Cleanup: Destroy the mutex
    pthread_mutex_destroy(&shared_data->mutex);

    // Cleanup: Remove the shared memory segment
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }

    cout << "Consumer finished and cleaned up shared memory.\n";

    return 0;
}