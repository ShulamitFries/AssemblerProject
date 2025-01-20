assembler: prog.o utils_and_checks.o macro.o first_pass.o second_pass.o linked_list.o 
	gcc -g -ansi -pedantic -Wall prog.o utils_and_checks.o macro.o first_pass.o second_pass.o linked_list.o -o assembler -lm
prog.o: prog.c
	gcc -c -g -ansi -pedantic -Wall prog.c -o prog.o
utils_and_checks.o: utils_and_checks.c utils_and_checks.h first_pass.h macro.h
	gcc -c -g -ansi -pedantic -Wall utils_and_checks.c -o utils_and_checks.o -lm
macro.o: macro.c macro.h linked_list.h utils_and_checks.h
	gcc -c -g -ansi -pedantic -Wall macro.c -o macro.o 
first_pass.o: first_pass.c first_pass.h linked_list.h utils_and_checks.h
	gcc -c -g -ansi -pedantic -Wall first_pass.c -o first_pass.o 
second_pass.o: second_pass.c second_pass.h linked_list.h first_pass.h utils_and_checks.h
	gcc -c -g -ansi -pedantic -Wall second_pass.c -o second_pass.o
linked_list.o: linked_list.c linked_list.h
	gcc -c -g -ansi -pedantic -Wall linked_list.c -o linked_list.o
