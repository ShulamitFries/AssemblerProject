#ifndef LINKED_LIST_H
#define LINKED_LIST_H


typedef struct node * next_type;
typedef struct{
	void *data;
	next_type next;
}node;


/**
 * add_node_end - Adds a new node to the end of the linked list.
 * 
 * This function creates a new node with the provided data and appends it
 * to the end of the linked list. If the list is empty, it initializes the list 
 * with the new node.
 * 
 * @param head Pointer to the head of the linked list.
 * @param new_data Pointer to the data to be stored in the new node.
 * @param delete_data Function pointer to a function that deletes the data in a node in case of allocation failure.
 */
void add_node_end(node ** head, void * new_data, void(*delete_data)(void *));



/**
 * free_list - Frees the entire linked list.
 * 
 * This function iterates through the linked list, deletes the data in each node 
 * using the provided delete_data function, and then frees the memory allocated 
 * for each node.
 * 
 * @param head_ptr Pointer to the head pointer of the linked list.
 * @param delete_data Function pointer to a function that deletes the data in a node.
 */
void free_list(node ** head_ptr, void(*delete_data)(void *));



#endif
