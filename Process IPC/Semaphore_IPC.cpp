#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <semaphore.h>
#include <fcntl.h>

using namespace std;

int main() {
    // Create or open a named semaphore with the following parameters:
    // "/my_semaphore": Name of the semaphore (must start with a '/').
    // O_CREAT: Create the semaphore if it does not exist.
    // 0666: Permissions for the semaphore (read and write for all users).
    // 1: Initial value of the semaphore.
    sem_t *sem = sem_open("/my_semaphore", O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    // Fork a new process to create a parent and child process
    pid_t pid = fork();

    if (pid == -1) {
        // Handle fork failure
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        cout << "Child process waiting for semaphore...\n";
        sem_wait(sem); // Wait (P operation) to acquire the semaphore
        cout << "Child process acquired semaphore.\n";

        // Critical section: Code that requires exclusive access
        cout << "Child process is in the critical section.\n";
        sleep(2); // Simulate some work in the critical section

        cout << "Child process releasing semaphore.\n";
        sem_post(sem); // Signal (V operation) to release the semaphore
    } else { // Parent process
        cout << "Parent process waiting for semaphore...\n";
        sem_wait(sem); // Wait (P operation) to acquire the semaphore
        cout << "Parent process acquired semaphore.\n";

        // Critical section: Code that requires exclusive access
        cout << "Parent process is in the critical section.\n";
        sleep(2); // Simulate some work in the critical section

        cout << "Parent process releasing semaphore.\n";
        sem_post(sem); // Signal (V operation) to release the semaphore

        // Wait for the child process to finish execution
        wait(NULL);

        // Cleanup: Close the semaphore
        if (sem_close(sem) == -1) {
            perror("sem_close");
            exit(EXIT_FAILURE);
        }

        // Cleanup: Unlink the named semaphore
        // sem_unlink() removes the named semaphore from the system.
        // After unlinking, the semaphore name is no longer available for use.
        // However, processes that already have the semaphore open can continue to use it.
        if (sem_unlink("/my_semaphore") == -1) {
            perror("sem_unlink");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}