#include <linux/module.h>
#include <linux/sched.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/time.h>
#include <linux/delay.h>
#include "keyb_layout.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Klimenko Alexey");

#define PRNT_PREFIX KERN_INFO "wormod: "
#define KEYBOARD_IRQ 1

struct workqueue_struct* my_wqueue;
struct work_struct my_work_1;
struct work_struct my_work_2;

unsigned char scancode;

static void my_work_func_1(struct work_struct *work)
{
    struct tm curr_tm;
    time64_to_tm(ktime_get_real_seconds(), 0, &curr_tm);
    printk(PRNT_PREFIX "work 1 sleeping...\n");
    msleep(500);
    printk(PRNT_PREFIX "work 1 time: %d:%d:%d\n", curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec);
}

static void my_work_func_2(struct work_struct *work)
{
    printk(PRNT_PREFIX "work 2 doing: %c\n", scancode_to_ascii(scancode));
}

irqreturn_t my_irq_handler(int irq, void* dev_id)
{
	scancode = inb(0x60);

    if (scancode & 0x80)
    {
        queue_work(my_wqueue, &my_work_2);
        if (scancode_to_ascii(scancode) == 0)
            queue_work(my_wqueue, &my_work_1);
    }

    return IRQ_HANDLED;
}

static int __init md_init(void)
{
    printk(PRNT_PREFIX "init\n");
    if (request_irq(KEYBOARD_IRQ, my_irq_handler, IRQF_SHARED, "my worker handler", my_irq_handler) == -1)
    {
        printk(PRNT_PREFIX "request_irq error\n");
        return -1;
    }

    my_wqueue = create_workqueue("myqueue");
    if (my_wqueue == NULL)
    {
        printk(PRNT_PREFIX "alloc_workqueue error\n");
        free_irq(KEYBOARD_IRQ, my_irq_handler);
        return -1;
    }

    INIT_WORK(&my_work_1, my_work_func_1);
    INIT_WORK(&my_work_2, my_work_func_2);

    return 0;
}

static void __exit md_exit(void)
{
    printk(PRNT_PREFIX "exit\n");

    free_irq(KEYBOARD_IRQ, my_irq_handler);
    flush_workqueue(my_wqueue);
    destroy_workqueue(my_wqueue);
}

module_init(md_init);
module_exit(md_exit);
