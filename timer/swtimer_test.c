#include<stdio.h>
#include<string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"

extern void SWTimerInit();
extern uint32_t SWTimerAllocate(uint32_t *,void (*)(void *),void *, char *);
extern void DumpTimers();
uint32_t SWTimerSet(uint32_t , uint32_t );
uint32_t SWTimerSetData(uint32_t,uint32_t *);
/* handle got from timer allocation function */
uint32_t handle;
/* Array to hold the timer datas */
uint32_t data[5];
/* call back function for timers */
void callback_test1(void *data_new){
	printf("timer with count %d completed  \n",(*(int *)data_new));
}
/* command for timer initialization */
void cmd_timer_init(int mode){
	if (mode!=CMD_INTERACTIVE) return;
	SWTimerInit();
}
ADD_CMD("timer_init",cmd_timer_init,"    initialize timer values")
/* command to set a count for a timer */
void cmd_set_timer(int mode){
	if (mode!=CMD_INTERACTIVE) return;
	uint32_t count=0;
	fetch_uint32_arg(&count);
	if(!count) { printf("enter a valid a count \n"); return;}	
	if(-1==SWTimerAllocate((uint32_t *)&handle,callback_test1,&data,"test1"))
		printf("No time slots left \n"); 
	else {//	fetch_uint32_arg(&data[handle]);
		data[handle]=count;
		SWTimerSetData(handle,&data[handle]);	}

	if(-1==SWTimerSet(handle,count)){
		printf("unable to set software timer \n");
		return;
	}
}
ADD_CMD("set_timer",cmd_set_timer,"    swicthing the tasks ")
/* command to dump the counter values */
void CmdDumpTimers(int mode){
	DumpTimers();		
}
ADD_CMD("dump_timer",CmdDumpTimers,"    swicthing the tasks ")

