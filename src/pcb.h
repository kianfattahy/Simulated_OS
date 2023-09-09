#ifndef PCB_H
#define PCB_H
#include <stdbool.h>
#include "memory_struct.h"
/*
 * Struct:  PCB 
 * --------------------
 * pid: process(task) id
 * PC: program counter, stores the index of line that the task is executing
 * start: the first line in shell memory that belongs to this task
 * end: the last line in shell memory that belongs to this task
 * job_length_score: for EXEC AGING use only, stores the job length score
 */
typedef struct
{
    bool priority;
    int pid;
    struct memory_struct *pc_;
    int current_page;
    int num_pages;
    int size; 
    int* page_table;
    char* script_name;
    int job_length_score;
    bool did_start;

    int PC;
    int start;
    int end;
}PCB;

int generatePID();
PCB* makePCB(char* filename);
#endif