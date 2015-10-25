/*
   task.c
   Contains basic functionalities for task switcher implementations.
 */


#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"
#define MAX_TASKS 4
typedef struct Task_s  // structure used for storing task information
{	void (*f)(void *data); /* Task function */
	void *data; /* Private data pointer for this task */
} Task_t;


/* Process list */
Task_t tasks[MAX_TASKS];
int32_t currentTask=0; // denotes the ID of current task


int32_t clear_tasklist(){
//process block initialization
	for(int i=0; i<MAX_TASKS; i++) {
			tasks[i].f = NULL;
	}
	 currentTask=0; 
	return 0;
}

int32_t TaskAdd(void (*f)(void *), void *data){
	//Adding a Task
	/* Try to find an empty slot */
	
	if(!f){
		printf("error : NULL value for task_function \n");
		return -2;
	}
	for(int i=0; i<MAX_TASKS; i++) {
		if(tasks[i].f == NULL) {
			tasks[i].f = f;
			tasks[i].data = data;
			return i;	
		}
	}
	/* No slots available, return -1 */
	return -1;
}

int32_t TaskKill(int32_t id){
	tasks[id].f = NULL;
	tasks[id].data = NULL;
	return 0;
}


int32_t TaskCurrent(void){
	//returning current running task ID
	printf("current task is %d \n",(int )currentTask);
	return currentTask;
}


/* Find the next task to run */
static int32_t TaskNext(void)
{
	//used to find out which is the next task to be executed
	int32_t i;
	uint32_t count=0;
	i = currentTask;
	do {
		i = (i + 1) % MAX_TASKS;
		count++;
	} while((tasks[i].f == NULL)
			&& (count <= MAX_TASKS));

	return (count <= MAX_TASKS) ? i : -1;
}

int32_t TaskSwitcher(void){
	//used to switch between the tasks
	currentTask = TaskNext();
	if(currentTask < 0) {
		return -1;
	}
	printf(" task %d is running \n",(int )currentTask);
	tasks[currentTask].f(tasks[currentTask].data);
	return 0;

}

