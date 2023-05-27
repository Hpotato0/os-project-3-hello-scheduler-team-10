// for 'purifying' the outputs of '/proc/sched_debug'
// to get only the stats related to wrr_rq
// partially generated by chatGPT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    FILE* sched_debug_fp = fopen("/proc/sched_debug", "r");
    if (sched_debug_fp == NULL) {
        perror("fopen");
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int found = 0;
    while (fgets(buffer, BUFFER_SIZE, sched_debug_fp) != NULL) {
        // Parse the buffer to extract the loads of wrr_rq
        if(found == 0){
            char* load_start = strstr(buffer, "wrr_rq");
            if(load_start != NULL){
                found = 1;
                //printf("%s", buffer);
            }
        }
        else{
            char* load_start = strstr(buffer, ".load");
            if(load_start != NULL){
                found = 0;

                int load;
                sscanf(load_start + strlen(".load                          : "), "%d", &load);
                printf("%d ", load);
            }
        }
    }
    printf("\n");

    if (fclose(sched_debug_fp) != 0) {
        perror("fclose");
        exit(1);
    }

    return 0;
}