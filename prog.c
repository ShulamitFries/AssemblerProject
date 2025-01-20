#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils_and_checks.h"
#include "macro.h"
#include "first_pass.h"
#include "second_pass.h"



int main(int argc, char *argv[])
{
	int i, has_errors = 0;
	node *head_macro_list = NULL; /* empty list */
	node *head_symbols_list = NULL;
	node *head_instructions_list = NULL;
	node *head_data_list = NULL;


	for (i = 1; i < argc; i++)/*Iterate over each file passed as an argument*/
	{
		if (macro_analyze(argv[i], &head_macro_list) == ERROR)
			has_errors = 1;
		
		has_errors = first_pass_analyze(argv[i], &head_symbols_list, &head_data_list, &head_instructions_list);
		
		if (check_macro_symbol_conflict(head_macro_list, head_symbols_list) == ERROR)
			has_errors = 1;
		
		if (second_pass_analyze(argv[i], &head_instructions_list, &head_data_list, &head_symbols_list, has_errors) == ERROR)
			has_errors = 1;
		
		if (has_errors)
			printf("Errors were detected and therefore no output files are generated, sorry:(\n");
			

		free_list(&head_macro_list, delete_macro_node);
		free_list(&head_symbols_list, delete_code_node);
		free_list(&head_instructions_list, delete_code_node);
		free_list(&head_data_list, delete_symbol_node);

	}


	return 0;
}


