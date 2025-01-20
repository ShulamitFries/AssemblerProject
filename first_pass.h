#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "linked_list.h"

#define MEMORY_START_ADDRESS 100
#define NUM_ADDRESSING_MODES 4
#define REG_LENGTH 2 
#define REG_BIT_LENGTH 3
#define NUMERIC_OP_LEN 12
#define TEMP_ENTRY_ADDRESS -1


typedef struct {
	char *code_word; 
	int adress;
	int line_num;   
} CodeNode;


typedef struct {
	char *name;
	int adress;
	int before_data;
	int is_entry;
	int is_extern;
	int line_num;
} SymbolNode;


typedef struct {
	char* name; /* Instruction name (e.g., "mov", "add"). */
	int num_operands; /* Number of operands required. */
	int source_methods[4]; /* Valid addressing modes for the source operand (0 = not allowed, 1 = allowed). */
	int target_methods[4]; /* Valid addressing modes for the target operand (0 = not allowed, 1 = allowed). */
} Instruction;




/**
 * Analyzes and processes an assembly language source file in the first pass of assembly.
 *
 * This function reads an assembly code file line by line and performs initial analysis to
 * identify labels, directives, and instructions. It handles the creation of symbol nodes,
 * data encoding, and instruction encoding based on the content of each line.
 *
 * The function also checks for various errors and warnings in the assembly code, such as:
 * - Incorrect label definitions
 * - Reserved words used as labels
 * - Invalid or misplaced directives
 *
 * The analysis updates the symbol list, data list, and instruction list accordingly.
 *
 * @param name_file The name of the assembly source file (excluding extension) to be analyzed.
 * @param head_symbols_list A pointer to the head of the linked list where symbol nodes will be added.
 * @param head_data_list A pointer to the head of the linked list where data nodes will be added.
 * @param head_instructions_list A pointer to the head of the linked list where instruction nodes will be added.
 *
 * @return An integer indicating whether errors were encountered during the analysis.
 *         - 0 if no errors were found.
 *         - 1 if errors were encountered.
 */
int first_pass_analyze(char *name_file, node **head_symbols_list, node **head_data_list, node **head_instructions_list);



/**
 * Creates a new symbol node and adds it to the linked list.
 *
 * This function allocates memory for a new symbol node, initializes it with the provided values,
 * and adds it to the end of the linked list. If memory allocation fails, the function prints an 
 * error message and exits the program.
 *
 * @param name The name of the symbol.
 * @param adress The address associated with the symbol.
 * @param before_data Indicates whether the symbol appears before the data section (1 if true, 0 if false).
 * @param is_entry Indicates whether the symbol is marked as an entry (1 if true, 0 if false).
 * @param is_extern Indicates whether the symbol is marked as external (1 if true, 0 if false).
 * @param head A pointer to the head of the linked list where the symbol node will be added.
 */
void crate_symbol_node(char *name, int adress, int before_data, int is_entry, int is_extern, node **head);



/**
 * Encodes data from a given line based on its type and updates the data list and symbols list.
 *
 * This function processes a line of assembly code, extracts data or symbols based on the type specified 
 * (e.g., "data", "string", "entry", "extern"), validates the content, and encodes it into the appropriate format. 
 * For "data" and "string" types, binary data nodes are created and added to the data list. For "entry" and "extern" types, 
 * symbol nodes are created and added to the symbols list. The function also updates the data counter (DC) as new data nodes are added.
 *
 * The function performs the following tasks:
 * - Skips over any whitespace or irrelevant characters.
 * - Validates numbers for "data" type to ensure they are whole numbers within the correct range.
 * - Converts numbers or string characters to binary format and adds them to the data list.
 * - Creates and adds symbol nodes for "entry" and "extern" types.
 * - Ensures proper formatting and syntax, returning an error if invalid data or strings are encountered.
 *
 * @param line The line of assembly code to be processed.
 * @param start_index A pointer to the current index in the line where parsing should begin.
 * @param data_type The type of data or directive (e.g., "data", "string", "entry", "extern").
 * @param DC The Data Counter, tracking the address for the data section.
 * @param head_data_list A pointer to the head of the data list for storing encoded data.
 * @param head_symbols_list A pointer to the head of the symbols list for storing symbols.
 *
 * @return The updated Data Counter (DC) after processing the line.
 */
int encoding_data(char *line, int *start, char *data_type, int DC, node **head_data_list, node **head_symbols_list);



/**
 * Extracts an integer number from a string starting at the specified index.
 *
 * This function extracts the first integer it encounters in the string `line`, starting from the
 * position indicated by `start_index`. It supports numbers with optional leading '+' or '-'
 * signs. The function updates `start_index` to the position immediately after the number.
 *
 * @param line The string from which the number is to be extracted.
 * @param start_index A pointer to the index in the string where the extraction should begin.
 *                    This value is updated to point to the character following the number.
 * @return The extracted integer.
 */
int extract_number(char *line, int *start_index);



/**
 * Creates a new code node (for data or instruction) and adds it to the linked list.
 *
 * This function allocates memory for a new code node, initializes it with the provided values,
 * and adds it to the end of the linked list. If memory allocation fails, the function prints an
 * error message and exits the program.
 *
 * @param code_word The code word (data or instruction) to be stored in the node.
 * @param adress The address associated with the code word.
 * @param head A pointer to the head of the linked list where the code node will be added.
 */
void crate_data_or_instruction_node(char *code_word, int adress, node **head);



/**
 * Encodes an assembly instruction into its machine code representation.
 *
 * This function processes a line of assembly code, extracts the operation name and its operands,
 * determines the addressing modes, validates them, and generates the corresponding machine code
 * words. The machine code words are then added to the instruction list.
 *
 * @param line The line of assembly code to be encoded.
 * @param start_index A pointer to the index in the line where encoding should start.
 * @param IC The current instruction counter.
 * @param head_instructions_list A pointer to the head of the list where the encoded instructions will be stored.
 *
 * @return The updated instruction counter (IC) after encoding the instruction, or `ERROR` if an error occurs.
 */
int encoding_instructions(char *line, int *start_index, int IC, node **head_instructions_list);



/**
 * Retrieves information about the addressing methods for a given instruction.
 *
 * This function populates the source and destination addressing methods for a specific instruction
 * from a predefined table. The table defines the legal addressing methods for each instruction's
 * source and destination operands. The index parameter is expected to be the index obtained from
 * the `find_op_name` function. The `op_names_table` and `instructions_table` are synchronized,
 * meaning each index in both arrays corresponds to the same operation.
 *
 * @param index The index of the instruction in the table, obtained from `find_op_name`.
 * @param source_methods An array to be populated with the legal addressing methods for the source operand.
 * @param dest_methods An array to be populated with the legal addressing methods for the destination operand.
 * @return The number of operands for the instruction.
 */
int information_operation(int index, int source_methods[], int dest_methods[]);



/**
 * Handles the extraction and validation of an operand from a line of assembly code.
 *
 * This function extracts an operand from the given line, checks its addressing mode,
 * and validates if it matches the allowed addressing methods for the operation.
 * If the operand is invalid or missing, the function will print an appropriate error
 * message and return `ERROR`.
 *
 * @param line The line of assembly code being processed.
 * @param start_index A pointer to the current index in the line, updated after extraction.
 * @param operand A pointer to a char pointer where the extracted operand will be stored.
 * @param addressing_methods An array indicating the valid addressing methods for the operation.
 *
 * @return The addressing mode of the operand if successful, or `ERROR` if an error occurs.
 */
int handle_operand(char *line, int *start_index, char **operand, int adressing_methods[]);



/**
 * Determines the addressing mode of the given operand.
 *
 * This function analyzes the operand and returns the corresponding addressing mode:
 * - Immediate addressing mode (0) if the operand starts with '#'.
 * - Indirect register mode (2) if the operand starts with '*' followed by a valid register name.
 * - Direct register mode (3) if the operand is a valid register.
 * - Direct mode (1) if the operand is a valid symbol (label).
 * 
 * If the operand is invalid, the function prints an error message and returns `ERROR`.
 *
 * @param operand The operand whose addressing mode is to be determined.
 * @return The addressing mode (0, 1, 2, 3), or `ERROR` if invalid.
 */
int find_addressing_mode(char *operand);



/**
 * Returns the code word of the operand based on the specified addressing mode.
 *
 * @param operand A string representing the operand to be processed.
 * @param addressing_mode An integer representing the addressing mode of the operand:
 *                        - 0: Immediate addressing mode (the operand is treated as an immediate value).
 *                        - 1: Direct addressing mode (the operand is treated as a direct address).
 *                        - 2: Indirect register addressing mode (the operand is treated as a register indirect address).
 *                        - 3: Direct register addressing mode (the operand is treated as a direct register address).
 * @param is_target_op An integer flag indicating whether the operand is a target operand.
 *                     This flag is used specifically in register addressing modes.
 * @return A string representing the additional code word of the operand based on the addressing mode.
*/
char *operand_encoding(char *operand, int addressing_mode, int is_target_op);



/**
 * Processes an operand in immediate addressing mode.
 *
 * @param operand A string representing the operand to be processed.
 *                The operand starts with '#' followed by the immediate number.
 * @return A string representing the code word generated for the immediate operand.
 *         The code word includes the binary representation of the immediate number and a suffix "100".
 *         - NULL if immediate number is invalid or memory allocation fails.
 */
char *immediate_addressing(char *operand);



/**
 * Processes an operand in register direct or indirect addressing mode.
 *
 * @param reg A string representing the register operand.
 *            The operand should be a number representing the register.
 * @param is_target_op An integer flag indicating whether the operand is a target operand.
 *                     If the operand is a target operand, it affects the code word format.
 * @return A string representing the code word generated for the register operand.
 *         The code word includes the binary representation of the register number and additional bits
 *         indicating the addressing mode.
 *         - NULL if memory allocation fails.
 */
char *register_addressing(char *reg, int is_target_op);



/**
 * Deletes memory allocated for a CodeNode structure.
 * 
 * This function frees the memory allocated for the code_word of the CodeNode
 * structure and then frees the CodeNode structure itself.
 * 
 * @param c A pointer to the CodeNode structure to be deleted.
 */
void delete_code_node(void *c);



/**
 * Deletes memory allocated for a SymbolNode structure.
 * 
 * This function frees the memory allocated for the naame of the SymbolNode
 * structure and then frees the SymbolNode structure itself.
 * 
 * @param s A pointer to the SymbolNode structure to be deleted.
 */
void delete_symbol_node(void *s);



#endif
