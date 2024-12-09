// list/list.c
// 
// Implementation for linked list.
//
// <Author>

// Include necessary libraries for input/output, memory allocation, and string manipulation
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Include the header file that contains the structure definitions and function prototypes
#include "list.h"

// Function to create a new, empty list
list_t *list_alloc() { 
  // Allocate memory for a new list structure
  list_t* mylist = (list_t *) malloc(sizeof(list_t)); 
  // If memory allocation was successful, initialize the list's head to NULL (empty list)
  if (mylist != NULL) {
    mylist->head = NULL;
  }
  // Return the newly created list
  return mylist;
}

// Function to free all memory used by the list
void list_free(list_t *l) {
  // If the list is NULL, there's nothing to free, so return
  if (l == NULL) return;
  
  // Start at the head of the list
  node_t *current = l->head;
  // Go through each node in the list
  while (current != NULL) {
    // Save the current node
    node_t *temp = current;
    // Move to the next node
    current = current->next;
    // Free the memory of the saved node
    free(temp);
  }
  // After freeing all nodes, free the list structure itself
  free(l);
}

// Function to convert the list to a string representation
char* listToString(list_t *l) {
    // If the list is NULL, return NULL
    if (l == NULL) return NULL;

    // Allocate a large buffer to store the string representation
    char* buf = (char*)malloc(sizeof(char) * 10024);
    // If memory allocation failed, return NULL
    if (buf == NULL) return NULL;

    // Temporary buffer for each node's string representation
    char tbuf[20];
    // Initialize the main buffer as an empty string
    buf[0] = '\0';

    // Start at the head of the list
    node_t* curr = l->head;
    // Go through each node in the list
    while (curr != NULL) {
        // Convert the current node's value to a string with an arrow
        sprintf(tbuf, "%d->", curr->value);
        // Append this string to the main buffer
        strcat(buf, tbuf);
        // Move to the next node
        curr = curr->next;
    }
    // After all nodes, append "NULL" to represent the end of the list
    strcat(buf, "NULL");
    // Return the complete string representation
    return buf;
}

//find the index of a given value in the list
int list_get_index_of(list_t *l, elem value) {
    // If the list is NULL, return -1 (not found)
    if (l == NULL) return -1;

    node_t *current = l->head;
    int index = 0;
    // Go through each node in the list
    while (current != NULL) {
        // If current node's value matches, return its index
        if (current->value == value) {
            return index;
        }
        current = current->next;
        index++;
    }
    return -1;
}

// Function to print list
void list_print(list_t *l) {
  // If the list is NULL, there's nothing to print, so return
  if (l == NULL) return;
  
  node_t *current = l->head;
  
  while (current != NULL) {
    // Print current node's value followed by an arrow
    printf("%d -> ", current->value);
    // Move to the next node
    current = current->next;
  }
  // After all nodes, print "NULL" to represent the end of the list
  printf("NULL\n");
}

// Function to get length of list
int list_length(list_t *l) {
  // If the list is NULL, its length is 0
  if (l == NULL) return 0;
  
  int count = 0;
  node_t *current = l->head;
  while (current != NULL) {
    count++;
    current = current->next;
  }

  return count;
}

// Function to add a new element to the front of the list
void list_add_to_front(list_t *l, elem value) {
  if (l == NULL) return;
  
  // Create a new node with the given value
  node_t *new_node = getNode(value);
  // Set new node's next pointer to the current head of list
  new_node->next = l->head;
  // Set the list's head to the new node, making it the new front of the list
  l->head = new_node;
}

// Helper function to create a new node with a given value
node_t * getNode(elem value) {
  // Allocate memory for a new node
  node_t *mynode = (node_t *) malloc(sizeof(node_t));
  // Set the node's value
  mynode->value = value;
  // Initialize the node's next pointer to NULL
  mynode->next = NULL;
  // Return the new node
  return mynode;
}

// Function to add a new element to the back of the list
void list_add_to_back(list_t *l, elem value) {
  if (l == NULL) return;
  

  node_t *new_node = getNode(value);
  
  // If list is empty, make the new node the head
  if (l->head == NULL) {
    l->head = new_node;
  } else {
    // Otherwise, find last node in list
    node_t *current = l->head;
    while (current->next != NULL) {
      current = current->next;
    }
    // Add the new node after the last node
    current->next = new_node;
  }
}

// Function to remove and return the element at the front of the list
elem list_remove_from_front(list_t *l) {
    if (l == NULL || l->head == NULL) {
        return -1;  // Return -1 to indicate an error
    }
    
    node_t *to_remove = l->head;
    elem value = to_remove->value;
    l->head = to_remove->next;
    free(to_remove);
    
    return value;
}

// Function to remove and return the element at the back of the list
elem list_remove_from_back(list_t *l) {
  // If the list is NULL or empty, return -1
  if (l == NULL || l->head == NULL) return -1;
  
  // If there's only one node in the list
  if (l->head->next == NULL) {
    // Get its value
    elem value = l->head->value;
    // Free the node
    free(l->head);
    // Set the list's head to NULL (empty list)
    l->head = NULL;
    // Return the value
    return value;
  }
  
  // Otherwise, find the second-to-last node
  node_t *current = l->head;
  while (current->next->next != NULL) {
    current = current->next;
  }
  
  // Get the value from the last node
  elem value = current->next->value;
  // Free the last node
  free(current->next);
  // Set the second-to-last node's next pointer to NULL, making it the new last node
  current->next = NULL;
  // Return the value from the removed node
  return value;
}

// Function to add a new element at a specific index in the list
void list_add_at_index(list_t *l, elem value, int index) {

  if (l == NULL || index < 0) return;
  
  // If the index is 0, add to the front of the list
  if (index == 0) {
    list_add_to_front(l, value);
    return;
  }
  
  // Create a new node with the given value
  node_t *new_node = getNode(value);
  // Start at the head of the list
  node_t *current = l->head;
  // Move to the node just before the desired index
  int i;
  for (i = 0; i < index - 1 && current != NULL; i++) {
    current = current->next;
  }
  
  // If we've reached the end of the list before the desired index, return
  if (current == NULL) return;
  
  // Insert the new node into the list
  new_node->next = current->next;
  current->next = new_node;
}

elem list_remove_at_index(list_t *l, int index) {
    printf("Removing at index: %d\n", index);  // Debug print

    if (l == NULL) {
        printf("List is NULL\n");  // Debug print
        return -1;
    }

    if (l->head == NULL) {
        printf("List is empty\n");  // Debug print
        return -1;
    }

    if (index < 0) {
        printf("Invalid index: %d\n", index);  // Debug print
        return -1;
    }

    if (index == 0) {
        elem value = l->head->value;
        node_t *temp = l->head;
        l->head = l->head->next;
        free(temp);
        printf("Removed %d from front\n", value);  // Debug print
        return value;
    }

    node_t *current = l->head;
    node_t *previous = NULL;
    int current_index = 0;

    while (current != NULL && current_index < index) {
        previous = current;
        current = current->next;
        current_index++;
    }

    if (current == NULL) {
        printf("Index out of bounds\n");  // Debug print
        return -1;
    }

    elem value = current->value;
    previous->next = current->next;
    free(current);

    printf("Removed %d at index %d\n", value, index);  // Debug print
    return value;
}

// Function to get the element at a specific index in the list
elem list_get_elem_at(list_t *l, int index) {

  if (l == NULL || index < 0) return -1;
  
  // Start at the head of the list
  node_t *current = l->head;
  // Move to the node at the desired index
  int i;
  for (i = 0; i < index && current != NULL; i++) {
    current = current->next;
  }
  
  // If we've found a node at the desired index, return its value; otherwise, return -1
  return (current != NULL) ? current->value : -1;
}