#ifndef __MD_H__
#define __MD_H__

#include <linux/module.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Klimenko Alexey");

#define LOG_LVL KERN_INFO

extern char* md1_data; 
extern char* md1_proc(void);

extern char* md1_noexport(void);

#endif /* __MD_H__ */
