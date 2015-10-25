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
#define TASK_STACK_SIZE 0x100

/* Space for process stacks. */
uint32_t stacks [MAX_TASKS][TASK_STACK_SIZE];

/* for co-operative */
typedef struct PCB_s {
	/* Storage for r4-r14 */
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t sl;
	uint32_t fp;
	uint32_t ip;
	uint32_t lr;
	uint32_t sp; /* Note location...*/

} PCB_t;

typedef struct Task_s  // structure used for storing task information
{	void (*f)(void *data); /* Task function */
	void *data; /* Private data pointer for this task */
	uint32_t *stack; /* Pointer to top of task's stack */
	PCB_t pcb; /* Process control block */
} Task_t;

/* Process list */
Task_t tasks[MAX_TASKS];
int32_t currentTask=0; // denotes the ID of current task

/**Function defenitions **/
__attribute__((naked)) static void TaskShell(void)
{
	/* call the appropriate Task Function */
	tasks[currentTask].f(tasks[currentTask].data);
	/* Terminate this task */
	TaskKill(currentTask);
	/* Call scheduler, Never returns */
	TaskSwitcher();
}

int32_t clear_tasklist(){
	//process block initialization
	for(int i=0; i<MAX_TASKS; i++) {
		tasks[i].f = NULL;
	}
	currentTask=0; 
	return 0;
}

int32_t TaskAdd(void (*f)(void *data), void *data){
	/* Adding a Task */
	/* Try to find an empty slot */

	if(!f){
		printf("error : NULL value for task_function \n");
		return -2;
	}
	// we are starting from 1 st position , because zeroth task is reserved for monitor task.
	for(int i=1; i<MAX_TASKS; i++) {
		if(tasks[i].f == NULL) {
			tasks[i].f = f;
			tasks[i].data = data;
			//PCB initialization
			memset(&(tasks[i].pcb),0,sizeof(tasks[i].pcb));
			/*In the LAB PC, we need to decrement the allocated sp size by 6 */
			//tasks[i].pcb.sp = (uint32_t)(&(stacks[i][TASK_STACK_SIZE-6]));
			//tasks[i].pcb.fp = (uint32_t)(&(stacks[i][TASK_STACK_SIZE-6]));
			/*In the LAB PC, we need to decrement the allocated sp size by 4 */
			tasks[i].pcb.sp = (uint32_t)(&(stacks[i][TASK_STACK_SIZE-4]));
			tasks[i].pcb.fp = (uint32_t)(&(stacks[i][TASK_STACK_SIZE-4]));
			tasks[i].pcb.lr = (uint32_t)TaskShell;
			/* Put an initial stack frame on too */
			stacks[i][TASK_STACK_SIZE-1] = (uint32_t)(TaskShell);
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
		i = ((i + 1) % (MAX_TASKS-1))+1;
		count++;
	} while((tasks[i].f == NULL)
			&& (count <= MAX_TASKS));

	return (count <= MAX_TASKS) ? i : -1;
}

int32_t TaskSwitcher(void)
{
	int32_t nextTask;
	register uint32_t *currentPCB asm("r0");
	register uint32_t *nextPCB asm("r1");

	nextTask = TaskNext();
	if(nextTask < 0) {
		/* In the case of no tasks to run,
		   return to the original thread */
		printf("TaskSwitcher(): "
				"No tasks to run!\n");
		nextTask = 0;
	}

	/* If the current task is the only one
	   to be running, just return */
	if(nextTask == currentTask){
		return 0;
	}
	currentPCB = &(tasks[currentTask].pcb.r4);
	nextPCB = &(tasks[nextTask].pcb.r4);
	currentTask = nextTask;

	/* NOTE : for the first time, currentTask being zero, the details of current program, that's the monitor program details 
	   will be stored into tasks[0]. And also note that we are reserving zeroth position for monitor program. 
	   So at the end, if there are no more tasks to be executed, our program will switch to
	   monitor program */

	asm volatile ("stm %[current]!,{r4-r12,r14}\n\t"
			"str sp, [%[current]]\n\t"
			"ldm %[next]!,{r4-r12,r14}\n\t"
			"ldr sp, [%[next]]\n\t"
			: /* No Outputs */
			: [current] "r" (currentPCB), [next] "r"
			(nextPCB) /* Inputs */
			: /* 'No' Clobbers */);
	/* We are now on the other context */

	return 0;
}
/* Dump the PCB at the given ID */
void TaskDump(int32_t taskid){
	printf("PCB dump is \n");
	printf("	r4 is %u \n",(unsigned int)tasks[taskid].pcb.r4);
	printf("	r5 is %u \n",(unsigned int)tasks[taskid].pcb.r5);
	printf("	r6 is %u \n",(unsigned int)tasks[taskid].pcb.r6);
	printf("	r7 is %u \n",(unsigned int)tasks[taskid].pcb.r7);
	printf("	r8 is %u \n",(unsigned int)tasks[taskid].pcb.r8);
	printf("	r9 is %u \n",(unsigned int)tasks[taskid].pcb.r9);
	printf("	sl is %u \n",(unsigned int)tasks[taskid].pcb.sl);
	printf("	fp is %u \n",(unsigned int)tasks[taskid].pcb.fp);
	printf("	ip is %u \n",(unsigned int)tasks[taskid].pcb.ip);
	printf("	lr is %u \n",(unsigned int)tasks[taskid].pcb.lr);
	printf("	sp is %u \n",(unsigned int)tasks[taskid].pcb.sp);
}
