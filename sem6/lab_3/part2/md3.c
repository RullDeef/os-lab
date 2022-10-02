#include "md.h"

static int __init md_init(void)
{
    printk(LOG_LVL "md3: loaded\n");

    printk(LOG_LVL "md3: data string exported from md1 : %s\n", md1_data);
    printk(LOG_LVL "md3: string returned md1_proc() is : %s\n", md1_proc());

    return -1;
}

static void __exit md_exit(void)
{
    printk(LOG_LVL "md3: unloaded\n");
}

module_init(md_init);
module_exit(md_exit);
