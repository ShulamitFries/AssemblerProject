#include "utils_and_checks.h"
#include "macro.h"
#include "first_pass.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>


/* A table of operation names and their corresponding opcodes */
Operation op_names_table[] = {{"mov", "0000"}, {"cmp", "0001"}, {"add", "0010"}, {"sub", "0011"}, {"lea", "0100"}, {"clr", "0101"}, {"not", "0110"}, {"inc", "0111"}, {"dec", "1000"}, {"jmp", "1001"}, {"bne", "1010"}, {"red", "1011"}, {"prn", "1100"}, {"jsr", "1101"}, {"rts", "1110"}, {"stop", "1111"}};



char *extract_word(char *line, int *start_index)
{
	int i = *start_index, j = 0;
	char *word;

	/* Skip leading whitespace */
	while( isspace(line[i]))
		i++;
	
	/* Find the length of the word */	
	/*while(!isspace(line[i]) && line[i] != EOS && line[i] != ',')*/
	while (isalnum(line[i]) || line[i] == '#' || line[i] == '*' || line[i] == '.' || line[i] == '-' || line[i] == '_')
	{
		i++;
		j++;
	}
	
	/* Allocate memory for the word */
	word = (char *)malloc(j + 1);
	if (!word)
	{
		printf("Allocation failure\n");
		exit(1);
	}
	
	/* Copy the word from the line */
	strncpy(word, line + (i - j), j);
	word[j] = EOS;
	
	*start_index = i; /* Update the end index */
	
	return word;
}



int check_only_whitespace_after_index(char *line, int i)
{
	while (isspace(line[i]))
		i++;
		
	if (line[i] != NEW_LINE && line[i] != EOF && line[i] != EOS)
		return ERROR; /* Indicate failure */
	
	return SUCCESS;
}



char* int_to_binary(int num, int bits)
{
	int i;
	char* binary = (char*)malloc(bits + 1);/* Allocate memory for 'bit_length' bits + 1 for null terminator */
	if (!binary)
	{
		printf("Allocation failure\n");
		exit(1);
	}

	binary[bits] = EOS;/* Ensure the string is null-terminated */

	for (i = bits - 1; i >= 0; i--)
	{
		binary[i] = (num & 1) ? '1' : '0';/* Set each bit to '1' or '0' based on the least significant bit of num */
	        num >>= 1; /* Shift number right by 1 bit to process the next bit */
	}

	return binary;
}



char* generate_full_name(const char *base_name, const char *extension) 
{
	char *full_name_file;

	
	/* Allocate memory for the full file name */
	full_name_file = (char *)malloc(strlen(base_name) + strlen(extension));  /* "." + null terminator */
    
	if (!full_name_file) 
	{
		printf("Allocation failure\n");
		exit(1);
	}

	/* Create the full file name with the appropriate extension */
	sprintf(full_name_file, "%s%s", base_name, extension);
	
	return full_name_file;
}



void init_file(FILE **fp, const char *name_file, const char *extension, const char *mode)
{
	char *full_name_file = generate_full_name(name_file, extension);
    
	/* Open the file with the specified mode */
	*fp = fopen(full_name_file, mode);
	/* Check if the file was opened successfully */
	if (!*fp) 
	{ 
        	printf("Error! The file %s cannot be opened with mode %s\n", full_name_file, mode);
        	free(full_name_file); /* Free the allocated memory in case of failure */
        	exit(1);
	}

	 /* Free the allocated memory for the full file name */
	free(full_name_file);
}



int find_op_index(char *op_name)
{
	int i;

	/* Loop through each operation name in the table */
	for (i = 0; i < NUM_OP_NAMES; i++)
	{
		/* Compare the given operation name with the current table entry */
		if (strcmp(op_name, op_names_table[i].name) == 0)
			return i;/*  Return the index if a match is found */
	}
	
	return ERROR; /* Return ERROR if the operation name is not found in the table */
}



int is_register(char *reg) 
{
	/* The full list of registers: r0, r1, r2, r3, r4, r5, r6, r7 
	That is registers are represented with the format 'rX', where X is a digit from 0 to 7 
	Also, their length is 2 */
	
	if (strlen(reg) != REG_LENGTH)
		return ERROR;

	/* Check if the first character is 'r' */
	if (reg[0] != 'r') 
		return ERROR;

	/* Check if the second character is a digit between '0' and '7'*/ 
	if (reg[1] >= '0' && reg[1] <= '7')
		return SUCCESS;

    return ERROR;
}



int is_directive(const char *str) 
{
	if (strcmp(str, ".data") == 0 || strcmp(str, ".string") == 0 || strcmp(str, ".entry") == 0 || strcmp(str, ".extern") == 0)
		return SUCCESS;
	
	return ERROR;
}



int is_reserved_word(const char *str)
{
    /* Check if it's a directive */
    if (is_directive(str) == SUCCESS)
        return SUCCESS;

    /* Check if it's an operation */
    if (find_op_index((char *)str) != ERROR)
        return SUCCESS;

    /* Check if it's a register */
    if (is_register((char *)str) == SUCCESS)
        return SUCCESS;

    /* Not a reserved word */
    return ERROR;
}



int is_valid_number(int num, int bit_length)
{
	/* Calculate the valid range for the given bit length in 2's complement representation */
	int min_value = -(int)pow(2, bit_length - 1);
	int max_value = (int)pow(2, bit_length - 1) - 1;

	/* Check if the number is within the valid range */
	if (num >= min_value && num <= max_value) 
        	return SUCCESS; /* Indicate success */
    
	return ERROR; /* Indicate failure */
}



int is_valid_comma_count(int comma, int expected_commas, int line_num)
{
	/* Check for missing comma error */
	if (comma < expected_commas)
	{
		printf("Error in line number %d: Missing comma\n", line_num);
		return ERROR; /* Indicate failure */
	}
	
	/* Check for missing comma error */
	if (comma > expected_commas)
	{
		printf("Error in line number %d: Too many commas\n", line_num); /*Multiple consecutive commas\n");*/
		return ERROR; /* Indicate failure */
	}
	
	return SUCCESS;/* Indicate success */
}



int is_valid_symbol(char *symbol_name)
{
	int i;
	
	/* Check if the first character is alphabetic */
	if (!isalpha(symbol_name[0]))
		return ERROR; /* Indicate failure */
	
	/* Check if all characters are alphanumeric */
	for (i = 0; symbol_name[i]; i++)
	{
		if (!isalnum(symbol_name[i]))
			return ERROR; /* Indicate failure */
	}
	
	/* Check if the symbol length exceeds the maximum allowed length */
	if (strlen(symbol_name) > MAX_LEN_SYMBOL)
		return ERROR; /* Indicate failure */

	
	return SUCCESS; /* Indicate success */
}



int check_macro_symbol_conflict(node *head_macro_list, node *head_symbols_list)
{
	node *temp1 = head_macro_list;
	node *temp2;
	MacroNode *macro_data;
	SymbolNode *symbol_data;
	char *macro_name;
	
	/*Iterate through the macro list*/
	while (temp1 != NULL)
	{
		macro_data = (MacroNode *)(temp1->data);
		macro_name = macro_data->name;
		temp2 = head_symbols_list; 
		
		/* Iterate through the symbol list*/
		while (temp2 != NULL)  
		{
			symbol_data = (SymbolNode *)(temp2->data);
			
			/*Check if the macro name matches the symbol name*/
			if (strcmp(macro_name, symbol_data->name) == 0)
			{
				printf("Error in line number %d: Label and macro with the same name - '%s'\n", symbol_data->line_num, macro_name);
				return ERROR; /* Indicate failure */
			}
			
			temp2 = (node *)temp2->next;
		}
		
		temp1 = (node *)temp1->next;
	}

	return SUCCESS; /* Indicate success */
}



int check_entry_extern_conflict(node *head_symbols_list) 
{
	node *temp1 = head_symbols_list;
	node *temp2;
	char *symbol_name;
	SymbolNode *symbol_data1, *symbol_data2;


	while (temp1 != NULL) 
	{
		symbol_data1 = (SymbolNode *)(temp1->data);
        
        	if (symbol_data1->is_entry) 
        	{
            	symbol_name = symbol_data1->name;
            	temp2 = head_symbols_list;

            		while (temp2 != NULL) 
            		{
                		symbol_data2 = (SymbolNode *)(temp2->data);
                
                		/* Check for entry and extern conflict */
                		if (strcmp(symbol_data2->name, symbol_name) == 0 && symbol_data2->is_extern) 
                		{
                    			printf("Error in line number %d: Label '%s' is defined as both entry and extern.\n", symbol_data1->line_num, symbol_name);
                    			return ERROR; /* Indicate failure */
                		}
                
                		temp2 = (node *)temp2->next;
			}
		}
        
        	temp1 = (node *)temp1->next;
	}

	return SUCCESS; /* Indicate success */
}



int check_duplicate_labels(node *head_symbols_list) 
{
	node *current = head_symbols_list;
	
	while (current != NULL) 
	{
		SymbolNode *current_data = (SymbolNode *)current->data;
		node *checker = (node *)current->next;
		while (checker != NULL) 
		{
			SymbolNode *checker_data = (SymbolNode *)checker->data;
			
			/* Compare the names of the current and checker labels */
			if (strcmp(current_data->name, checker_data->name) == 0 )
			{
				/* Checking if the duplication in the names is of an external label and a regular label or two regular labels with the same name and printing an error message accordingly */
				if (current_data -> is_extern || checker_data -> is_extern)
				{
					printf("Error in line number %d: An extern label %s is defined in the current file.\n", current_data -> is_extern? current_data->line_num : checker_data->line_num, current_data -> is_extern? current_data->name : checker_data->name);
					return ERROR;
				}
				
				else
				{
	                		printf("Error in line number %d: Label %s is defined for the second time. A label name cannot be defined more than once.\n", current_data->line_num, current_data->name);
	                		return ERROR; /* Found a duplicate */
	                	}
			}
			checker = (node *)checker->next;
		}
        	current = (node *)current->next;
	}
	
	return SUCCESS; /* No duplicates found */
}



