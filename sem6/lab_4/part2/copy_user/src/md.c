#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include "cookie.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Klimenko Alexey");

#define PROC_FILE_NAME      "fortune"
#define PROC_SYMLINK_NAME   "fortune_symlink"
#define PROC_DIR_NAME       "fortune_dir"

static struct proc_dir_entry *dir_dentry;
static struct proc_dir_entry *symlink_dentry;
static struct proc_dir_entry *fortune_dentry;

static struct cookie *cookie_pot = NULL;
static bool readed = false;

static int my_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "md: my_open()");
    readed = false;
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "md: my_release()");
    return 0;
}

static ssize_t my_read(struct file *f, char __user *u, size_t cnt, loff_t *off)
{
    if (readed)
        return 0;
    readed = true;
    const char *buffer = NULL;
    if (pop_cookie(&cookie_pot, &buffer))
    {
        printk(KERN_INFO "md: no cookies left.");
        return 0;
    }
    else if (copy_to_user(u, buffer, strlen(buffer) + 1))
    {
        printk(KERN_INFO "md: failed copy_to_user!");
        return -1;
    }

    printk(KERN_INFO "md: my_read() readed: %s ", buffer);
    return strlen(buffer) + 1;
}

static ssize_t my_write(struct file *f, const char __user *u, size_t cnt, loff_t *off)
{
    char *buffer = vmalloc(sizeof(char) * (cnt + 1));
    memset(buffer, '\0', cnt + 1);
    if (copy_from_user(buffer, u, cnt) != 0)
    {
        printk(KERN_INFO "md: failed copy_from_user!");
        return -1;
    }

    push_cookie(&cookie_pot, buffer);

    printk(KERN_INFO "md: my_write() wroted: %s ", buffer);
    return cnt;
}

static struct proc_ops ops = {
    .proc_open = my_open,
    .proc_read = my_read,
    .proc_write = my_write,
    .proc_release = my_release,
}; 

static int md_init(void)
{
    printk(KERN_INFO "md: module loaded.");

    dir_dentry = proc_mkdir(PROC_DIR_NAME, NULL);
    symlink_dentry = proc_symlink(PROC_SYMLINK_NAME, dir_dentry, PROC_FILE_NAME);
    fortune_dentry = proc_create(PROC_FILE_NAME, S_IRUGO | S_IWUGO, dir_dentry, &ops);

    if (fortune_dentry == NULL)
    {
        printk(KERN_ERR "md: fortune_dentry was not created.");
        return -1;
    }

    return 0;
}

static void md_exit(void)
{
    free_cookies(&cookie_pot);
    proc_remove(symlink_dentry);
    proc_remove(dir_dentry);
    proc_remove(fortune_dentry);

    printk(KERN_INFO "md: module unloaded.");
}

module_init(md_init);
module_exit(md_exit);
