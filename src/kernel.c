#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "pcb.h"
#include "kernel.h"
#include "shell.h"
#include "shellmemory.h"
#include "interpreter.h"
#include "ready_queue.h"


bool multi_threading = false;
pthread_t worker1;
pthread_t worker2;
bool active = false;
bool debug = false;
bool in_background = false;
pthread_mutex_t queue_lock;

void lock_queue(){
    if(multi_threading) pthread_mutex_lock(&queue_lock);
}

void unlock_queue(){
    if(multi_threading) pthread_mutex_unlock(&queue_lock);
}

int throw_page_fault(PCB* pcb) {
    char* page[FRAME_SIZE];
    
    load_page(pcb, page, pcb->current_page + 1);
    store_page_in_frame(pcb, page, pcb->current_page + 1);
    
    QueueNode *node = malloc(sizeof(QueueNode));
    node->pcb = pcb;
    ready_queue_add_to_tail(node);
    return -1;
}

int execute_pcb(PCB* pcb) {
    //view_all_frames();

    //increment pc
    // init if hasn't started
    if(!pcb->did_start) {
        pcb->did_start = true;
        pcb->pc_ = mem_get_frame_data(pcb->page_table[0]);
    } else {
        struct memory_struct *last_frame_entry = mem_get_frame_data(pcb->page_table[pcb->current_page]) + (FRAME_SIZE - 1);
        // if we don't need a new frame
        if(pcb->pc_ >= last_frame_entry) {
            // check ened script
            if(pcb->current_page >= pcb->num_pages - 1) pcb->pc_ = NULL;

            // get next frame
            else if (pcb->page_table[pcb->current_page+1] >= 0) pcb->pc_ = mem_get_frame_data(pcb->page_table[++pcb->current_page]);

            else return throw_page_fault(pcb);
        } else {
            // incrament pc_ and set null if we're done
            pcb->pc_++;
            if(strcmp((pcb->pc_)->value, "none") == 0)  pcb->pc_ = NULL;
        }
    }

    // check did finish
    if(pcb->pc_ == NULL) return 1;
    // printf("command being called is %s\n",pcb->pc_->value); 
    // print_ready_queue();
    parseInput(strdup(pcb->pc_->value));
    // runt the command
    return 0;
}

void *scheduler_FCFS(){
    printf("RUNNING FCFS WHAT WHY");
    // QueueNode *cur;
    // while(true){
    //     lock_queue();
    //     if(is_ready_empty()) {
    //         unlock_queue();
    //         if(active) continue;
    //         else break;   
    //     }
    //     cur = ready_queue_pop_head();
    //     unlock_queue();
    //     execute_process(cur, MAX_INT);
    // }
    // if(multi_threading) pthread_exit(NULL);
    return 0;
}

void *scheduler_SJF(){
    printf("RUNNING SJF WHAT WHY");

    // QueueNode *cur;
    // while(true){
    //     lock_queue();
    //     if(is_ready_empty()) {
    //         unlock_queue();
    //         if(active) continue;
    //         else break;
    //     }
    //     cur = ready_queue_pop_shortest_job();
    //     unlock_queue();
    //     execute_process(cur, MAX_INT);
    // }
    // if(multi_threading) pthread_exit(NULL);
    return 0;
}

void *scheduler_AGING_alternative(){
    printf("RUNNING FCFS WHAT WHY");

    // QueueNode *cur;
    // while(true){
    //     lock_queue();
    //     if(is_ready_empty()) {
    //         unlock_queue();
    //         if(active) continue;
    //         else break;
    //     }
    //     cur = ready_queue_pop_shortest_job();
    //     ready_queue_decrement_job_length_score();
    //     unlock_queue();
    //     if(!execute_process(cur, 1)) {
    //         lock_queue();
    //         ready_queue_add_to_head(cur);
    //         unlock_queue();
    //     }   
    // }
    // if(multi_threading) pthread_exit(NULL);
    return 0;
}

void *scheduler_AGING(){
    printf("RUNNING AGING WHAT WHY");

    // QueueNode *cur;
    // int shortest;
    // sort_ready_queue();
    // while(true){
    //     lock_queue();
    //     if(is_ready_empty()) {
    //         unlock_queue();
    //         if(active) continue;
    //         else break;
    //     }
    //     cur = ready_queue_pop_head();
    //     shortest = ready_queue_get_shortest_job_score();
    //     if(shortest < cur->pcb->job_length_score){
    //         ready_queue_promote(shortest);
    //         ready_queue_add_to_tail(cur);
    //         cur = ready_queue_pop_head();
    //     }
    //     ready_queue_decrement_job_length_score();
    //     unlock_queue();
    //     if(!execute_process(cur, 1)) {
    //         lock_queue();
    //         ready_queue_add_to_head(cur);
    //         unlock_queue();
    //     }
    // }
    // if(multi_threading) pthread_exit(NULL);
    return 0;
}

void *scheduler_RR(void *arg) {
    // print_ready_queue();

    int pf, err = 0;
    QueueNode *head_node;

    
    while ((head_node = ready_queue_pop_head()) != NULL) {   
        PCB* rq_head = head_node->pcb;
        int err; 
        // execute two instructions of the process
        for (int i =  0; (i < 2) && ((rq_head->pc_ != NULL) || (rq_head->did_start == 0)); i++) {
            err = execute_pcb(rq_head);
            if(err == -1) break;
        }

        if(!err)ready_queue_add_to_tail(head_node);
        // if(ready_queue_peak()->pcb->pc_) {
        //     if(err != -1) ready_queue_add_to_tail(head_node);
        // } else {
            // printf("LOOK HERE %s\n",head_node->pcb->script_name);
            // print_ready_queue();
            // ready_queue_remove_pcb(head_node);
            // print_ready_queue();
        // }
    }
}


int threads_initialize(char* policy){
    active = true;
    multi_threading = true;
    int arg[1];
    pthread_mutex_init(&queue_lock, NULL);
    if(strcmp("FCFS",policy)==0){
        pthread_create(&worker1, NULL, scheduler_FCFS, NULL);
        pthread_create(&worker2, NULL, scheduler_FCFS, NULL);
    }else if(strcmp("SJF",policy)==0){
        pthread_create(&worker1, NULL, scheduler_SJF, NULL);
        pthread_create(&worker2, NULL, scheduler_SJF, NULL);
    }else if(strcmp("RR",policy)==0){
        arg[0] = 2;
        pthread_create(&worker1, NULL, scheduler_RR, (void *) arg);
        pthread_create(&worker2, NULL, scheduler_RR, (void *) arg);
    }else if(strcmp("AGING",policy)==0){
        pthread_create(&worker1, NULL, scheduler_AGING, (void *) arg);
        pthread_create(&worker2, NULL, scheduler_AGING, (void *) arg);
    }else if(strcmp("RR30", policy)==0){
        arg[0] = 30;
        pthread_create(&worker1, NULL, scheduler_RR, (void *) arg);
        pthread_create(&worker2, NULL, scheduler_RR, (void *) arg);
    }
    return 0;
}

void threads_terminate(){
    if(!active) return;
    bool empty = false;
    while(!empty){
        empty = is_ready_empty();
    }
    active = false;
    pthread_join(worker1, NULL);
    pthread_join(worker2, NULL);
}


int schedule_by_policy(char* policy, bool mt){
    if(strcmp(policy, "FCFS")!=0 && strcmp(policy, "SJF")!=0 && 
        strcmp(policy, "RR")!=0 && strcmp(policy, "AGING")!=0 && strcmp(policy, "RR30")!=0){
            return 15;
    }
    if(active) return 0;
    if(in_background) return 0;
    int arg[1];
    if(mt) return threads_initialize(policy);
    else{
        if(strcmp("FCFS",policy)==0){
            scheduler_FCFS();
        }else if(strcmp("SJF",policy)==0){
            scheduler_SJF();
        }else if(strcmp("RR",policy)==0){
            arg[0] = 2;
            scheduler_RR((void *) arg);
        }else if(strcmp("AGING",policy)==0){
            scheduler_AGING();
        }else if(strcmp("RR30", policy)==0){
            arg[0] = 30;
            scheduler_RR((void *) arg);
        }
        return 0;
    }
}

