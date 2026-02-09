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
    cout << "File Reader started...\n";

    // Open or create the lock file
    int lock_fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0666);
    if (lock_fd == -1) {
        perror("Failed to open lock file");
        exit(EXIT_FAILURE);
    }

    int read_count = 0;
    int max_attempts = 15; // Allow multiple attempts to read messages
    int attempts = 0;

    while (read_count < 5 && attempts < max_attempts) {
        attempts++;

        // Acquire shared lock for reading (allows multiple readers)
        cout << "Reader: Waiting for lock...\n";
        if (flock(lock_fd, LOCK_SH) == -1) {
            perror("Failed to acquire lock");
            close(lock_fd);
            exit(EXIT_FAILURE);
        }

        cout << "Reader: Lock acquired\n";

        // Critical section: Read from file
        ifstream file(DATA_FILE);
        if (file.is_open()) {
            string line;
            int line_count = 0;

            // Read all lines and display them
            while (getline(file, line)) {
                line_count++;
                if (line_count > read_count) {
                    cout << "Reader: Read - " << line << endl;
                    read_count++;
                }
            }

            file.close();
        } else {
            cout << "Reader: Data file not yet available\n";
        }

        // Release the lock
        if (flock(lock_fd, LOCK_UN) == -1) {
            perror("Failed to release lock");
            close(lock_fd);
            exit(EXIT_FAILURE);
        }

        cout << "Reader: Lock released\n";

        // Wait before next read attempt
        sleep(1);
    }

    close(lock_fd);

    if (read_count >= 5) {
        cout << "File Reader finished successfully. Read " << read_count << " messages.\n";
    } else {
        cout << "File Reader finished. Read " << read_count << " messages (timeout).\n";
    }

    return 0;
}
