#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>

#define MAX_BUFFER_SIZE 1024

static DEFINE_MUTEX(my_module_mutex);
static char output_buffer[MAX_BUFFER_SIZE];


static ssize_t read_cpuinfo(char *buf) {
    struct file *cpuinfo;
    loff_t pos = 0; 
    ssize_t result; 
я
    cpuinfo = filp_open("/proc/cpuinfo", O_RDONLY, 0);
    if (IS_ERR(cpuinfo)) {
        pr_info("filp_open error\n");
        return PTR_ERR(cpuinfo);
    }

    result = kernel_read(cpuinfo, buf, MAX_BUFFER_SIZE, &pos);

    filp_close(cpuinfo, NULL);

    return result;
}

static ssize_t my_module_read(struct file *file, char __user *buffer, size_t length, loff_t *offset) {
    ssize_t err;

    mutex_lock(&my_module_mutex);

    err = read_cpuinfo(output_buffer);
    if (err < 0) {
        mutex_unlock(&my_module_mutex);
        return err;
    }

    err = simple_read_from_buffer(buffer, length, offset, output_buffer, strlen(output_buffer));

    mutex_unlock(&my_module_mutex);

    return err;
}

static const struct proc_ops my_module_proc_ops = {
    .proc_read = my_module_read,
};

static int __init my_module_init(void) {
    proc_create("my_module", 0, NULL, &my_module_proc_ops);
    pr_info("My module has been inserted.\n");
    return 0;

static void __exit my_module_exit(void) {
    remove_proc_entry("my_module", NULL);
    pr_info("My module has been removed.\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("No license");
MODULE_AUTHOR("Fardzinov Maksim");
MODULE_DESCRIPTION("OS Lab2");
MODULE_VERSION("1");
