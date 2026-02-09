#include <iostream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <cstring>
#include <cstdlib>
#include <string>

using namespace std;

#define DATA_FILE "/tmp/ipc_data.txt"
#define LOCK_FILE "/tmp/ipc_lock.lock"

int main() {
    cout << "File Writer started...\n";

    // Open or create the lock file
    int lock_fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0666);
    if (lock_fd == -1) {
        perror("Failed to open lock file");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i <= 5; ++i) {
        // Acquire exclusive lock to prevent race conditions
        cout << "Writer: Waiting for lock...\n";
        if (flock(lock_fd, LOCK_EX) == -1) {
            perror("Failed to acquire lock");
            close(lock_fd);
            exit(EXIT_FAILURE);
        }

        cout << "Writer: Lock acquired\n";

        // Critical section: Write to file
        ofstream file(DATA_FILE, ios::app);
        if (!file.is_open()) {
            perror("Failed to open data file");
            flock(lock_fd, LOCK_UN);
            close(lock_fd);
            exit(EXIT_FAILURE);
        }

        string message = "Message " + to_string(i) + " from Writer Process";
        file << message << endl;
        cout << "Writer: Wrote - " << message << endl;
        file.close();

        // Release the lock
        if (flock(lock_fd, LOCK_UN) == -1) {
            perror("Failed to release lock");
            close(lock_fd);
            exit(EXIT_FAILURE);
        }

        cout << "Writer: Lock released\n";

        // Simulate writing time
        sleep(2);
    }

    close(lock_fd);
    cout << "File Writer finished.\n";

    return 0;
}
