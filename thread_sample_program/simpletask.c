#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"

static uint32_t counter=0;
void taskcounter(void)
{
if(counter!=0)
{
printf("the current counter value is  %d \n",(int)counter);
counter--;
}
}

void cmdcount(int mode)
{
uint32_t count_new=0;
if (mode!=CMD_INTERACTIVE) return;
fetch_uint32_arg(&count_new);
counter=counter+count_new;
}
ADD_CMD("count",cmdcount,"          type your count value")


