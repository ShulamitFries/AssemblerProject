#include "linked_list.h"
#include "utils_and_checks.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "macro.h"


int line_num_s = 0; /* Global variable for line number in files with suffix s, (used for error messages). */


int macro_analyze(char * name_file, node **head_macro_list)
{
	char line[MAX_LEN_LINE], *first_field, *macro_name, *macro_replacement;
	int i;
	FILE * fr, *fw;
	

	/* Initialize input and output files */
	init_file(&fr, name_file, ".as", "r");
	init_file(&fw, name_file, ".am", "w+");
	
	
	/* Read lines from the input file */
	while (fgets(line,MAX_LEN_LINE,fr))
	{
		line_num_s++;
		i = 0;  /* Reset i for each new line */
        	first_field = extract_word(line, &i);
			
		/* If the line starts with "macr", handle the macro definition */
		if (strcmp(first_field, "macr") == 0)
		{
			macro_name = extract_word(line, &i);
		
			/* Checking that there are no extra characters in the definition line */
			if (!check_only_whitespace_after_index(line, i))
			{
				printf("Error in line number %d: No additional characters are allowed in the definition line\n", line_num_s);
				free(macro_name);
				free(first_field);
				return ERROR;
			}
				
			/* Checks if a macro name is a reserved word, if so - the macro name is invalid and this is an error, stop, report the errors and go to the next source file (if any).*/
			if (is_reserved_word(macro_name) == SUCCESS)
			{
				printf("Error in line number %d: invalid macro name\n", line_num_s);
				free(macro_name);
				free(first_field);
				return ERROR;
			}
			else
			{
				if (!handle_macro(&fr, macro_name, head_macro_list))
				{
					free(macro_name);
					free(first_field);
					return ERROR;
				}
			}
				
			free(macro_name);
		}	
		/* If the line contains a macro name, replace it with the macro content */	
		else if ((macro_replacement = find_macro(first_field, head_macro_list)) != NULL) 
			fputs(macro_replacement, fw);
		
		/* Otherwise, write the line as is to the output file */
		else
			fputs(line, fw);
			
		free(first_field);
	}
	
	/* Close the input and output files */
	fclose(fr);
	fclose(fw);
	
	return SUCCESS;
}



int handle_macro(FILE ** fp, char *macro_name, node **head)
{
	char line[MAX_LEN_LINE], *first_field, *macro_content, *ptr;
	int i, len;
	
	/* Initialize the macro content with an empty string */
	macro_content = (char *)malloc(sizeof(char));
	if (!macro_content)
	{
		printf("Allocation failure\n");
		exit(1);
	}
	*macro_content = EOS;
	
	/* Read lines until "endmacr" is encountered */
	while (fgets(line,MAX_LEN_LINE,*fp))
	{
		i = 0; /* Reset i for each new line */
		first_field = extract_word(line, &i);
		
		if (strcmp(first_field, "endmacr") == 0)
		{
			/* Checking that there are no extra characters in the end line */
			if (!check_only_whitespace_after_index(line, i))
			{
				printf("Error in line number %d: No additional characters are allowed in the end line\n", line_num_s);
				free(first_field);
				return ERROR;/* Indicate failure */
			}
			create_node(macro_name, macro_content, head);
			free(macro_content);
			return SUCCESS; /* Indicate success */
		}	
		else
		{
			len = strlen(line);
			
			/* Reallocate memory to accommodate the new line */
			ptr = (char *)realloc(macro_content, strlen(macro_content)+(len+1)*sizeof(char));
			if (!ptr)
			{
				free(macro_content);
				printf("Allocation failure\n");
				exit(1);/*return 1 in main*/
			}
			macro_content = ptr;
			*(macro_content+strlen(macro_content)+1) = EOS; /*Ensure the new memory is null-terminated */
			strcat(macro_content, line);
		}
		
		free(first_field);
	}
	
	free(macro_content);
	free(ptr);
	
	return SUCCESS; /* Indicate success */
}



void create_node(char *macro_name, char *macro_content, node **head)
{
	/* Allocate memory for the MacroNode structure */
	MacroNode *data = (MacroNode *)malloc(sizeof(MacroNode));
	if (!data) 
	{
		printf("Allocation failure\n");
		exit(1);
	}
	
	/*Allocate memory and copy the macro name */
	data->name = (char *)malloc(strlen(macro_name+1));
	if (!data->name)
	{
        	printf("Allocation failure\n");
        	free(data);
		exit(1);
	}
	strcpy(data->name, macro_name);
	
	/*  Allocate memory and copy the macro content*/
	data->content = (char *)malloc(strlen(macro_content)+1);
	if (!data->content)
	{
        	printf("Allocation failure\n");
        	free(data->name);
        	free(data);
		exit(1);
	}
	strcpy(data->content, macro_content);
	
	add_node_end(head,(void *)data, delete_macro_node);
	
	/*free(data);*/
}



char * find_macro(char *first_field, node **head)
{
	node * temp = *head;
	MacroNode *data_m;

	/* Traverse the linked list to find the macro */
	while (temp != NULL)
	{
		data_m = (MacroNode *)temp->data;
		if (strcmp(data_m->name, first_field) == 0)
		{
			return data_m->content;
		}
		temp = (node *)temp->next;
	}
	
	/* Return NULL if the macro is not found */	
	return NULL;
}



void delete_macro_node(void *m)
{
	MacroNode *data_macro_node = (MacroNode *)m;
	free(data_macro_node -> name);
	free(data_macro_node -> content);
	free(data_macro_node);
}

