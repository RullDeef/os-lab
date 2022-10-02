#include <linux/init_task.h>
#include <linux/module.h>

#define LOG_LVL KERN_INFO

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Klimenko Alexey");

void print_task_info(struct task_struct* task)
{
    printk(LOG_LVL "md: pid=%05d, name=%16s, state=%04d, prio=%03d, ppid=%05d, name_p=%16s\n",
        task->pid, task->comm, task->__state, task->prio, task->parent->pid, task->parent->comm);
}

static int __init md_init(void)
{
    struct task_struct *task = &init_task;

    printk(LOG_LVL "md: loaded\n");

    do
    {
        print_task_info(task);
    } while ((task = next_task(task)) != &init_task);

    printk(LOG_LVL "md: current task:\n");
    print_task_info(current);

    return 0;
}

static void __exit md_exit(void)
{
    printk(LOG_LVL "md: current task:\n");
    print_task_info(current);

    printk(LOG_LVL "md: unloaded\n");
}

module_init(md_init);
module_exit(md_exit);
