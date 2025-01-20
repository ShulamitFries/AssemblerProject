#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>



void add_node_end(node ** head, void * new_data, void(*delete_data)(void *))
{
	node * temp = *head;
	if(temp==NULL) /* If the list is empty */
	{
		*head = (node *)malloc(sizeof(node));
		if(*head==NULL)
		{
			printf("Allocation failure\n");
			exit(1);
		}
		
		(*head)->next = NULL;
		(*head)->data = new_data;
	}
	else
	{
		/* Traverse to the end of the list */
		while(temp->next!=NULL)
		{
			temp = (node *)temp->next;
		}
		temp->next = (next_type)malloc(sizeof(node));
		if(temp->next==NULL)
		{
			printf("Allocation failure\n");
			free_list(head, delete_data); /* Clean up the list in case of allocation failure */
			exit(1);
		}
		
		temp = (node *)temp->next;
		temp->data = new_data;
		temp->next = NULL;
	}
	

}



void free_list(node ** head_ptr, void(*delete_data)(void *))
{
	node * temp;
	node * head = *head_ptr;
	
	/* While there are nodes in the list */
	while(head!=NULL)
	{
		temp = (node *)head->next; /* Store the next node */
		delete_data((void *)head->data); /* Free the data in the current node */
		free(head); /* Free the current node */
		head = temp;
	}
	
	*head_ptr = NULL; /* Set the head pointer to NULL, indicating the list is empty */

}



