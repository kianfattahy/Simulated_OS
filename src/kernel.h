#ifndef KERNEL
#define KERNEL
#include "pcb.h"
int schedule_by_policy(char* policy, bool mt);
void ready_queue_destory();
void threads_terminate();
#endif