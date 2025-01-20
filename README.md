# AssemblerProject
This project implements an assembler for an assembly language defined within the project specifications. The assembler processes input files containing code written in the defined assembly language and generates the following output files for each input file:  

- **Object file**: Contains the binary machine code.  
- **Externals file**: Contains information about external symbols appearing in the code.  
- **Entries file**: Contains information about symbols declared as entry points in the code.  

### Operation  
The assembler operates in two passes:  
1. **First pass**: Scans the input file, builds a symbol table, and performs preliminary encoding of instructions and data.  
2. **Second pass**: Completes the encoding of instructions using the symbol table.  

### Features  
- **Full support for the assembly language specification**: The project supports all operations, directives, addressing methods, and other language features as defined in the project specifications.  
- **Error detection and handling**: The assembler identifies syntax errors in the assembly code and reports them clearly and in detail, including the line number in the source file.  
- **Macro support**: The assembler recognizes and processes macro definitions and usage, expanding macro code before performing the assembly.  
- **Output file generation**: The assembler produces three types of output files: object, externals, and entries.  

### Project Structure  
The code is modular, with components divided based on tasks:  
- First pass  
- Second pass  
- Helper functions  
- Symbol table  
- Memory map  
- Constant tables  

### Running the Assembler  
1. **Compilation**: Compile the project source files using the `make` command with the provided Makefile.  
2. **Execution**: Run the assembler's executable, passing the names of source files (with the `.as` extension) as command-line arguments.  

Example:  
```bash  
make  
./assembler file1.as file2.as  
```  

### Example  
**Sample input file (ps.as)**:  
```assembly  
; file ps.as  

.entry LIST  
.extern fn1  

MAIN: add r3, LIST  
jsr fn1  
LOOP: prn #48  
lea STR, r6  
inc r6  
mov *r6, L3  
sub r1, r4  
cmp r3, #-6  
bne END  
add r7, *r6  
clr K  
sub L3, L3  
.entry MAIN  
jmp LOOP  
END: stop  

STR: .string "abcd"  
LIST: .data 6, -9  
.data -100  
K: .data 31  
.extern L3  
```  

**After running the command**:  
```bash  
./assembler ps  
```  

The following files will be generated:  
- `ps.ob`  
- `ps.ext`  
- `ps.ent`  

### Documentation  
The code contains detailed comments explaining the algorithms and implementation.
