/*
   tasktest.c
   contains command implementation for "task_switch".
   adds certain number of tasks [ which can be modified],
   and calles task_switcher function which works on Co-operative 
   mutlitasking concepts.

 */

#include <stdio.h>
#include <string.h>
#include "stm32f3xx_hal.h"
#include "stm32f3_discovery.h"
#include "common.h"

/* sample task for printing one string */
void taskB(void *data){
	while(1){
		printf(" AAAA \n");
		/* call it if you want to dumb the PCB */
		//TaskDump(TaskCurrent());
		TaskSwitcher();
	}
}

/* Example for different states and illustrates the process of task switching between different tasks*/
void taskA(void *data){

	while(1){
		printf(" step 1 accomplished \n");
		TaskSwitcher();
		/* call it if you want to dumb the PCB */
		//TaskDump(TaskCurrent());
		printf(" step 2 accomplished \n");
		TaskSwitcher();
		printf(" step 3 accomplished \n");
		TaskSwitcher();
		printf(" step 4 accomplished and killing task2 \n");
		TaskKill(2); /* killing the second task so that no more 2nd task wont be executed further */
		TaskSwitcher();
		printf(" step 5 accomplished \n");
		TaskSwitcher();
		printf(" step 6 accomplished \n");
		TaskSwitcher();
		TaskKill(1);
		TaskSwitcher();
	}
}

void CmdTask(int mode){

	if (mode!=CMD_INTERACTIVE) return;

	int a=11,b=22;
	clear_tasklist();
	TaskAdd(taskA,(void*)a);
	TaskAdd(taskB,(void*)b);
	printf("task switcher is going to be called \n");
	TaskSwitcher();
}
ADD_CMD("task_switch",CmdTask,"          swicthing the tasks ")
