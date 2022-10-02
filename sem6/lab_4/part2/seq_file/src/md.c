#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/vmalloc.h>
#include "cookie.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Klimenko Alexey");

static struct proc_dir_entry *seqf_dentry;
static struct proc_dir_entry *dir_dentry;
static struct proc_dir_entry *symlink_dentry;

static struct cookie *cookie_pot = NULL;

void *my_seq_op_start(struct seq_file *m, loff_t *pos)
{
    printk(KERN_INFO "md: my_seq_op_start()");

    if (*pos >= 1)
        return NULL;
    else
    {
        const char* data = NULL;
        if (pop_cookie(&cookie_pot, &data) != 0)
            printk(KERN_WARNING "md: no cookies left!");
        return data;
    }
}

void my_seq_op_stop(struct seq_file *m, void *v)
{
    printk(KERN_INFO "md: my_seq_op_stop()");
    vfree(v);
}

void *my_seq_op_next(struct seq_file *m, void *v, loff_t *pos)
{
    printk(KERN_INFO "md: my_seq_op_next()");
    ++*pos;
    return NULL;
}

int my_seq_op_show(struct seq_file *m, void *v)
{
    printk(KERN_INFO "md: my_seq_op_show()");
    if (v != NULL)
        seq_printf(m, "%s", (char *)v);
    return 0;
}

static struct seq_operations seq_ops = {
    .start = my_seq_op_start,
    .stop = my_seq_op_stop,
    .next = my_seq_op_next,
    .show = my_seq_op_show
};

int my_proc_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "md: my_proc_open()");
    int res = seq_open(file, &seq_ops);
    if (res != 0)
        printk(KERN_ERR "md: failed to open sequence file");
    return res;
}

static ssize_t my_proc_write(struct file *file, const char __user *u, size_t cnt, loff_t *off)
{
    printk(KERN_INFO "md: my_proc_write()");

    char *buffer = vmalloc(sizeof(char) * (cnt + 1));
    memset(buffer, '\0', cnt + 1);

    if (copy_from_user(buffer, u, cnt) == -1)
        printk(KERN_ERR "md: bad copy_from_user()");
    else
    {
        push_cookie(&cookie_pot, buffer);
        printk(KERN_INFO "md: writed new fortune");
    }

    return cnt;
}

static int my_seq_release(struct inode* inode, struct file* file)
{
    printk(KERN_INFO "md: my_seq_release()");
    return seq_release(inode, file);
}

static struct proc_ops seq_fops = {
    .proc_open = my_proc_open,
    .proc_read = seq_read,
    .proc_write = my_proc_write,
    .proc_lseek = seq_lseek,
    .proc_release = my_seq_release
};

static int __init md_init(void)
{
    dir_dentry = proc_mkdir("fortune_dir", NULL);
    seqf_dentry = proc_create("fortune", S_IRWXUGO, dir_dentry, &seq_fops);
    symlink_dentry = proc_symlink("fortune_symlink", dir_dentry, "fortune");

    if (seqf_dentry == NULL)
    {
        printk(KERN_ERR "md: seqf_dentry was not created");
        return -1;
    }

    printk(KERN_INFO "md: module loaded");
    return 0;
}

static void md_exit(void)
{
    free_cookies(&cookie_pot);
    proc_remove(seqf_dentry);
    printk(KERN_INFO "md: module unloaded");
}

module_init(md_init);
module_exit(md_exit);
