#ifndef UTILS_H
#define UTILS_H

#include "linked_list.h"
#include <stdio.h>

 
#define SUCCESS 1
#define ERROR -1 
#define EOS '\0'
#define NEW_LINE '\n'
#define CODE_WORD_LEN 15
#define MAX_LEN_LINE 82 /*The length of a line in the source file is a maximum of 80 characters (not including the \n character) + 1 place for EOS.*/
#define MAX_LEN_SYMBOL 31 
#define NUM_OP_NAMES 16


typedef struct {
	char *name;
	char *operation_code;
} Operation;
	
extern Operation op_names_table[];




/**
 * extract_word - Extracts a word from a given line starting from a specified index.
 * 
 * This function skips leading whitespace characters, extracts the next word (a sequence of 
 * non-whitespace characters) from the line, and returns it as a newly allocated string. 
 * It also updates the end index to point to the character immediately after the extracted word.
 * 
 * @param line: The input string from which the word is to be extracted.
 * @param start: A pointer to the starting index in the line to begin the extraction.
 * 
 * @return A newly allocated string containing the extracted word. The caller is responsible 
 *         for freeing this string.
 */
char *extract_word(char *line, int *start);



/**
 * Checks if the rest of the line contains only whitespace characters after a given index.
 *
 * @param line The line read from the file.
 * @param i The index to start checking from.
 * @return Returns SUCCESS if the rest of the line contains only whitespace characters, otherwise returns ERROR.
 */
int check_only_whitespace_after_index(char *line, int i); 
 
 

/**
 * Converts an integer to its binary string representation.
 *
 * @param num The integer to be converted to binary.
 * @param bits The length of the binary string representation in bits.
 *                    This specifies the number of bits in the resulting binary string.
 * @return A string representing the binary form of the integer.
 *         - The string is null-terminated.
 *         - NULL if memory allocation fails.
 */
char* int_to_binary(int num, int bits);



/**
 * Generates a full file name by appending a given extension to a base file name.
 *
 * @param base_name The base name of the file (without extension).
 * @param extension The desired extension for the file (e.g., "am").
 * @return A dynamically allocated string containing the full file name with the extension.
 */
char* generate_full_name(const char *base_name, const char *extension);



/**
 * Initializes a file with a given name, extension, and mode.
 *
 * @param fp A pointer to the file pointer that will be initialized.
 * @param name_file The base name of the file (without extension).
 * @param extension The desired extension for the file (e.g., "as", "am").
 * @param mode The mode in which to open the file (e.g., "r", "w+", etc.).
 */
void init_file(FILE **fp, const char *name_file, const char *extension, const char *mode);



/**
 * Finds the index of an operation name in the operation names table.
 *
 * @param op_name The name of the operation to find.
 * @return The index of the operation name in the table if found, otherwise returns ERROR.
 */
int find_op_index(char *op_name);



/**
 * Checks if a given string represents a valid register.
 *
 * @param reg A string representing the register to be checked.
 * @return An integer indicating whether the string is a valid register:
 *         - SUCCESS: Valid register (string is in the format 'rX' where X is a digit from 0 to 7).
 *         - ERROR: Invalid register.
 */
int is_register(char *reg);



/**
 * Checks if a given string represents a valid assembler directive.
 *
 * @param str A string representing the directive to be checked.
 * @return An integer indicating whether the string is a valid directive:
 *         - SUCCESS: Valid directive (string matches one of ".data", ".string", ".entry", or ".extern").
 *         - ERROR: Invalid directive.
 */
int is_directive(const char *str);



/**
 * Checks if a given string is a reserved word in the assembler.
 * Reserved words include directives, operation names, and register names.
 *
 * @param str A string representing the word to be checked.
 * @return An integer indicating whether the string is a reserved word:
 *         - SUCCESS: Reserved word (the string is a directive, operation name, or register name).
 *         - ERROR: Not a reserved word.
 */
int is_reserved_word(const char *str);
 
 
 
/**
 * Checks if a given number can be represented within a specified bit length using 2's complement representation.
 *
 * This function verifies whether the provided integer can be represented
 * within the range defined by the specified bit length in 2's complement representation.
 * The valid range for an N-bit 2's complement number is from -2^(N-1) to 2^(N-1) - 1.
 *
 * @param num The integer number to be checked.
 * @param bit_length The bit length for the 2's complement representation.
 * @return SUCCESS if the number is within the valid range for the specified bit length, ERROR otherwise.
 */
int is_valid_number(int num, int bit_length);



/**
 * Checks the number of commas and prints appropriate error messages if the number of commas is incorrect.
 *
 * This function verifies whether the number of commas matches the expected count. If the number of commas
 * does not match the expected count, it prints an appropriate error message.
 *
 * @param comma The count of commas to be checked.
 * @param expected_commas The expected count of commas.
 * @param line_num The line number in the input where the commas are being checked. Used for printing error messages.
 * @return SUCCESS if the number of commas matches the expected count, ERROR otherwise.
 */
int is_valid_comma_count(int comma, int expected_commas, int line_num);



/**
 * Checks if the given symbol name is valid.
 *
 * A valid label (symbol) is one that:
 * - Begins with an alphabetic character (either uppercase or lowercase).
 * - Contains only alphanumeric characters (letters and digits).
 * - Has a maximum length of 31 characters.
 *
 * Labels are symbols defined at the beginning of instruction lines or data/string directives (.data or .string).
 * 
 * @param symbol_name The name of the symbol to validate.
 * @return Returns SUCCESS if the symbol is valid, ERROR otherwise.
 */
int is_valid_symbol(char *symbol_name);



/**
 * Checks for naming conflicts between macros and symbols.
 *
 * This function iterates through the list of macros and the list of symbols to check if any macro 
 * has the same name as a symbol. If a conflict is found, it prints an error message and returns 
 * failure. If no conflicts are found, it returns success.
 *
 * @param head_macro_list A pointer to the head of the macro list.
 * @param head_symbols_list A pointer to the head of the symbol list.
 *
 * @return SUCCESS if no naming conflicts are found, or ERROR if a conflict is detected.
 */
int check_macro_symbol_conflict(node *head_macro_list, node *head_symbols_list);



/**
 * Checks for conflicts between entry and extern labels.
 * 
 * This function iterates through the list of symbols to check if any label is marked
 * as both `entry` and `extern`. If such a conflict is found, an error message is 
 * printed. 
 * 
 * @param head_symbols_list A pointer to the head of the symbol list.
 * @return SUCCESS if no conflicts are found, ERROR if conflicts are detected.
 */
int check_entry_extern_conflict(node *head_symbols_list);



/**
 * Checks for duplicate labels in a case-sensitive manner in a linked list of symbols.
 *
 * The function iterates through a linked list of symbols and compares each symbol name
 * with all subsequent symbol names to check for duplicates. Labels (symbols) are considered
 * case-sensitive, so "Label" and "label" are treated as different symbols.
 *
 * @param head_symbols_list A pointer to the head of the linked list containing the symbols.
 * @return SUCCESS if no duplicates found, ERROR otherwise.
 */
int check_duplicate_labels(node *head_symbols_list);



#endif
