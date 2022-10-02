#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/irq.h>
#include "keyb_layout.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Klimenko Alexey");

#define KEYBOARD_IRQ 1
#define KBD_DATA_BUF_SIZE 1024
#define PRNT_PREFIX KERN_INFO "intmod: "

static struct my_tasklet_data
{
    char buf[KBD_DATA_BUF_SIZE];
    int index;
} my_data;

static unsigned char scancode;

void my_tasklet_function(struct tasklet_struct *tasklet)
{
    printk(PRNT_PREFIX "tasklet working... state = %d\n", tasklet->state);

    char symbol = scancode_to_ascii(scancode);
    if (symbol != '\0')
    {
        my_data.buf[my_data.index++] = symbol;
        printk(PRNT_PREFIX "symbol: \'%c\'\n", symbol);
    }
}

DECLARE_TASKLET(my_tasklet, my_tasklet_function);

irqreturn_t my_irq_handler(int irq, void* dev_id)
{
	scancode = inb(0x60);

    if (scancode & 0x80)
    {
        printk(PRNT_PREFIX "keyboard interrupt\n");
        printk(PRNT_PREFIX "tasklet state before schedule = %d\n", my_tasklet.state);
        tasklet_schedule(&my_tasklet);
        printk(PRNT_PREFIX "tasklet state after schedule = %d\n", my_tasklet.state);
    }
 
    return IRQ_HANDLED;
}

static int __init md_init(void)
{
    printk(PRNT_PREFIX "init\n");

    if (request_irq(KEYBOARD_IRQ, my_irq_handler, IRQF_SHARED, "my softirq", my_irq_handler) == -1)
    {
        printk(PRNT_PREFIX "could not register irq handler\n");
        return -1;
    }

    return 0;
}

static void __exit md_exit(void)
{
    printk(PRNT_PREFIX "exit\n");

    free_irq(KEYBOARD_IRQ, my_irq_handler);
    tasklet_kill(&my_tasklet);
}

module_init(md_init);
module_exit(md_exit);
