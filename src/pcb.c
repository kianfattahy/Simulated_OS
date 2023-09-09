#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pcb.h"

int pid_counter = 1;

int generatePID(){
    return pid_counter++;
}

//In this implementation, Pid is the same as file ID 
PCB* makePCB(char* filename){
    PCB * newPCB = malloc(sizeof(PCB));
    char* dir = "backingstore/";
	char* filepath = malloc(strlen(filename) + strlen(dir) + 1);
	sprintf(filepath, "%s%s",dir,filename);
    newPCB->script_name = filepath;
    newPCB->pid = generatePID();
    newPCB->priority = false;
    return newPCB;
}