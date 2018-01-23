#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"

#include <string.h>

extern int memsize;

extern int debug;

extern struct frame *coremap;

int clock_hand = 0;

int *referenced;
/* Page to evict is chosen using the clock algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int clock_evict() {
    while (clock_hand < memsize){
        if (referenced[clock_hand] == 1){//if reference bit is one,
                                        //give it a second chance
            referenced[clock_hand] = 0;  //set it back to zero
        }else{                           //if reference bit is zero, we found the page
            referenced[clock_hand] = 1;  //set reference bit to one
            return clock_hand;
        }
        clock_hand = (clock_hand + 1) % memsize; //circular index
    }
    return 0;//should never reach here
}

/* This function is called on each access to a page to update any information
 * needed by the clock algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void clock_ref(pgtbl_entry_t *p) {
    
    int frame;
    frame = p->frame >> PAGE_SHIFT;
    referenced[frame] = 1;//set the reference bit at frame index to one
    
}

/* Initialize any data structures needed for this replacement
 * algorithm. 
 */
void clock_init() {
    referenced = malloc(sizeof(int) * memsize);
    int i;
    for (i = 0; i < memsize; i++){
        referenced[i] = 0;
    }
}
