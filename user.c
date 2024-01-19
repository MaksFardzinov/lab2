#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    int cpuFile;
    ssize_t bytes_read;
    char buffer[MAX_BUFFER_SIZE];

    cpuFile = open("/proc/my_module", O_RDONLY);
    if (cpuFile == -1) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    bytes_read = read(cpuFile, buffer, MAX_BUFFER_SIZE - 1);
    if (bytes_read == -1) {
        perror("Error reading file");
        close(cpuFile);
        return EXIT_FAILURE;
    }

    buffer[bytes_read] = '\0';

    printf("Data read from module:\n%s\n", buffer);

    close(cpuFile);

    return EXIT_SUCCESS;
}
