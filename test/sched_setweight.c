#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

# define SYS_SCHED_SETWEIGHT 294
# define SYS_SCHED_GETWEIGHT 295

int main(int argc, char *argv[])
{
    if(argc != 3){
        printf("Usage: ./sched_setweight [pid] [weight]\n");
        return 0;
    }

    int pid = atoi(argv[1]);
    int weight = atoi(argv[2]);
    int result = syscall(SYS_SCHED_SETWEIGHT, pid, weight);
    int errnum = errno;

    if(result < 0){
        printf("error returned from syscall sched_setweight: %s\n", strerror(errnum));
        return -1;
    }

    return 0;
}