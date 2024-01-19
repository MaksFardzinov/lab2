#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>

#define MAX_BUFFER_SIZE 1024  // Определяем максимальный размер буфера для хранения данных

static DEFINE_MUTEX(my_module_mutex); // Инициализируем мьютекс для предотвращения одновременного доступа
static char output_buffer[MAX_BUFFER_SIZE]; // Буфер для хранения данных, считанных из /proc/cpuinfo

// Функция для чтения информации о памяти из /proc/cpuinfo
static ssize_t read_cpuinfo(char *buf) {
    struct file *cpuinfo;
    loff_t pos = 0;  // Позиция в файле для чтения
    ssize_t result;  // Результат чтения файла

    // Открываем файл /proc/cpuinfo для чтения
    cpuinfo = filp_open("/proc/cpuinfo", O_RDONLY, 0);
    if (IS_ERR(cpuinfo)) {
        pr_info("filp_open error\n");
        return PTR_ERR(cpuinfo); // Возвращаем код ошибки, если файл не может быть открыт
    }

    // Читаем данные из файла в буфер
    result = kernel_read(cpuinfo, buf, MAX_BUFFER_SIZE, &pos);

    // Закрываем файл
    filp_close(cpuinfo, NULL);

    return result; // Возвращаем количество прочитанных байт или код ошибки
}

// Функция чтения, вызываемая при чтении из файла /proc/my_module
static ssize_t my_module_read(struct file *file, char __user *buffer, size_t length, loff_t *offset) {
    ssize_t err;

    // Захватываем мьютекс перед чтением данных
    mutex_lock(&my_module_mutex);

    // Чтение данных о памяти
    err = read_cpuinfo(output_buffer);
    if (err < 0) {
        // В случае ошибки освобождаем мьютекс и возвращаем код ошибки
        mutex_unlock(&my_module_mutex);
        return err;
    }

    // Копирование данных из ядра в буфер пользователя
    err = simple_read_from_buffer(buffer, length, offset, output_buffer, strlen(output_buffer));

    // Освобождаем мьютекс после завершения операции
    mutex_unlock(&my_module_mutex);

    return err; // Возвращаем количество прочитанных байт или код ошибки
}

// Определение операций для /proc файла
static const struct proc_ops my_module_proc_ops = {
    .proc_read = my_module_read, // Функция чтения
};

// Функция инициализации модуля
static int __init my_module_init(void) {
    // Создаем файл /proc/my_module
    proc_create("my_module", 0, NULL, &my_module_proc_ops);
    pr_info("My module has been inserted.\n");
    return 0; // Возвращаем 0, если модуль успешно инициализирован
}

// Функция выхода модуля
static void __exit my_module_exit(void) {
    // Удаляем файл /proc/my_module
    remove_proc_entry("my_module", NULL);
    pr_info("My module has been removed.\n");
}

module_init(my_module_init); // Регистрация функции инициализации модуля
module_exit(my_module_exit); // Регистрация функции выхода модуля

// Метаданные модуля
MODULE_LICENSE("No license");
MODULE_AUTHOR("Fardzinov Maksim");
MODULE_DESCRIPTION("OS Lab2");
MODULE_VERSION("1");
