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
    if(argc != 2){
        printf("Usage: ./sched_getweight [pid]\n");
        return 0;
    }

    int pid = atoi(argv[1]);
    int result = syscall(SYS_SCHED_GETWEIGHT, pid);
    int errnum = errno;

    if(result < 0){
        printf("error returned from syscall sched_setweight: %s\n", strerror(errnum));
        return -1;
    }
    printf("weight: %d\n", result);

    return 0;
}