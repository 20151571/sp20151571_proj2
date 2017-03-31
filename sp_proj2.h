#ifndef __sp_proj2__
    #define __sp_proj2__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>


// assembler directives(pseudo-instructions)
typedef enum{
    start, end, base, byte,
    word, resb, resw
}assem_directives;

typedef enum{
    A, X, L, B , S ,
    T , F , PC, SW
}Register;


typedef struct{
    int base_address;
    int locr;
    int line;
    int address;
}Pass1;

typedef struct{
    char symbol[30];
    int address;
    struct Symbol * next;
}Symbol;

typedef Symbol * symbolPtr;

typedef struct{
    const int size; // 37
    Symbol * table[37];
    Symbol * table_endp[37];
}Symbol_table;

void sp2_init(Symbol_table *); // 초기화 함수
int get_Stablekey(Symbol_table *, char *); // symbol_table에 넣을 string의 key를 구해주는 함수
void symbol_insert(Symbol_table *, char *, int); // symbol_table insert fucntin
int get_register(char *); // 어떤 register인지 구해주는함수
int get_asmd(char *); // 어떤 asmd인지 구해주는 함수.
int tokenize(char *, int );
int get_operhand(char *); // operhand를 구해주는 함수
int get_argnum(char *, char **); // 인자 개수를 확인해주는 함수
void print_file(char *); // file 내용을 출력하는 함수
int command_assemble(char *); // assemble 명령어를 수행하는 함수
int command_type(char *); // type 명령어를 수행하는 함수
void command_symbol(); // symbol 명령어를 수행하는 함수

#endif
