#include "second_pass.h"
#include "first_pass.h"
#include "utils_and_checks.h"
#include "linked_list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/* External declaration of global variables IC and DC, defined in another file */
extern int IC;
extern int DC;



int second_pass_analyze(char *name_file, node **head_instructions_list, node **head_data_list, node **head_symbols_list, int has_errors)
{
	node *head_extern_symbols = NULL;
	int error_flag = has_errors;
	
	
	/* Merge entry labels with their corresponding definitions. If an error occurs, mark it in the error flag */
	if (merge_entry_labels(head_symbols_list) == ERROR)
		error_flag = 1;
	
	
	/* check if any label is marked as both `entry` and `extern`. If an error occurs, mark it in the error flag */
	if (check_entry_extern_conflict(*head_symbols_list) == ERROR)
	
		error_flag = 1;
		
	/* Check for 2 labels with the same name. If duplicates are found, mark the error */
	if (check_duplicate_labels(*head_symbols_list) == ERROR)
        	error_flag = 1;
        
        
        /* Update code words with symbol addresses. If an error occurs, mark it */
        if (update_code_words(head_instructions_list, head_symbols_list, &head_extern_symbols) == ERROR)
        	error_flag = 1;
        
        
        
        /*  If any errors are found during the first or the second pass, return ERROR */			
        if (error_flag)
        	return ERROR;	
        /* Generate output files only if no errors are found */
        else 
        {
        	create_object_file(name_file, *head_instructions_list, *head_data_list);
        	
        	/* If there are entry labels - creating a entry file */
        	if (count_entry_symbols(*head_symbols_list) > 0) /* Count entry symbols */
			create_entry_files(name_file, *head_symbols_list);
		
		
		/* If there are extern labels - creating a extern file */	
		if (head_extern_symbols != NULL) 
			create_extern_files(name_file, head_extern_symbols);
	}


	free_list(&head_extern_symbols, delete_extern_symbol_node);
	return SUCCESS;
}



int merge_entry_labels(node **head_symbols_list) 
{
	node *temp1 = *head_symbols_list;
	node *temp2;
	node *to_delete;
	node *prev1 = NULL; /* To keep track of the previous node for deletion */
	char *symbol_name;
	SymbolNode *symbol_data1, *symbol_data2;


	/* IterateTraverse through the symbol list */
	while (temp1 != NULL) 
	{
		symbol_data1 = (SymbolNode *)(temp1->data);
        
		if (symbol_data1->is_entry && symbol_data1->adress == TEMP_ENTRY_ADDRESS) 
        	{
            		symbol_name = symbol_data1->name;
            		temp2 = *head_symbols_list;           

			/* IterateTraverse through the symbol list */
            		while (temp2 != NULL) 
            		{
                		symbol_data2 = (SymbolNode *)(temp2->data);
                
                		/* If matching label found, mark it as entry */
                		if (strcmp(symbol_data2->name, symbol_name) == 0 && !symbol_data2->is_entry) 
                		{
                    			symbol_data2->is_entry = 1; /* Mark the label as entry */
                    			break;
                		}
                		temp2 = (node *)temp2->next;
            		}
            
            		/* If no corresponding symbol found, handle the error */
            		if (temp2 == NULL)
            		{
                		printf("Error in line number %d: Entry label '%s' is not defined in the current source file.\n", symbol_data1->line_num, symbol_name);
                		return ERROR;
                	}
                	/*Delete temp1 node if found */
                	else
                	{
                		if (prev1 == NULL) 
                    			*head_symbols_list = (node *)temp1->next; /* Remove head */
                		else 
                    			prev1->next = temp1->next; /* Bypass temp1 node */
                		to_delete = temp1;
                		temp1 = (node *)temp1->next; /* Move to next node */
                		free(to_delete->data);
                		free(to_delete);
                		continue; /* Skip temp1 update at the end of the loop */
            		}
		}
        	else 
			prev1 = temp1; /* Move prev1 forward for non-entry nodes */
			
        	temp1 = (node *)temp1->next;
	}
	
	return SUCCESS;
}



int update_code_words(node **head_instructions_list, node **head_symbols_list, node **head_extern_symbols)
{
	node *temp1 = *head_instructions_list;
	node *temp2;
	CodeNode *instruction_data;
	SymbolNode *symbol_data;
	char *symbol_name;
	char *adress_in_binary;
	
	/* Iterate through the instructions list */
	while (temp1 != NULL)
	{
		instruction_data = (CodeNode *)(temp1->data);
	
		/* Check if the code word does not start with '0' or '1', indicating a label name */
		if ((instruction_data -> code_word)[0] != '0' && (instruction_data -> code_word)[0] != '1')
		{
			symbol_name = instruction_data -> code_word;
			temp2 = *head_symbols_list;
			
			/* Iterate through the symbols list */
			while (temp2 != NULL) 
            		{
                		symbol_data = (SymbolNode *)(temp2->data);
                		
                		/* Check if the symbol name matches */
                		if (strcmp(symbol_name, symbol_data -> name) == 0)
                		{
                			/* Convert symbol address to binary and store it in the code word */
                			adress_in_binary = int_to_binary(symbol_data -> adress, 12);
                			instruction_data->code_word = (char *)malloc(CODE_WORD_LEN * sizeof(char));

					if (!instruction_data->code_word) 
					{
						printf("Allocation failure\n");
						exit(1);
					}
                			strcpy(instruction_data -> code_word, adress_in_binary);
                			free(adress_in_binary);
                			
                			/* Add appropriate suffix based on whether the symbol is external or not */
                			if (symbol_data -> is_extern)
                			{
                				strcat(instruction_data -> code_word, "001");
                				
                				/* Save external symbol and address for the external file */
                				crate_extern_node(head_extern_symbols, symbol_data->name, instruction_data->adress);
                			}
                			else
                				strcat(instruction_data -> code_word, "010");
                			
                			break; /* Exit the symbols list iteration */
                		
                		}
                		temp2 = (node *)temp2->next;
                	}
                	
                	/* If no matching symbol is found, print an error and return ERROR */
                	if (temp2 == NULL)
                	{
                		printf("Error in line number %d: Using an undefined label\n", instruction_data->line_num);
                		return ERROR;
                	}
		}
	
		temp1 = (node *)temp1->next;
	}
	
	
	
	return SUCCESS; /* Return SUCCESS if all code words are updated successfully */
}



void crate_extern_node(node **head, char *name, int address)
{
	/* Allocate memory for a new data node */
	ExternSymbolNode *node_data = (ExternSymbolNode *)malloc(sizeof(ExternSymbolNode));
    
	if (!node_data)
	{
        	printf("Allocation failure\n");
        	exit(1);
	}
    
	/* Initialize the data node with the provided values */
	node_data->name = name;
	node_data->address = address;

	/* Add the node to the end of the data linked list */
	add_node_end(head,(void *)node_data, delete_extern_symbol_node);
}



void create_object_file(char *name_file, node *head_instructions_list, node *head_data_list)
{
	node *temp = head_instructions_list;
	CodeNode *instruction_data;
	CodeNode *data_data;
	int adress;
	char code_word_in_octal[6]; /* 5 digits + null terminator */
	FILE *f;

	/* Initialize object file */
	init_file(&f, name_file, ".ob", "w");

	/* Write the header to the object file */
	fprintf(f, " %d %d\n", IC-100, DC); /* -100 because IC will be initialized to 100 - the address from which you can write to the memory */


	/* Write the instruction section */
	temp = head_instructions_list;
	while (temp != NULL)
	{
		instruction_data = (CodeNode *)(temp->data);
        	adress = instruction_data->adress;
        	fprintf(f, "%04d ", adress);

        	binary_to_octal(instruction_data->code_word, code_word_in_octal); /* Convert binary code word to octal */
        	fprintf(f, "%s\n", code_word_in_octal);

        	temp = (node *)temp->next;
	}


    	/* Write the data section */
    	temp = head_data_list;
    	while (temp != NULL)
    	{
        	data_data = (CodeNode *)(temp->data);
        	adress = data_data->adress;
        	fprintf(f, "%04d ", adress);

        	binary_to_octal(data_data->code_word, code_word_in_octal); /*  Convert binary data to octal */
        	fprintf(f, "%s\n", code_word_in_octal);

        	temp = (node *)temp->next;
    	}


    	/* Close the object file */
    	fclose(f);
}




void binary_to_octal(const char *binary, char *octal) 
{
	int decimal = 0;
	int power = 1;
	int i;
    
	/* Convert the binary string to a decimal number */
	for (i = 14; i >= 0; --i) 
	{
        	if (binary[i] == '1')
            		decimal += power;
            		
        	power *= 2;
	}

	/* Convert the decimal number to an octal string with 5 digits including leading zeros */
	sprintf(octal, "%05o", decimal);
}



int count_entry_symbols(node *head_symbols_list)
{
	int count_is_entry = 0;
	node *temp = head_symbols_list;
	SymbolNode *symbol_data;
    
	/* Iterate through the symbols list */
	while (temp != NULL)
	{
        	symbol_data = (SymbolNode *)(temp->data);
        
        	/* Check if the symbol is extern */    
        	if (symbol_data -> is_entry)
            		count_is_entry++;
            
        	temp = (node *)temp->next;
	}
    
	return count_is_entry; /* Return the total count of extern symbols */
}



void create_entry_files(char *name_file, node *head_symbols_list)
{
	node *temp = head_symbols_list;
	SymbolNode *symbol_data;
	FILE *f;
	
	/* Initialize object file */
	init_file(&f, name_file, ".ent", "w");
	
	
	/* Iterate through the symbols list */
	while (temp != NULL)
	{
		symbol_data = (SymbolNode *)(temp->data);
	
		/* Check if the symbol is marked as entry */
		if (symbol_data -> is_entry)
		{
			fprintf(f, "%s ", symbol_data -> name);
			fprintf(f, "%04d\n", symbol_data -> adress);
				
		}
			
		temp = (node *)temp->next;
	}
	
		
	/* Close the entries file */	
	fclose(f);
}



void create_extern_files(char *name_file, node *head_extern_symbols)
{
	node *temp = head_extern_symbols;
	ExternSymbolNode *extern_data;
	FILE *f;

	/* Initialize object file */
	init_file(&f, name_file, ".ext", "w");
	
	/* Iterate through the extern symbols list and write each symbol and address to the file */
	while (temp != NULL)
	{
        	extern_data = (ExternSymbolNode *)(temp->data);
        	
        	/* Write the symbol name and its address */
        	fprintf(f, "%s %04d\n", extern_data->name, extern_data->address);
        	
        	temp = (node *)temp->next;
	}
	

	/* Close the external file */	
	fclose(f);
}



void delete_extern_symbol_node(void *e)
{
	ExternSymbolNode *data_ext_symbol_node = (ExternSymbolNode *)e;
	free(data_ext_symbol_node);
}




