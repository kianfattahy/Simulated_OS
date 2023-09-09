#ifndef SHELLMEMORY_H
#define SHELLMEMORY_H


#include "pcb.h"

#define FRAME_SIZE 3

void mem_init();
char *mem_get_value(char *var);
void mem_set_value(char *var, char *value);
void printShellMemory();
void view_all_frames();
int load_page(PCB* pcb, char** page, int page_num);
int store_page_in_frame(PCB* pcb, char** page, int page_num);
PCB* load_script(char* filename);
struct memory_struct *mem_get_frame_data(int frame_num);
#endif