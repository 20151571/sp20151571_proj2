#ifndef __sp_proj2__
    #define __sp_proj2__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "structure.h"

// assembler directives(pseudo-instructions)
typedef enum{
    start, end, base, byte,
    word, resb, resw
}assem_directives;

typedef enum{
    A, X, L, B , S ,
    T , F , PC, SW
}Register;

typedef enum{
    comment, opcode, pass1_start, asmd, label, pass1_end, label_end
}pass1_info;

void sp2_init(Symbol_table *); // 초기화 함수
int get_Stablekey(Symbol_table *, char *); // symbol_table에 넣을 string의 key를 구해주는 함수
void symbol_insert(Symbol_table *, char *, int); // symbol_table insert fucntin
void symbol_init(Symbol_table *);
int hash_find(Hash, char *mnemonic); // opcode 찾는 함
int get_register(char *); // 어떤 register인지 구해주는 함수
int get_type(char *, Hash); // string이 label인지 address인지 등등 구해주는 함수
int get_asmd(char *); // 어떤 asmd인지 구해주는 함수.
int get_byte(char **); // byte인 경우 값을 구해주는 함
int loc_count(char *, int asmd ); // location counter
int tokenize(char *, int );
int get_operhand(char *); // operhand를 구해주는 함수
int get_argu(char *, char **, Hash); // 인자 개수를 확인해주는 함수
int make_line(char *string, int type, size_t idx,
        int *flag, Pass1 *Pinfo, line_inform *line_info);
void print_file(char *); // file 내용을 출력하는 함수

#endif
