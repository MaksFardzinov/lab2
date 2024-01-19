#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    int fd;
    ssize_t bytes_read;
    char buffer[MAX_BUFFER_SIZE];

    // Открываем файл модуля
    fd = open("/proc/my_module", O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Читаем данные из файла
    bytes_read = read(fd, buffer, MAX_BUFFER_SIZE - 1);
    if (bytes_read == -1) {
        perror("Error reading file");
        close(fd);
        return EXIT_FAILURE;
    }

    // Добавляем символ окончания строки
    buffer[bytes_read] = '\0';

    // Выводим считанные данные
    printf("Data read from module:\n%s\n", buffer);

    // Закрываем файл
    close(fd);

    return EXIT_SUCCESS;
}
