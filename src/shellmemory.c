#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>

#include "pcb.h"
#include "frame.h"
#include "ready_queue.h"
#include "shellmemory.h"
#include "memory_struct.h"


int FREE_LIST_SIZE = (int) (FRAMEMEMSIZE/3); 

struct memory_struct shellmemory[VARMEMSIZE];
struct memory_struct framememory[FRAMEMEMSIZE];

frame_t free_list[(int) (FRAMEMEMSIZE/3)];

// Helper functions
int match(char *model, char *var) {
	int i, len=strlen(var), matchCount=0;
	for(i=0;i<len;i++)
		if (*(model+i) == *(var+i)) matchCount++;
	if (matchCount == len)
		return 1;
	else
		return 0;
}

char *extract(char *model) {
	char token='=';    // look for this to find value
	char value[1000];  // stores the extract value
	int i,j, len=strlen(model);
	for(i=0;i<len && *(model+i)!=token;i++); // loop till we get there
	// extract the value
	for(i=i+1,j=0;i<len;i++,j++) value[j]=*(model+i);
	value[j]='\0';
	return strdup(value);
}


// Shell memory functions

void mem_init(){
	int i;
	for (i=0; i<VARMEMSIZE; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}

	for (i=0; i<FRAMEMEMSIZE; i++){		
		framememory[i].var = "none";
		framememory[i].value = "none";
	}


	for (i = 0; i < FREE_LIST_SIZE; i ++) {
		free_list[i].avail = 1;
		free_list[i].age = 0;
		free_list[i].pcb = NULL;
	}
}

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {
	int i;
	for (i=0; i<VARMEMSIZE; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<VARMEMSIZE; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

struct memory_struct *mem_get_frame_data(int frame_num) {
	// printf("1\n");
	//reset age of accessed memory

	// printf("frame num %d\n", frame_num);
	free_list[frame_num].age = 0;
	// printf("2\n");


	for(int i = 0; i < FREE_LIST_SIZE; i++) {
		// printf("%d\n", (3+i));

		if(i == frame_num || free_list[i].avail) continue;
		free_list[i].age++;
	}
	// printf("4\n");

	return &framememory[frame_num*FRAME_SIZE];
}



int load_page(PCB* pcb, char** page, int page_num) {
	// printf("load page\n");

	FILE* fs = fopen(pcb->script_name, "rt");
	char line[100];

	int start_idx = 0;
	while(start_idx < (page_num * FRAME_SIZE) && fgets(line,100,fs)) {
		start_idx++;
	}


	// fill page
	for(int i = 0; i < FRAME_SIZE; i++) {
		if(fgets(line,100,fs)) {
			page[i] = strdup(line);
			//printf("\nwe are at line: %s\n", line);
		}
		else page[i] = "none";
	}

	fclose(fs);
	return 0;
}
bool check_init(PCB *pcb, int idx){ return free_list[idx].age == FRAME_SIZE*5+1 && idx;}
void print_evicted_frame(int frame_num) {
	//view_all_frames();
	//view age of every frame
	for (int i = 0; i < 6; i++){
		//printf("\nAGE OF FRAME %d: %d\n",i, free_list[i].age);
	}

	char* line_evicted = (mem_get_frame_data(frame_num))->value;
	if (strcmp(line_evicted,"none") == 0) return;

	printf("Page fault! Victim page contents:\n\n");
	for (int i = 0; i < FRAME_SIZE; i++) {
		char* line_evicted = (mem_get_frame_data(frame_num)+i)->value;
		
		int length = strlen(line_evicted);
		if (line_evicted[length - 1] == '\n') {
			line_evicted[length - 1] = '\0';
		}
			
		printf("%s\n",line_evicted);
	}
	printf("\nEnd of victim page contents.\n");
}

int mem_clear_frame(int frame_num){
	struct memory_struct *first_entry = mem_get_frame_data(frame_num);

	for(int i = 0; i < FRAME_SIZE; i++) {
		struct memory_struct *entry = first_entry + i;
		entry->value = "none";
		entry->var = "none";
	}
	
	free_list[frame_num].age = 0;
	free_list[frame_num].avail = true;
	free_list[frame_num].pcb = NULL;
	

	return 0;
} 

int evict_pcb(PCB* pcb, char** page) {
	//LRU
	int max_age = -1;
	int frame_num = 0;
	for(int i = 0; i < FREE_LIST_SIZE; i++) {
		if(free_list[i].age > max_age)  {
			if(check_init(pcb,i)) continue;
			max_age = free_list[i].age;
			frame_num = i;
		}
	}

	// update pcb page table for evicted frame
	PCB* pcb_out = free_list[frame_num].pcb;
	for(int i = 0; i < pcb->num_pages; i++) {
		if(pcb_out->page_table[i] == frame_num) {
			pcb_out->page_table[i] = -1;
			break;
		}
	}

	print_evicted_frame(frame_num);
	mem_clear_frame(frame_num);
	return frame_num;
}


int update_framemem(PCB* pcb, char** page, int frame_num) {
	// printf("update frame mem\n");

	// printf("getting frame %d\n", frame_num);
	struct memory_struct *entry = mem_get_frame_data(frame_num);

	// printf("entry val = %s\n",entry->value);
	char key[50];
	//printf("\nupdating frame number %d\n", frame_num);
	for(int i = 0; i < FRAME_SIZE; i++, entry++) {
		//printf("line is: %s\n", page[i]);
		sprintf(key, "%s",pcb->script_name);
		entry->value = strdup(page[i]);
		if(strcmp(page[i],"none")) entry->var = "none";
		else entry->var = strdup(key);
	}
	// free(page);
	
	return 0;
}

int store_page_in_frame(PCB* pcb, char** page, int page_num) {
	// printf("store_page_in_frame\n");

	//check there is space
	bool frame_avail = false;
	int avail_frame_idx;
	for(int i = 0; i < FREE_LIST_SIZE; i++){
		//printf("is avail var: %d\n", free_list[i].avail);
		if(free_list[i].avail) {
			avail_frame_idx = i;
			frame_avail = true; 
			break;
		}
	}


	// evict if needed
	if(!frame_avail) avail_frame_idx = evict_pcb(pcb, page);

	// update frame mem
	update_framemem(pcb, page, avail_frame_idx);

	// update page table
	pcb->page_table[page_num] = avail_frame_idx;
	
	// update free list
	free_list[avail_frame_idx].avail = 0;
	//printf("oh shit we just made %dth frame full\n",avail_frame_idx);
	for (int i = 0; i < 3;i++){
		//printf("line: %s\n",framememory[avail_frame_idx * 3 + i].value);
	}

	free_list[avail_frame_idx].pcb = pcb;

	return 0;
}

void prep_pcb_init(FILE* fs, PCB* pcb) {
	char line[100];

	pcb->size = 0;
	while(fgets(line, 100, fs)) {
		pcb->size++;
	}

	pcb->job_length_score = pcb->size;
	if(pcb->size%FRAME_SIZE == 0) {
		pcb->num_pages = pcb->size/FRAME_SIZE;
	} else {
		pcb->num_pages = (int) (pcb->size/FRAME_SIZE) + 1;
	}
	pcb->page_table = calloc(pcb->num_pages, sizeof(int));

	for(int i = 0; i < pcb->num_pages; i++) {
		pcb->page_table[i] = -1;
	}
	pcb->current_page = 0;
	pcb->pc_ = NULL;
	pcb->did_start = false; 
}

PCB* load_script(char* filename) {
	PCB* pcb = makePCB(filename);
	FILE* fs = fopen(pcb->script_name, "rt");
	prep_pcb_init(fs, pcb);

	rewind(fs);

	char* page[FRAME_SIZE];
	for(int i = 0; i < 2; i++){
		if(i > pcb->num_pages) break;
		load_page(pcb, page, i);
		store_page_in_frame(pcb,page,i);
	}
	fclose(fs);
	return pcb;
}

//get value based on input key
char *mem_get_value(char *var_in) {
	int i;
	for (i=0; i<VARMEMSIZE; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			return strdup(shellmemory[i].value);
		} 
	}
	return NULL;

}

void view_all_frames(){
	printf("-----------------------------------\n");
	for (int i = VARMEMSIZE; i < VARMEMSIZE + FRAMEMEMSIZE; i++){
		if (i%3 == 1){
			printf("\n");
			
		}
		int length = strlen(shellmemory[i].value);
        if (shellmemory[i].value[length - 1] == '\n') {
            shellmemory[i].value[length - 1] = '\0'; // Replace the newline character with a null terminator
        }
		
		printf("=%s\n",shellmemory[i].value);

	}
	printf("-----------------------------------\n");
}