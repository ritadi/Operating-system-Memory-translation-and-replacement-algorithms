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

typedef struct LinkedListNode_t{
    unsigned int frame;
    struct LinkedListNode_t *next;
} lnklstnode;

lnklstnode *head = NULL;
lnklstnode *tail = NULL;

int *referenced;


/* Page to evict is chosen using the accurate LRU algorithm.
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */

int lru_evict() {
    if (head == NULL){
        fprintf(stderr, "Cannot access head pointer\n");
    }
    
    int frame = head->frame;//not first access and not in page table, eviction only
    
    if (head == tail){
        tail = NULL;
    }
    
    if (referenced[frame] != 1){
        fprintf(stderr, "should've been referenced\n");
    }
    
    //about to be evicted, set to unreferenced
    referenced[frame] = 0;
    //update the head node
    lnklstnode *new_node = head->next;
    head = new_node;
    
	return frame;
}

/* This function is called on each access to a page to update any information
 * needed by the lru algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void lru_ref(pgtbl_entry_t *p) {
    
    int frame = p->frame >> PAGE_SHIFT;
    
    //create a new node to store frame of p
    lnklstnode *new_node = (lnklstnode *)malloc(sizeof(lnklstnode));
    new_node->frame = frame;
    new_node->next = NULL;
    
    
    if (referenced[frame] == 1){//frame already in page table
        
        //append new node to tail
        tail->next = new_node;
        tail = tail->next;
        
        lnklstnode *cpy = head;
        lnklstnode *cpy1 = NULL;
        lnklstnode *before = NULL;
        //trying to find the node where frame last referenced
        while (cpy->frame != frame){
            before = cpy;
            cpy = cpy->next;
        }
        
        if (cpy != head){//delete in the middle of the list
            cpy1 = cpy->next;
            before->next = cpy1;
            free(cpy);
        }else{//delete the head
            head = cpy->next;
            if (head == NULL){//empty list after deletion
                tail = NULL;
            }
            
        }
        
    }else{//first access and not evicted
        
        if (tail == NULL){//empty list, make new node both head and tail
            tail = new_node;
            head = tail;
        }else{//not empty list, append new node to tail
            tail->next = new_node;
            tail = tail->next;
        }
        
        referenced[frame] = 1;//set to referenced, frame in page table
    }
	return;
}


/* Initialize any data structures needed for this 
 * replacement algorithm 
 */
void lru_init() {
    referenced = malloc(sizeof(int) * memsize);
    int i;
    for (i = 0; i < memsize; i++){
        referenced[i] = 0;
    }
    
}
