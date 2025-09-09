#include "app_common.h"
#include <stdio.h>

int main()
{
    //get_time
    long cur_ms = app_common_getCurrentTimestamp();
    printf("current millisecons timestamp is: %ld\n", cur_ms);
}