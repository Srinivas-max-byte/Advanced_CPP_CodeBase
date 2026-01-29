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

    // Create shared memory segment large enough for SharedData structure
    int shmid = shmget(key, sizeof(SharedData), 0666 | IPC_CREAT);
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

    // Initialize the mutex with process-shared attribute
    // This allows the mutex to be shared between different processes
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    // Set the mutex to be shared between processes (not just threads)
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared_data->mutex, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    // Initialize shared data
    shared_data->count = 0;
    shared_data->ready = false;

    cout << "Producer started...\n";

    for (int i = 0; i < 5; ++i) {
        // Lock the mutex before accessing shared memory
        pthread_mutex_lock(&shared_data->mutex);

        // Critical section: Write to shared memory
        string message = "Message " + to_string(i + 1);
        strncpy(shared_data->buffer, message.c_str(), BUFFER_SIZE - 1);
        shared_data->buffer[BUFFER_SIZE - 1] = '\0'; // Ensure null termination
        shared_data->count++;
        shared_data->ready = true;
        cout << "Producer wrote: " << message << " (Count: " << shared_data->count << ")\n";

        // Unlock the mutex after accessing shared memory
        pthread_mutex_unlock(&shared_data->mutex);

        sleep(1); // Simulate production time
    }

    // Detach from shared memory
    if (shmdt(shared_data) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }

    cout << "Producer finished.\n";

    return 0;
}