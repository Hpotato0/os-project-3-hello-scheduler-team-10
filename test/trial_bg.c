#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> // isdigit
#include <string.h>
#include <errno.h>
#include <time.h>

# define SYS_SCHED_SETWEIGHT 294
# define SYS_SCHED_GETWEIGHT 295


void factorize(int num){
    // printf("Factorizing %d: ", num);

    for(int p = 2; num > 1; p++){
        while(num % p == 0){
            // printf("%d, ", p);
            num = num / p;
        }
    }

    // printf("DONE\n");
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Usage: ./trial_bg [self_weight]\n");
        return 0;
    }
    if(!isdigit(*argv[1])){
        printf("Argument should be a number\n");
        return 0;
    }
    int weight = atoi(argv[1]);
    int result = syscall(SYS_SCHED_SETWEIGHT, getpid(), weight);
    int errnum = errno;

    if(result < 0){
        printf("error returned from syscall sched_setweight: %s\n", strerror(errnum));
        return -1;
    }

    while(1){
        factorize(54128);
    }

    return 0;
}
