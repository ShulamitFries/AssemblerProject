#ifndef SECOND_PASS_H
#define SECOND_PASS_H

#include "linked_list.h"
#include "first_pass.h"


typedef struct ExternSymbol {
    char *name;    /* The external symbol name */
    int address;   /* The address at which the external symbol is referenced */
} ExternSymbolNode;



/**
 * Performs the second pass analysis on the instructions and symbols lists.
 * 
 * This function iterates through the instructions list and updates the code words
 * with the corresponding addresses from the symbols list.
 * Additionally, it creates an object file with the extension `.ob` where each line
 * contains the address and content of a memory word. If there are external labels, it
 * creates a file with the extension `.ext` where each line contains the name of an 
 * external symbol and the address in the machine code that refers to this symbol. 
 * For each internal entry label, it creates a file with the extension `.ent` where each
 * line contains the name of the symbol defined as `entry` and its value as found in the
 * symbol table. Values are represented in decimal format.
 * 
 * @param head_instructions_list Pointer to the head of the instructions list.
 * @param head_data_list Pointer to the head of the data list.
 * @param head_symbols_list Pointer to the head of the symbols list.
 * @param name_file The name of the source file.
 * @return SUCCESS if the analysis is completed successfully, ERROR otherwise.
 */
int second_pass_analyze(char *name_file, node **head_instructions_list, node **head_data_list, node **head_symbols_list, int has_errors);



/**
 * Merges entry labels with their corresponding definitions.
 * 
 * This function processes the list of symbols to handle labels marked as `entry`.
 * An entry label is one that is defined within the current source file, but is 
 * intended to be used as an operand in instructions found in other source files.
 * 
 * For each label marked as `entry`, the function searches the symbol list for 
 * a label with the same name. If a matching label is found that is not marked 
 * as `entry`, it updates the matching label to be an entry label and deletes the 
 * original entry label. If no matching label is found, an error message is printed 
 * indicating that the entry label is not defined.
 * 
 * The function also removes the original entry label from the list after it has 
 * been merged with the corresponding label, if applicable.
 * 
 * @param head_symbols_list A double pointer to the head of the symbol list.
 * @return SUCCESS if the analysis is completed successfully, ERROR otherwise.
 */
int merge_entry_labels(node **head_symbols_list);



/**
 * update_code_words - Updates code words in the instructions list with symbol addresses.
 * 
 * This function iterates through the instructions list and updates the code words with
 * the corresponding symbol addresses from the symbols list.  If a symbol is marked as 
 * extern, the function adds the appropriate suffix to the code word and adds the symbol 
 * and address to the extern symbols list.
 * 
 * @param head_instructions_list Pointer to the head of the instructions linked list.
 * @param head_symbols_list Pointer to the head of the symbols linked list.
 * @param extern_symbols_list Pointer to the head of the extern symbols linked list.
 * @return SUCCESS if the analysis is completed successfully, ERROR otherwise.
 */
int update_code_words(node **head_instructions_list, node **head_symbols_list, node **extern_symbols_list);



/**
 * Adds a new extern symbol node to the extern symbols linked list.
 *
 * This function creates a new node for an extern symbol, initializes it with the provided 
 * symbol name and address, and adds it to the end of the extern symbols linked list. 
 * If memory allocation fails, the function prints an error message and exits the program.
 *
 * @param name The name of the extern symbol to be added.
 * @param adress The address associated with the extern symbol.
 * @param extern_symbols_list A pointer to the head of the extern symbols linked list.
 */
void crate_extern_node(node **head, char *name, int address);



/**
 * Creates an externals file based on the instructions and symbols lists.
 * 
 * This function generates a file with the extension `.ext` containing the external symbols and their addresses
 * based on the instructions and symbols lists. Each line in the file contains the name of an external symbol and the
 * address in the machine code that refers to this symbol. Multiple addresses in the machine code may refer to the 
 * same external symbol. Each such reference has a separate line in the externals file. Addresses are represented in 
 * decimal format.
 * 
 * @param name_file The base name of the source file.
 * @param head_instructions_list Pointer to the head of the instructions list.
 * @param head_symbols_list Pointer to the head of the symbols list.
 */
void create_object_file(char * name_file, node *head_instructions_list, node *head_data_list);



/**
 * Converts a 15-character binary string to a 5-digit octal string with leading zeros.
 *
 * @param binary The input binary string (must be 15 characters long).
 * @param octal The output string where the octal representation will be stored.
 *              Ensure the string has enough space to hold 6 characters
 *              (5 digits + null terminator).
 */
void binary_to_octal(const char *binary, char *octal);



/**
 * count_entry_symbols - Counts the number of entry symbols in the symbols list.
 * 
 * This function iterates through the symbols list and counts the number of symbols
 * marked as entry.
 * 
 * @param head_symbols_list Pointer to the head of the symbols linked list.
 * @return The number of extern symbols.
 */
int count_entry_symbols(node *head_symbols_list);



/**
 * Creates an entry file based on the symbols list.
 * 
 * This function generates a file with the extension `.ent` containing the entry symbols and their addresses
 * based on the symbols list. Each line in the file contains the name of a symbol defined as `entry` and its value
 * as found in the symbol table. Values are represented in decimal format.
 * 
 * @param name_file The base name of the source file.
 * @param head_symbols_list Pointer to the head of the symbols list.
 */
void create_entry_files(char *name_file, node *head_symbols_list);



/**
 * Creates an external file based on the external symbols list.
 * 
 * This function generates the .ext file containing the name of each extern symbol
 * and the corresponding address where it is used in the machine code.
 * 
 * @param name_file The base name of the source file (without extension).
 * @param extern_symbols_list Pointer to the head of the extern symbols linked list.
 */
void create_extern_files(char *name_file, node *head_extern_symbols);



/**
 * Deletes memory allocated for a ExternSymbolNode structure.
 * 
 * This function frees the ExternSymbolNode structure itself,
 * Because no field of ExternSymbolNode was dynamically allocated
 * 
 * @param e A pointer to the SymbolNode structure to be deleted.
 */
void delete_extern_symbol_node(void *e);


 
#endif
