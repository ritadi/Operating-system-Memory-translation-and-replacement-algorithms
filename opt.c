#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include "pagetable.h"


#include "sim.h"
//memsize, tracefile and MAXLINE



extern struct frame *coremap;

int size;

int tracefile_index;

int idx;

addr_t *address_array;




/* Page to evict is chosen using the optimal (aka MIN) algorithm. 
 * Returns the page frame number (which is also the index in the coremap)
 * for the page that is to be evicted.
 */
int opt_evict() {
	int i;
	int x;
	int distance = 0; //max distance
	for (i = 0; i < memsize; i++ ){
		int dis = 0; //distance for vaddr i
		for (x = tracefile_index; x<size; x++){
			if (coremap[i].vaddr != address_array[x]){
				dis++;
			}
			else {	// found vaddr
				if (distance < dis){
					distance = dis;
					idx = i;
					//best eviction candidate yet
				
				}
				break;
				
			}
				
		}
		if (x == size){ //didnt find same vaddr in remaining trace file
			if (distance < dis){
				distance = dis;
				idx = i;
				// exvict the not found one
			}
		}
	}	


	return idx;
}

/* This function is called on each access to a page to update any information
 * needed by the opt algorithm.
 * Input: The page table entry for the page that is being accessed.
 */
void opt_ref(pgtbl_entry_t *p) {
	tracefile_index++; // update where in the trace file we are at
	return;
}

/* Initializes any data structures needed for this
 * replacement algorithm.
 */
void opt_init() {
	tracefile_index = 0;
	idx = 0;
	char type;
	addr_t vaddr;
	FILE* fp;
	if (!(fp = fopen(tracefile, "r"))){
		perror(tracefile);
	}
	char buffer[MAXLINE];
	int count = 0;
	//check how many lines the trace file has
	while (fgets(buffer, MAXLINE, fp) != NULL){
		if (buffer[0] != '='){
		count++;
		}
	}
	int i = 0;
	size = count;
	address_array = (addr_t *)malloc(count * sizeof (addr_t));
	rewind(fp);
	//read trace file into a array line by line
	while (fgets(buffer, MAXLINE, fp) != NULL){
		if (buffer[0] != '='){
			sscanf(buffer, "%c %lx", &type, &vaddr);
			address_array[i] = vaddr;
			i++;
		}
		
	}
	fclose(fp);
}

