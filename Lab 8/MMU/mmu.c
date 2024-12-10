#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "list.h"
#include "util.h"

void TOUPPER(char * arr){
  
    for(int i=0;i<strlen(arr);i++){
        arr[i] = toupper(arr[i]);
    }
}

void get_input(char *args[], int input[][2], int *n, int *size, int *policy) 
{
  	FILE *input_file = fopen(args[1], "r");
	  if (!input_file) {
		    fprintf(stderr, "Error: Invalid filepath\n");
		    fflush(stdout);
		    exit(0);
	  }

    parse_file(input_file, input, n, size);
  
    fclose(input_file);
  
    TOUPPER(args[2]);
  
    if((strcmp(args[2],"-F") == 0) || (strcmp(args[2],"-FIFO") == 0))
        *policy = 1;
    else if((strcmp(args[2],"-B") == 0) || (strcmp(args[2],"-BESTFIT") == 0))
        *policy = 2;
    else if((strcmp(args[2],"-W") == 0) || (strcmp(args[2],"-WORSTFIT") == 0))
        *policy = 3;
    else {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
    }
        
}

void allocate_memory(list_t * freelist, list_t * alloclist, int pid, int blocksize, int policy) {
   node_t *current = freelist->head;
    node_t *prev = NULL;

    node_t *selected_prev = NULL;
    node_t *selected_node = NULL;

    if(policy == 1){ // First Fit
        while(current != NULL){
            int current_size = current->blk->end - current->blk->start + 1;
            if(current_size >= blocksize){
                selected_prev = prev;
                selected_node = current;
                break;
            }
            prev = current;
            current = current->next;
        }
    }
    else if(policy == 2){ // Best Fit
        int smallest_diff = __INT32_MAX__;
        while(current != NULL){
            int current_size = current->blk->end - current->blk->start + 1;
            if(current_size >= blocksize){
                int diff = current_size - blocksize;
                if(diff < smallest_diff){
                    smallest_diff = diff;
                    selected_prev = prev;
                    selected_node = current;
                }
            }
            prev = current;
            current = current->next;
        }
    }
    else if(policy == 3){ // Worst Fit
        int largest_diff = -1;
        while(current != NULL){
            int current_size = current->blk->end - current->blk->start + 1;
            if(current_size >= blocksize){
                int diff = current_size - blocksize;
                if(diff > largest_diff){
                    largest_diff = diff;
                    selected_prev = prev;
                    selected_node = current;
                }
            }
            prev = current;
            current = current->next;
        }
    }
    else{
        printf("Error: Unknown Memory Management Policy\n");
        return;
    }

    if(selected_node == NULL){
        printf("Error: Not Enough Memory\n");
        return;
    }

    // Allocate memory from selected_node
    int allocated_start = selected_node->blk->start;
    int allocated_end = allocated_start + blocksize - 1;

    // Create allocated block
    block_t *allocated_blk = malloc(sizeof(block_t));
    allocated_blk->pid = pid;
    allocated_blk->start = allocated_start;
    allocated_blk->end = allocated_end;

    // Insert allocated block into alloclist in ascending order by address
    list_add_ascending_by_address(alloclist, allocated_blk);

    // Handle fragmentation
    int remaining_size = (selected_node->blk->end - selected_node->blk->start + 1) - blocksize;
    if(remaining_size > 0){
        block_t *fragment = malloc(sizeof(block_t));
        fragment->pid = 0;
        fragment->start = allocated_end + 1;
        fragment->end = selected_node->blk->end;

        // Update the selected node's block to represent the allocated portion
        selected_node->blk->end = allocated_end;

        // Insert the fragment back into freelist based on policy
        if(policy == 1){ // First Fit
            list_add_to_back(freelist, fragment);
        }
        else if(policy == 2){ // Best Fit
            list_add_ascending_by_blocksize(freelist, fragment);
        }
        else if(policy == 3){ // Worst Fit
            list_add_descending_by_blocksize(freelist, fragment);
        }
    }

    // If the entire block is allocated, remove it from freelist
    if((selected_node->blk->end - selected_node->blk->start + 1) == blocksize){
        // Remove selected_node from freelist
        if(selected_prev == NULL){
            freelist->head = selected_node->next;
        }
        else{
            selected_prev->next = selected_node->next;
        }
        // Free the selected_node
        free(selected_node->blk);
        free(selected_node);
    }
}

void deallocate_memory(list_t * alloclist, list_t * freelist, int pid, int policy) { 
  node_t *current = alloclist->head;
    node_t *prev = NULL;

    // Find the allocated block with the given pid
    while(current != NULL && current->blk->pid != pid){
        prev = current;
        current = current->next;
    }

    if(current == NULL){
        printf("Error: Can't locate Memory Used by PID: %d\n", pid);
        return;
    }

    // Remove the block from alloclist
    if(prev == NULL){
        alloclist->head = current->next;
    }
    else{
        prev->next = current->next;
    }

    // Prepare the block to be freed
    current->blk->pid = 0;

    // Insert the block back into freelist based on policy
    if(policy == 1){ // First Fit
        list_add_to_back(freelist, current->blk);
    }
    else if(policy == 2){ // Best Fit
        list_add_ascending_by_blocksize(freelist, current->blk);
    }
    else if(policy == 3){ // Worst Fit
        list_add_descending_by_blocksize(freelist, current->blk);
    }
    else{
        printf("Error: Unknown Memory Management Policy\n");
        // Since policy is unknown, default to adding to back
        list_add_to_back(freelist, current->blk);
    }

    // Free the node
    free(current);
}

list_t* coalese_memory(list_t * list){
  list_t *temp_list = list_alloc();
  block_t *blk;
  
  while((blk = list_remove_from_front(list)) != NULL) {  // sort the list in ascending order by address
        list_add_ascending_by_address(temp_list, blk);
  }
  
  //combine physically adjacent blocks
  
  list_coalese_nodes(temp_list);
        
  return temp_list;
}

void print_list(list_t * list, char * message){
    node_t *current = list->head;
    block_t *blk;
    int i = 0;
  
    printf("%s:\n", message);
  
    while(current != NULL){
        blk = current->blk;
        printf("Block %d:\t START: %d\t END: %d", i, blk->start, blk->end);
      
        if(blk->pid != 0)
            printf("\t PID: %d\n", blk->pid);
        else  
            printf("\n");
      
        current = current->next;
        i += 1;
    }
}

/* DO NOT MODIFY */
int main(int argc, char *argv[]) 
{
   int PARTITION_SIZE, inputdata[200][2], N = 0, Memory_Mgt_Policy;
  
   list_t *FREE_LIST = list_alloc();   // list that holds all free blocks (PID is always zero)
   list_t *ALLOC_LIST = list_alloc();  // list that holds all allocated blocks
   int i;
  
   if(argc != 3) {
       printf("usage: ./mmu <input file> -{F | B | W }  \n(F=FIFO | B=BESTFIT | W-WORSTFIT)\n");
       exit(1);
   }
  
   get_input(argv, inputdata, &N, &PARTITION_SIZE, &Memory_Mgt_Policy);
  
   // Allocated the initial partition of size PARTITION_SIZE
   
   block_t * partition = malloc(sizeof(block_t));   // create the partition meta data
   partition->start = 0;
   partition->end = PARTITION_SIZE + partition->start - 1;
                                   
   list_add_to_front(FREE_LIST, partition);          // add partition to free list
                                   
   for(i = 0; i < N; i++) // loop through all the input data and simulate a memory management policy
   {
       printf("************************\n");
       if(inputdata[i][0] != -99999 && inputdata[i][0] > 0) {
             printf("ALLOCATE: %d FROM PID: %d\n", inputdata[i][1], inputdata[i][0]);
             allocate_memory(FREE_LIST, ALLOC_LIST, inputdata[i][0], inputdata[i][1], Memory_Mgt_Policy);
       }
       else if (inputdata[i][0] != -99999 && inputdata[i][0] < 0) {
             printf("DEALLOCATE MEM: PID %d\n", abs(inputdata[i][0]));
             deallocate_memory(ALLOC_LIST, FREE_LIST, abs(inputdata[i][0]), Memory_Mgt_Policy);
       }
       else {
             printf("COALESCE/COMPACT\n");
             FREE_LIST = coalese_memory(FREE_LIST);
       }   
     
       printf("************************\n");
       print_list(FREE_LIST, "Free Memory");
       print_list(ALLOC_LIST,"\nAllocated Memory");
       printf("\n\n");
   }
  
   list_free(FREE_LIST);
   list_free(ALLOC_LIST);
  
   return 0;
}