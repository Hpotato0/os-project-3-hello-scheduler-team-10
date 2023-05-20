#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

# define SYS_SCHED_SETWEIGHT 294
# define SYS_SCHED_GETWEIGHT 295

int main(int argc, char *argv[])
{
    syscall(SYS_SCHED_SETWEIGHT, 0, 0);
    syscall(SYS_SCHED_GETWEIGHT, 0);
    return 0;
}