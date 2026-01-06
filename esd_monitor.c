
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "esd_monitor"
#define CLASS_NAME  "esd"

static dev_t dev_num;
static struct cdev esd_cdev;
static struct class *esd_class;

/* Simulated states */
static int ir_present = 1;
static int wristband_connected = 1;

static ssize_t esd_read(struct file *file, char __user *buf,
                        size_t len, loff_t *offset)
{
    char msg[128];
    int status_safe;

    if (*offset > 0)
        return 0;

    status_safe = ir_present && wristband_connected;

    snprintf(msg, sizeof(msg),
             "IR=%d\nWRISTBAND=%s\nSTATUS=%s\n",
             ir_present,
             wristband_connected ? "CONNECTED" : "DISCONNECTED",
             status_safe ? "SAFE" : "UNSAFE");

    if (copy_to_user(buf, msg, strlen(msg)))
        return -EFAULT;

    *offset = strlen(msg);
    return *offset;
}

static ssize_t esd_write(struct file *file, const char __user *buf,
                         size_t len, loff_t *offset)
{
    char cmd[32];

    if (len > sizeof(cmd) - 1)
        return -EINVAL;

    if (copy_from_user(cmd, buf, len))
        return -EFAULT;

    cmd[len] = '\0';

    if (strncmp(cmd, "ir=0", 4) == 0)
        ir_present = 0;
    else if (strncmp(cmd, "ir=1", 4) == 0)
        ir_present = 1;
    else if (strncmp(cmd, "wrist=0", 7) == 0)
        wristband_connected = 0;
    else if (strncmp(cmd, "wrist=1", 7) == 0)
        wristband_connected = 1;

    return len;
}

static int esd_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int esd_release(struct inode *inode, struct file *file)
{
    return 0;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = esd_open,
    .read    = esd_read,
    .write   = esd_write,
    .release = esd_release,
};

static int __init esd_init(void)
{
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    cdev_init(&esd_cdev, &fops);
    cdev_add(&esd_cdev, dev_num, 1);

    esd_class = class_create(THIS_MODULE, CLASS_NAME);
    device_create(esd_class, NULL, dev_num, NULL, DEVICE_NAME);

    pr_info("ESD monitor driver loaded\n");
    return 0;
}

static void __exit esd_exit(void)
{
    device_destroy(esd_class, dev_num);
    class_destroy(esd_class);
    cdev_del(&esd_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("ESD monitor driver unloaded\n");
}

module_init(esd_init);
module_exit(esd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vamsi Krishna");
MODULE_DESCRIPTION("Linux character driver for ESD wristband monitoring");
