#include <linux/init_task.h>
#include "md.h"

// надо проделать след действия:
// в модуле 2 вызвать локальную функцию, получить ошибку
// вызвать no_export как у Цирюлика (анализировать ошибку)
// no_export прописать в header как extern и опять вызвать в модуле 2 получить ошибку, анализировать на каком этапе
// вызвать функции в модуле 3, коментировать ошибку
// выкинуть все, оставить правильные вызывы, попытаться загрузить сначала модуль 2, потом модуль 1

char* md1_data = "abc"; 

extern char* md1_proc(void)
{
    return md1_data; 
}

static char* md1_local(void)
{
    return md1_data; 
}

extern char* md1_noexport(void)
{
    return md1_data; 
} 

EXPORT_SYMBOL( md1_data ); 
EXPORT_SYMBOL( md1_proc ); 

static int __init md_init(void)
{
    printk(LOG_LVL "md1: loaded\n");
    return 0;
}

static void __exit md_exit(void)
{
    printk(LOG_LVL "md1: unloaded\n");
}

module_init(md_init);
module_exit(md_exit);
