#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main() {
    int p_c1_fd[2], c1_c2_fd[2], c2_p_fd[2]; // Three pipes

    // 1. Create all pipes in parent
    pipe(p_c1_fd);
    pipe(c1_c2_fd);
    pipe(c2_p_fd);

    pid_t pid1 = fork();

    if (pid1 == 0) { // Child 1 process
        // Close unused pipes for Child 1
        close(p_c1_fd[1]); // C1 reads from parent
        close(c1_c2_fd[0]); // C1 writes to C2
        close(c2_p_fd[0]); close(c2_p_fd[1]); // C1 doesn't use c2_p pipe

        char buffer[100];
        read(p_c1_fd[0], buffer, sizeof(buffer));
        // Process data (e.g., append something) and pass to child 2
        strcat(buffer, " processed by C1");
        write(c1_c2_fd[1], buffer, strlen(buffer) + 1);

        close(p_c1_fd[0]);
        close(c1_c2_fd[1]);
        exit(0);
    }

    pid_t pid2 = fork();

    if (pid2 == 0) { // Child 2 process
        // Close unused pipes for Child 2
        close(c1_c2_fd[1]); // C2 reads from C1
        close(c2_p_fd[0]); // C2 writes to parent
        close(p_c1_fd[0]); close(p_c1_fd[1]); // C2 doesn't use p_c1 pipe

        char buffer[100];
        read(c1_c2_fd[0], buffer, sizeof(buffer));
        // Process data (e.g., append something) and pass to parent
        strcat(buffer, " processed by C2");
        write(c2_p_fd[1], buffer, strlen(buffer) + 1);

        close(c1_c2_fd[0]);
        close(c2_p_fd[1]);
        exit(0);
    }

    // Parent process
    // Close unused pipes for Parent
    close(p_c1_fd[0]); // Parent writes to C1
    close(c2_p_fd[1]); // Parent reads from C2
    close(c1_c2_fd[0]); close(c1_c2_fd[1]); // Parent doesn't use c1_c2 pipe

    const char* message = "Original data";
    write(p_c1_fd[1], message, strlen(message) + 1);

    waitpid(pid1, NULL, 0); // Wait for child 1 to finish
    waitpid(pid2, NULL, 0); // Wait for child 2 to finish

    char final_result[100];
    read(c2_p_fd[0], final_result, sizeof(final_result));
    std::cout << "Parent received: " << final_result << std::endl;

    close(p_c1_fd[1]);
    close(c2_p_fd[0]);

    return 0;
}
