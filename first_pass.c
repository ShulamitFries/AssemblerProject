#include "first_pass.h"
#include "utils_and_checks.h"
#include "linked_list.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


int line_num_m = 0; /* Global variable for line number in files with suffix m, (used for error messages). */

/* Global variable IC (Instruction Counter) initialized to 100 - MEMORY_START_ADDRESS. 
   This variable is used across multiple files to track the memory address of instructions. */
int IC = MEMORY_START_ADDRESS;

/* Global variable DC (Data Counter) initialized to 0.
   This variable is used across multiple files to track the memory address of data. */
int DC = 0;



int first_pass_analyze(char *name_file, node **head_symbols_list, node **head_data_list, node **head_instructions_list)
{
	int i = 0;
	int has_errors = 0, symbol_flag;
	char line[MAX_LEN_LINE], *first_field, *data_type;
	node *temp;
	FILE *f;
	
	/* Initialize input file */
	init_file(&f, name_file, ".am", "r");


	while (fgets(line, MAX_LEN_LINE, f))
	{
		line_num_m++;
		
	
		/* Skips the current loop iteration if the line contains only whitespace characters (spaces, tabs, carriage returns, newlines) or is empty. */	
		if (strspn(line, " \t\r\n") == strlen(line))
			continue;
		
		
		/* Skips the current loop iteration if the first character of the line is a semicolon (';'). 
		In this assembly language, lines starting with a semicolon are comments and should be ignored. */
		if (line[0] == ';')
			continue;
	
		i = 0;
		symbol_flag = 0;
		first_field = extract_word(line, &i);
	
		/* Checking if first_field is a definition of a symbol, by checking if the character that appears immediately after it is ':' */
		if (line[i] == ':')
		{
			/* Error checking that there is no white character next to ':' */
			if (!isspace(line[i+1]))
			{
				printf("Error in line number %d: A label with no whitespace after the ':'\n", line_num_m);
				has_errors = 1;
				free(first_field);
				continue;
			}
			
			
			i++; /* to skip the ':' */
			
			/* Error checking for setting a label at the top of the line and the rest of the line is empty */
			if (check_only_whitespace_after_index(line, i) == SUCCESS)
			{
				printf("Error in line number %d: A label at the top of the line and the rest of the line is empty\n", line_num_m);
				has_errors = 1;
				free(first_field);
				continue;
			}
			
			/* Error checking that the label name is a reserved word */
			if (is_reserved_word(first_field) == SUCCESS)
			{
				printf("Error in line number %d: Reserved words (name of a operation, directive or register) cannot also be used as a label name\n", line_num_m);
				has_errors = 1;
				free(first_field);
				continue;
			}
			
			symbol_flag = 1;
		}
		else
		{
			/* Error checking for parentheses not attached to the label name */
			while (isspace(line[i]))
				i++;
			
			if (line[i] == ':')
			{
				printf("Error in line number %d: A label definition must end with ':' and must be adjacent to the label name without any spaces\n", line_num_m);
				has_errors = 1;
				free(first_field);
				continue;
			}
			else
				i = 0;
		}
		
		/* Skip any whitespace. */	
		while (isspace(line[i]))
			i++;
			
		
		if (line[i] == '.')
		{
			i++; /* to skip the '.' */
			data_type = extract_word(line, &i); /* Extract the next word from the line as the data type. */
			
			if (symbol_flag)
			{
				/* Create a symbol node if the label is valid and not associated with an entry or extern directive. */
				if (is_valid_symbol(first_field) == ERROR)
                		{
                   		 	printf("Error in line number %d: Invalid label. A valid label begins with an alphabetic letter (uppercase or lowercase), followed by some series of alphabetic letters (uppercase or lowercase) and/or numbers. The maximum length of a label is 31 characters\n", line_num_m);
                    			has_errors = 1;
                    			free(first_field);
					free(data_type);
                    			continue;
                		}
                
                		/* The label defined at the beginning of the .entry or .extern line is meaningless and the assembler ignores this label */
                		else if (strcmp(data_type, "entry") == 0 || strcmp(data_type, "extern") == 0)
                		{
                    			printf("Warning: a label defined at the beginning of the .entry or .extern line is meaningless\n");
                		}
                
                		else
                    			crate_symbol_node(first_field, DC, 1, 0, 0, head_symbols_list);
				
			}

			/* Encode the data from the line. If an error occurs, mark it and continue to the next line. */
			DC = encoding_data(line, &i, data_type, DC, head_data_list, head_symbols_list);
			if (DC == ERROR)
			{
				has_errors = 1;
				free(first_field);
				free(data_type);
				continue;
			}
			
			free(data_type);
		}
		else
		{		
			if (symbol_flag)
			{
				/* Create a symbol node if the label is valid */
				if (is_valid_symbol(first_field) == ERROR)
                		{
                   		 	printf("Error in line number %d: Invalid label. A valid label begins with an alphabetic letter (uppercase or lowercase), followed by some series of alphabetic letters (uppercase or lowercase) and/or numbers. The maximum length of a label is 31 characters\n", line_num_m);
                    			has_errors = 1;
                    			free(first_field);
                    			continue;
                		}
                		else
					crate_symbol_node(first_field, IC, 0, 0, 0, head_symbols_list);
			}
			
			/* Encode the instructions from the line. If an error occurs, mark it and continue to the next line. */
			IC = encoding_instructions(line, &i, IC, head_instructions_list);
			if (IC == ERROR)
			{
				has_errors = 1;
				free(first_field);
				continue;
			}
		}
		
		free(first_field);
		
	}


	/* If there are no errors, adjust the addresses for data and symbols before data (If there are errors then no output files are created, so there is no point in the address being updated) */
	if (!has_errors)
	{
		/* Iterate through the data list */
		temp = *head_data_list;
		while(temp != NULL)
		{	
			/* Update the address for each data node by adding IC */
			((CodeNode *)(temp->data))->adress += IC;
			temp = (node *)temp->next;
		}
	
		/* Iterate through the symbols list */
		temp = *head_symbols_list;
		while(temp != NULL)
		{	
			/* Update the address for each symbol marked as 'before_data' */
			if(((SymbolNode *)(temp->data))->before_data)
				((SymbolNode *)(temp->data))->adress += IC;
			temp = (node *)temp->next;
		}
	}
	
	
	return has_errors;
}



void crate_symbol_node(char *new_name, int adress, int before_data, int is_entry, int is_extern, node **head)
{
	/* Allocate memory for a new symbol node */
	SymbolNode *node_data = (SymbolNode *)malloc(sizeof(SymbolNode));
	
	if (!node_data) 
	{
		printf("Allocation failure\n");
		exit(1);
	}
	
	/*Initialize the symbol node with the provided values */
	node_data->name = (char *)malloc(strlen(new_name) + 1);
	if (!node_data->name)
	{
        	printf("Allocation failure\n");
        	free(node_data);
		exit(1);
	}
	strcpy(node_data->name, new_name);

	node_data->adress = adress;
	node_data->before_data = before_data;
	node_data->is_entry = is_entry;
	node_data->is_extern = is_extern;
	node_data ->line_num = line_num_m;

	/* Add the node to the end of the symbol linked list */
	add_node_end(head, (void *)node_data, delete_symbol_node);
}




int encoding_data(char *line, int *start_index, char *data_type, int DC, node **head_data_list, node **head_symbols_list)
{
	char *data, *symbol_name;
	int i = *start_index, num, comma = 0;

	
	if (strcmp(data_type, "data") == 0)
	{
		/* Skip over any spaces or commas in the line, count the commas */
		while (isspace(line[i]) || line[i] == ',')
		{
			if (line[i] == ',')
				comma++;
			i++;
		}

		/* Validate that there is no comma = the number of commas is 0 before the first number */
		if (is_valid_comma_count(comma, 0, line_num_m) == ERROR)
			return ERROR;
			
		while (line[i])
		{
	
			num = extract_number(line, &i);	
			
			/* Validate that the number is valid: it must be a whole number and within the correct range. */
			if (is_valid_number(num, CODE_WORD_LEN) == ERROR || line[i] == '.')
			{
				printf("Error in line number %d: Invalid number\n", line_num_m);
				return ERROR;
			}
			
			data = int_to_binary(num, CODE_WORD_LEN); /*  Convert the number to a binary string. */
			crate_data_or_instruction_node(data, DC, head_data_list); /* Create a data node with the binary string and add it to the data list. */	
			DC++;
			free(data);
			
			comma = 0;
			while (isspace(line[i]) || line[i] == ',')
			{
				if(line[i] == ',')
					comma++;
				i++;
			}

			
			if (line[i])
			{
				/* Validate that there is exactly 1 comma between the numbers */
				if (is_valid_comma_count(comma, 1, line_num_m) == ERROR)
					return ERROR;
			}
			else /* end of line */
			{
				/* Validate that there is no comma = the number of commas is 0 after the last number */
				if (is_valid_comma_count(comma, 0, line_num_m) == ERROR)
					return ERROR;
			}	
		}
		
					
		/* Check for extra characters after the current index */
		if (check_only_whitespace_after_index(line, i) == ERROR)
		{
			printf("Error in line number %d: Extra characters at the end of a line\n", line_num_m);
			return ERROR;
		}
		
	}
	else if (strcmp(data_type, "string") == 0)
	{
		/* Skip any leading whitespace. */
		while (isspace(line[i] ))
			i++;
		
		/* Check for the opening double quote of the string. */	
		if (line[i] == '"')
			i++; /* Move past the opening quote. */
		else
		{
			/*  If no opening quote is found, return an error. */
			printf("Error in line number %d: Invalid string\n", line_num_m);
			return ERROR;
		}
		
		/* Loop through the characters of the string. */
		while (isalpha(line[i]) || isspace(line[i]))
		{
			/* Process each non-whitespace character. */
			if (!isspace(line[i]))
			{
				/* Convert the character to its binary form and create a data node. */
				data = int_to_binary((int)line[i], CODE_WORD_LEN);
				crate_data_or_instruction_node(data, DC, head_data_list);
				DC++;
				free(data);
			}
			i++;	
		}
		
		/* Check for the closing double quote of the string. */
		if (line[i] == '"')
			i++;
		else
		{
			/* If no closing quote is found, return an error. */
			printf("Error in line number %d: Invalid string\n", line_num_m);
			return ERROR;
		}
		
		
		/* Add the string terminator (null character in binary form) as a data node. */
		crate_data_or_instruction_node("000000000000000", DC, head_data_list);
		DC++;
		
		/* Check for extra characters after the current index */
		if (check_only_whitespace_after_index(line, i) == ERROR)
		{
			printf("Error in line number %d: The '.string' directive accepts only one string\n", line_num_m);
			return ERROR;
		}
		
	}
	else if (strcmp(data_type, "entry") == 0)
	{
		symbol_name = extract_word(line, &i);
		
		/* Check for extra characters after the current index */
		if (check_only_whitespace_after_index(line, i) == ERROR)
		{
			printf("Error in line number %d: The directive '.entry' accepts only one parameter\n", line_num_m);
			return ERROR;
		}
		
		crate_symbol_node(symbol_name, TEMP_ENTRY_ADDRESS, 0, 1, 0, head_symbols_list);
	}
	else if (strcmp(data_type, "extern") == 0)
	{
		symbol_name = extract_word(line, &i);
		
		/* Check for extra characters after the current index */
		if (check_only_whitespace_after_index(line, i) == ERROR)
		{
			printf("Error in line number %d: The directive '.extern' accepts only one parameter\n", line_num_m);
			return ERROR;
		}
		
		crate_symbol_node(symbol_name, 0, 0, 0, 1, head_symbols_list);
	}

	
	*start_index = i;
	return DC;
}



int extract_number(char *line, int *start_index) 
{
	int i = *start_index, num = 0, sign = 1;
	
	/* Skip leading whitespace characters */
	while (isspace(line[i]))
		i++;
	
	/*  Check for a sign (+ or -) */
	if (line[i] == '-')
	{
		sign = -1;
		i++;
	}
	else if (line[*start_index] == '+')
        	i++;
        	
        /* Extract digits to form the number */
	while (isdigit(line[i]))
	{	
		num = num*10 + (line[i] - '0');
		i++;
	}
	
	/* Apply the sign to the extracted number */
	num *= sign;
	
	/* Update the start index to the position after the number */
	*start_index = i;
	
	
	return num;
}



void crate_data_or_instruction_node(char *code_word, int adress, node **head)
{
	/* Allocate memory for a new data node */
	CodeNode *node_data = (CodeNode *)malloc(sizeof(CodeNode));
	
	if (!node_data) 
	{
		printf("Allocation failure\n");
		exit(1);
	}

	/* Initialize the data node with the provided values */
	node_data->code_word = (char *)malloc(strlen(code_word) + 1);
	if (!node_data->code_word)
	{
        	printf("Allocation failure\n");
        	free(node_data);
		exit(1);
	}
	strcpy(node_data->code_word, code_word);
	
	node_data->adress = adress;
	node_data->line_num = line_num_m;

	/* Add the node to the end of the data linked list */
	add_node_end(head,(void *)node_data, delete_code_node);
}



int encoding_instructions(char *line, int *start_index, int IC, node **head_instructions_list)
{
	char *first_code_word, *second_code_word = NULL, *third_code_word = NULL;
	char *operation_name, *op_code, *first_operand, *second_operand;
	int i = *start_index, op_code_index, num_operands, comma = 0;
	int addressing_mode1, addressing_mode2;
	int source_methods[NUM_ADDRESSING_MODES] = {0}, target_methods[NUM_ADDRESSING_MODES] = {0};
	const char *addressing_mode_coding[] = {"0001", "0010", "0100", "1000"};

	/* Allocate memory for the first code word */
	first_code_word = (char *)malloc(CODE_WORD_LEN * sizeof(char));
	if (!first_code_word)
	{
		printf("Allocation failure\n");
		exit(1);
	}
	
	/* Skip initial whitespaces and commas before the first operand */
	while (line[i] && (isspace(line[i]) || line[i] == ','))
	{
		if (line[i] == ',')
			comma++;
		i++;
	}

	/* Validate that there is no comma = the number of commas is 0 before the first operand */
	if (is_valid_comma_count(comma, 0, line_num_m) == ERROR)
		return ERROR; 
		
	
	/* Extract operation name and find its index in the operation names table */
	operation_name = extract_word(line, &i);
	op_code_index = find_op_index(operation_name);
	
	/* Error checking for action name that does not exist */
	if (op_code_index == ERROR)
	{
		printf("Error in line number %d: Operation name '%s' does not exist. \nNote that the function name and the first operand are separated with white characters\n", line_num_m, operation_name);
		free(operation_name);	
		return ERROR;
	}
	
	free(operation_name);

	/* Get the operation code and initialize the first code word */
	op_code = op_names_table[op_code_index].operation_code;
	strcpy(first_code_word, op_code);
	
	/* Get the number of operands and their addressing methods */
	 num_operands = information_operation(op_code_index, source_methods, target_methods);
		
	
	/* Handle encoding based on the number of operands */			
	switch (num_operands) {
		case 0:
			/* No operands: . The fields of the source operand and the destination operand in the first word of the instruction are unused, and therefore will be zeroed */
			strcat(first_code_word, "00000000");
			break;
		
		case 1:	
			/* Handle a single operand, find its addressing mode, and encode it in the first code word */		
			addressing_mode1 = handle_operand(line, &i, &first_operand, target_methods);
			if (addressing_mode1 == ERROR)
			{
				free(first_operand);
				return ERROR;
			}
			
			/* , the field of the source operand (bits 7-10) in the first word of the instruction encoding is meaningless, and therefore will contain zeros */	
			strcat(first_code_word, "0000");
			strcat(first_code_word, addressing_mode_coding[addressing_mode1]);
			
			/* Encode the operand itself in a separate code word */
			second_code_word = operand_encoding(first_operand, addressing_mode1, 0);
			if (second_code_word == NULL)
			{
				free(first_operand);
				return ERROR;
			}
			
			free(first_operand);	
			break;
		case 2:
			/* Handle two operands, starting with the source operand */
			addressing_mode1 = handle_operand(line, &i, &first_operand, source_methods);
			if (addressing_mode1 == ERROR)
			{
				free(first_operand);
				return ERROR;
			}
				
			/* Append addressing mode of the source operand to the first code word */	
			strcat(first_code_word, addressing_mode_coding[addressing_mode1]);
			
			/* Encode the source operand in a separate code word */
			second_code_word = operand_encoding(first_operand, addressing_mode1, 0);
			if (second_code_word == NULL)
			{
				free(first_operand);
				return ERROR;
			}
			
			free(first_operand);
	
			/* Skip whitespace or commas before the second operand */
			comma = 0;	
			while (line[i] && (isspace(line[i]) || line[i] == ','))
			{
				if (line[i] == ',')
					comma++;
				i++;
			}

			/* Validate that there is exactly 1 comma between the operands */
			if (is_valid_comma_count(comma, 1, line_num_m) == ERROR)
				return ERROR;
			
			/* Handle the target operand and find its addressing mode */
			addressing_mode2 = handle_operand(line, &i, &second_operand, target_methods);	
			if (addressing_mode2 == ERROR)
			{
				free(second_operand);
				return ERROR;
			}
				
			
			/* Append addressing mode of the target operand to the first code word */	
			strcat(first_code_word, addressing_mode_coding[addressing_mode2]);
			
			/* Encode the target operand in a separate code word */
			third_code_word = operand_encoding(second_operand, addressing_mode2, 1);
			if (third_code_word == NULL)
			{
				free(second_operand);
				return ERROR;
			}
			
			free(second_operand);
			
			/* Special case: if both operands use indirect or direct register addressing (addressing_mode 2 or 3), 
			   they can be encoded in the same code word, so we merge them */
			if((addressing_mode1 == 2 || addressing_mode1 == 3) && (addressing_mode2 == 2 || addressing_mode2 == 3))
			{
				second_code_word[9] = third_code_word[9];
				second_code_word[10] = third_code_word[10];
				second_code_word[11] = third_code_word[11];
				third_code_word = NULL; /*  The third code word is no longer needed */
			}
				
			break;
			
	}
	
			
	
	/* Skip any whitespace or commas after the last operand */
	comma = 0;	
	while (line[i] && (isspace(line[i]) || line[i] == ','))
	{
		if (line[i] == ',')
			comma++;
		i++;
	}
	
	/* Validate that there is no comma = the number of commas is 0 after the last operand */
	if (is_valid_comma_count(comma, 0, line_num_m) == ERROR)
		return ERROR;

	/* Check for extra operand after the current index */
	if (check_only_whitespace_after_index(line, i) == ERROR)
	{
		printf("Error in line number %d: Extra operand\n", line_num_m);
		return ERROR;
	}				
	
	/* Set the ARE field (A = 1, R = 0, E = 0) for the first code word as this is an absolute instruction */
	strcat(first_code_word, "100");
	
	/* Create a node for the first code word and add it to the instructions list */
	crate_data_or_instruction_node(first_code_word, IC, head_instructions_list);
	IC++;
	
	/* If there is a second code word (for the first operand), create a node and add it to the list */
	if (second_code_word)
	{
		crate_data_or_instruction_node(second_code_word, IC, head_instructions_list);
		IC++;
	}	
	
	/* If there is a third code word (for the second operand), create a node and add it to the list */
	if (third_code_word)
	{
		crate_data_or_instruction_node(third_code_word, IC, head_instructions_list);
		IC++;
	}
	
	
	/* Free allocated memory */
	free(first_code_word);
	free(second_code_word);
	free(third_code_word);
	
	return IC; /* Return the updated instruction counter */
}



int information_operation(int index, int source_methods[], int target_methods[])
{
	int i;
	/* Define a table of instructions with their respective properties. */
	Instruction instructions_table[] = {
        {"mov", 2, {1, 1, 1, 1}, {0, 1, 1, 1}},
        {"cmp", 2, {1, 1, 1, 1}, {1, 1, 1, 1}},
        {"add", 2, {1, 1, 1, 1}, {0, 1, 1, 1}},
        {"sub", 2, {1, 1, 1, 1}, {0, 1, 1, 1}},
        {"lea", 2, {0, 1, 0, 0}, {0, 1, 1, 1}},
        {"clr", 1, {0, 0, 0, 0}, {0, 1, 1, 1}},
        {"not", 1, {0, 0, 0, 0}, {0, 1, 1, 1}},
        {"inc", 1, {0, 0, 0, 0}, {0, 1, 1, 1}},
        {"dec", 1, {0, 0, 0, 0}, {0, 1, 1, 1}},
        {"jmp", 1, {0, 0, 0, 0}, {0, 1, 1, 0}},
        {"bne", 1, {0, 0, 0, 0}, {0, 1, 1, 0}},
        {"red", 1, {0, 0, 0, 0}, {0, 1, 1, 1}},
        {"prn", 1, {0, 0, 0, 0}, {1, 1, 1, 1}},
        {"jsr", 1, {0, 0, 0, 0}, {0, 1, 1, 0}},
        {"rts", 0, {0, 0, 0, 0}, {0, 0, 0, 0}},
        {"stop", 0, {0, 0, 0, 0}, {0, 0, 0, 0}}
	};
    
    
	/*Directly copy from instructions_table to passed arrays*/
	for (i = 0; i < NUM_ADDRESSING_MODES; i++) 
	{
        	source_methods[i] = instructions_table[index].source_methods[i];
        	target_methods[i] = instructions_table[index].target_methods[i];
	}
    
	
	/* Return the number of operands for the instruction. */		
	return instructions_table[index].num_operands;
}



int handle_operand(char *line, int *start_index, char **operand, int adressing_methods[])
{
	int i = *start_index;
	int addressing_mode;
	
	
	/* Check for missing operand after the current index */
	if (check_only_whitespace_after_index(line, i) == SUCCESS)
	{
		printf("Error in line number %d: Missing operand\n", line_num_m);
		return ERROR;
	}
	
	/* Extract the next operand from the line */		
	*operand = extract_word(line, &i);
	
	/* Determine the addressing mode of the operand */		
	addressing_mode = find_addressing_mode(*operand);
	
	/* If an error occurs during addressing mode determination, return ERROR for the analyzing of the input line to finish*/		
	if (addressing_mode == ERROR)
		return ERROR;
	
	/* Validate that the addressing mode is allowed for the operation */	
	if (adressing_methods[addressing_mode] == 0)
	{
		printf("Error in line number %d: An operand type that does not match the operation\n", line_num_m);
		return ERROR;
	}
	
	
	*start_index = i; /* Update the start index after processing */
	return addressing_mode;
}



int find_addressing_mode(char *operand)
{
	/* If the operand starts with '#', it's an immediate addressing mode */
	if (operand[0] == '#')
		return 0;
		
	/* If the operand starts with '*', it could be an indirect register mode */
	if (operand[0] == '*')/* Check if the rest of the operand is a valid register */
	{
		if (is_register(operand+1) == SUCCESS)
			return 2;/* Indirect register mode */
			
		else /* Invalid register name */	
		{
			printf("Error in line number %d: Invalid register name\n", line_num_m);
			return ERROR;
		}	
	}
	
	/* Check if the operand is a valid register for direct register mode */
	if (is_register(operand) == SUCCESS)
		return 3;
		
	/* If the operand is a valid symbol, it's in direct mode 
	This check is performed last because a register name can be a valid label name. */
	if (is_valid_symbol(operand) == SUCCESS)
		return 1;
	
	
	/* If no valid addressing mode is found, return an error */
	printf("Error in line number %d: Addressing method does not exist\n", line_num_m);	
	return ERROR;
}



char *operand_encoding(char *operand, int addressing_mode, int is_target_op)
{
	char * copy_operand;
	
	switch (addressing_mode) {
		case 0:
			return immediate_addressing(operand);
		case 1:
			copy_operand = (char *)malloc(sizeof(operand));
			if (!copy_operand)
			{
				printf("Allocation failure\n");
				exit(1);
			}
			strcpy(copy_operand, operand);
			
			return copy_operand;/* The copy of the label name is returned so that it can be replaced with this label address in a second pass */
		case 2:
			return register_addressing(operand+1, is_target_op);/* +1 because the first character is '*' which represents the indirect register addressing mode */
		case 3:
			return register_addressing(operand, is_target_op);
	}

	return NULL;
}



char *immediate_addressing(char *operand)
{
	char *code_word, *num_in_binary;
	int num;
	
	/* Ensure the number is a valid integer */
	if (strchr(operand + 1, '.') != NULL) 
	{
		printf("Error in line number %d:  Invalid number, not an integer number\n", line_num_m);
		return NULL;
	}
	/*if (num != (int)num)*/
	
	num = atoi(operand+1);/* Convert the operand (excluding the '#') to an integer */
	
	/* Check if the number is within the valid range */
	if (is_valid_number(num, NUMERIC_OP_LEN) == ERROR)
	{
		printf("Error in line number %d:  Invalid number, out of range\n", line_num_m);
		return NULL;
	}
	
	
	
	/* Allocate memory for the code word (16 characters) */
	code_word = (char *)malloc(CODE_WORD_LEN * sizeof(char));
	if (!code_word)
	{
		printf("Allocation failure\n");
		exit(1);
	}
	
	/* Convert the immediate number to a binary string (12 bits) */
	num_in_binary = int_to_binary(num, NUMERIC_OP_LEN);
	/* Copy the immediate number (excluding the '#') into code_word */
	strcpy(code_word, num_in_binary); 
	/* Append "100" to the code_word, because in immediate addressing, the value of the A bit is 1, and the other two bits are set to zero */
	strcat(code_word, "100"); 
	
	
	free(num_in_binary);	
	 return code_word;
}



char *register_addressing(char *reg, int is_target_op)
{
	int reg_num = atoi(reg+1);/* Convert the register number from string to integer, +1 to skip the 'r' and get to the register number*/
	char *reg_in_binary, *code_word;
	
	/* Allocate memory for the code word (16 characters) */
	code_word = (char *)malloc(CODE_WORD_LEN * sizeof(char));
	if (!code_word)
	{
		printf("Allocation failure\n");
		exit(1);
	}
	
	/* Convert the register number to a binary string (3 bits) */
	reg_in_binary = int_to_binary(reg_num, REG_BIT_LENGTH);

	/* If the operand is a target operand, the additional information word of the command will contain in bits 3-5 the number of the register that is used as a pointer. */
	if (is_target_op)
	{
		strcpy(code_word, "000000000");
		strcat(code_word, reg_in_binary);
	}
	/* If the register is a source operand (= not a target operand), the register number will be encoded in bits 6-8 of the additional data word */
	else
	{
		strcpy(code_word, "000000");
		strcat(code_word, reg_in_binary);
		strcat(code_word, "000");
	}
	/* Append "100" to the code_word, because in register addressing, the value of the A bit is 1, and the other two bits are set to zero */
	strcat(code_word, "100");
	
	
	free(reg_in_binary);	
	return code_word;
}



void delete_code_node(void *c)
{
	CodeNode *data_code_node = (CodeNode *)c;
	free(data_code_node -> code_word);
	free(data_code_node);
}



void delete_symbol_node(void *s)
{
	SymbolNode *data_symbol_node = (SymbolNode *)s;
	free(data_symbol_node -> name);
	free(data_symbol_node);
}

