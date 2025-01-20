#ifndef MACRO_H
#define MACRO_H

#include "linked_list.h"


typedef struct {
	char *name;
	char *content;
} MacroNode;



/**
 * Analyzes a file, handling macros and replacing them with their content.
 *
 * This function reads a file line by line, processes macros defined in the file, and writes the output
 * to a new file. If a macro definition is found, it is processed and its content is stored in a linked list.
 * Any occurrence of the macro in the subsequent lines is replaced with its content. If errors are found in
 * the macro definitions, appropriate error messages are printed and the function returns an error code.
 *
 * @param name_file The name of the file to be analyzed.
 * @param head_macro_list A pointer to a pointer to the head of the linked list that stores macros.
 *                        This list will be updated with the macros found in the file.
 * @return Returns SUCCESS if the file was processed correctly and ERROR if there was an error
 *         during processing (e.g., invalid macro definition or file operation failure).
 */
int macro_analyze(char * name_file, node **head_macro_list);




/**
 * Handles the processing of a macro within a file.
 *
 * This function reads lines from the file pointed to by `fp`, starting from the current
 * position, and processes the content of a macro until an "endmacr" directive is encountered.
 * It then creates a node with the macro's name and content and adds it to the linked list.
 *
 * @param fp Pointer to the file pointer.
 * @param macro_name Name of the macro being processed.
 * @param head Pointer to the head of the linked list.
 * @return Returns SUCCESS if the macro was successfully processed, otherwise returns ERROR.
 */
int handle_macro(FILE ** fp, char *macro_name, node **head);



/**
 * create_node - Creates a new node with macro name and content, and adds it to the linked list.
 * 
 * This function allocates memory for a new data_macro structure, assigns the macro name and content
 * to it, creates a new node with this data, and adds the node to the end of the linked list.
 * 
 * @param macro_name: The name of the macro.
 * @param macro_content: The content of the macro.
 * @param head: Pointer to the pointer of the head of the linked list.
 */
void create_node(char *macro_name, char *macro_content, node **head);



/**
 * find_macro - Searches for a macro name in a linked list and returns its content.
 * 
 * This function traverses a linked list of nodes, each containing a data_macro structure,
 * and looks for a macro with a specified name. If found, it returns the content of the macro.
 * If the macro is not found, it returns NULL.
 * 
 * @param first_field: The name of the macro to be searched.
 * @param head: A pointer to the head of the linked list.
 * 
 * @return The content of the found macro, or NULL if the macro is not found.
 */
char * find_macro(char * first_field, node **head);



/**
 * Deletes memory allocated for a MacroNode structure.
 * 
 * This function frees the memory allocated for the content of the MacroNode
 * structure and then frees the MacroNode structure itself.
 * 
 * @param m A pointer to the MacroNode structure to be deleted.
 */
void delete_macro_node(void *m);


#endif 
