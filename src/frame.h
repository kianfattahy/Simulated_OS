#ifndef FRAME_H
#define FRAME_H

#include "pcb.h"

typedef struct frame {
    bool avail; 
    int age;
    PCB* pcb;
} frame_t;



#endif