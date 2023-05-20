#include <linux/syscalls.h>

SYSCALL_DEFINE2(sched_setweight, pid_t, pid, unsigned int, weight){
    printk("sched_setweight called!\n");
    return 0;
}

SYSCALL_DEFINE1(sched_getweight, pid_t, pid){
    printk("sched_getweight called!\n");
    return 0;
}