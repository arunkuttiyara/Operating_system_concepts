#include<stdio.h>
#include<string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"

/* Timer flags */
#define TIMER_ALLOCATED (1<<0)
#define TIMER_COUNTING (1<<1)
#define NUM_SWTIMERS 5

/* timer structure */
typedef struct Timer_s {
	uint32_t flags;
	uint32_t count;
	void *data;
	void (*callback)(void *);
	char *name;
} Timer_t;
static Timer_t timers[NUM_SWTIMERS];

void SWTimerInit(void);
uint32_t SWTimerAllocate(uint32_t *handle, void (*callback)(void *),void *data, char *name);
uint32_t SWTimerFree(uint32_t handle);
uint32_t SWTimerSet(uint32_t handle, uint32_t count);
uint32_t SWTimerSetData(uint32_t handle, void *data);
void SWTimerTick(void);
void DumpTimers();

/* initialization of timers */
void SWTimerInit(void){
	for(int i=0;i<NUM_SWTIMERS;i++)
		memset(&timers[i],0,sizeof(timers[i]));
}

/* Timer Allocation */
uint32_t SWTimerAllocate(uint32_t *handle, void (*callback)(void *),void *data, char *name){

	for(int i=0;i<NUM_SWTIMERS;i++){
		if(timers[i].flags==0){
			*handle=i;
			timers[i].flags = TIMER_ALLOCATED;	
			timers[i].callback = callback;	
			timers[i].name = name;	
			return 0;
		}	
	}
	return -1;
}
/* Frreing a Timer */
uint32_t SWTimerFree(uint32_t handle){
	if(handle >=0 && handle<=NUM_SWTIMERS)	{
		memset(&timers[handle],0,sizeof(timers[handle]));
		return 0;
	}
	else return 1;
}
/* setting count value to a tmer */
uint32_t SWTimerSet(uint32_t handle, uint32_t count){
	if(!count){
		printf("error \n");
		timers[handle].flags=TIMER_ALLOCATED;
		return 0;
	}	
	if(count>=0 && handle<=NUM_SWTIMERS && timers[handle].flags==TIMER_ALLOCATED){
		timers[handle].count=count;	
		timers[handle].flags=TIMER_COUNTING; 
		return 0;
	}
	else return -1;
}
/* setting data field associated with a timer  */
uint32_t SWTimerSetData(uint32_t handle, void *data){
	if(handle>=0 && handle<=NUM_SWTIMERS && timers[handle].flags==TIMER_ALLOCATED) {
		timers[handle].data = data; 

	}
	else return -1;
	return 0;	
}
/* called from main at every 1 sec 
   keeps the count bupdated for all timers 
 */
void SWTimerTick(void){
	for(int i=0;i<NUM_SWTIMERS;i++){
		if(timers[i].flags==TIMER_COUNTING && timers[i].count!=0)
			timers[i].count--;	
		if(timers[i].count==0){
			if(timers[i].callback!=NULL){
				(*timers[i].callback)((timers[i].data));
				SWTimerFree(i);
			}
		}	
	}		
}
/* Dump the count information */
void DumpTimers(){
	for(int i=0;i<NUM_SWTIMERS;i++){
		printf("timers[%d] count = %d \n",(int )i,(int )timers[i].count);
		printf("timers[%d] flag = %d \n",(int )i,(int )timers[i].flags);
		printf("timers[%d] name = %s \n",(int )i,timers[i].name);
	}
}
