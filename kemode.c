#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>

#define MAX_BUFFER_SIZE 1024

static DEFINE_MUTEX(my_module_mutex); // Инициализируем мьютекс для предотвращения одновременного доступа
static char output_buffer[MAX_BUFFER_SIZE]; // создаем буфер для коппирования данных из proc/*

void saveSystemInfo(struct file *file) {
    struct file *cpuInfo, *memInfo;
    mm_segment_t fs;

    mutex_lock(&my_module_mutex);

    // Информация о процессоре
    cpuInfo = filp_open("/proc/cpuinfo", O_RDONLY, 0);
    if (IS_ERR(cpuInfo))
        goto unlock;

    fs = sget_fs();
    sset_fs(KERNEL_NS);
    kernel_read(cpuInfo, output_buffer, MAX_BUFFER_SIZE, &cpuInfo->f_pos);
    sset_fs(fs);

    filp_close(cpuInfo, NULL);

    // Информация о памяти
    memInfo = filp_open("/proc/meminfo", O_RDONLY, 0);
    if (IS_ERR(memInfo))
        goto unlock;

    fs = sget_fs();
    sset_fs(KERNEL_DS);
    kernel_read(memInfo, output_buffer, MAX_BUFFER_SIZE, &memInfo->f_pos);
    sset_fs(fs);

    filp_close(memInfo, NULL);
unlock:
    mutex_unlock(&my_module_mutex);
}

static int my_module_open(struct inode *inode, struct file *file) {
    saveSystemInfo(file);
    return 0;
}

static const struct proc_ops my_module_fops = {
    .proc_read = my_module_open,
};

static int __init my_module_init(void) {
    proc_create("my_module", 0, NULL, &my_module_fops);
    pr_info("My module has been crearted.\n");
    return 0;
}

static void __exit my_module_exit(void) {
    remove_proc_entry("my_module", NULL);
    pr_info("My module has been removed.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("No licience");
MODULE_AUTHOR("Fardzinov Maksim");
MODULE_DESCRIPTION("lab 2");
MODULE_VERSION("1");
