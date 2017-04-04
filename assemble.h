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

//register
typedef enum{
    A, X, L, B , S ,
    T , F , PC, SW
}Register;

typedef enum{
    comment, opcode, pass1_start, asmd, label, pass1_end
}pass1_info;


int symbol_cmp(void *, void *); // symbol compare function
void print_symbol(symbol_info *, int ); // symbol print function
void delete_whitespace(char *); // whitespace 제거해주는 함수
int get_Stablekey(Symbol_table *, char *); // symbol_table에 넣을 string의 key를 구해주는 함수
symbolPtr symbol_find(Symbol_table *, char *string); // symbol find funcition
void symbol_insert(Symbol_table *, char *, int); // symbol_table insert fucntin
void symbol_init(Symbol_table *); // symbol init
Hnode opcode_find(Hash *, char *mnemonic); // opcode 찾는 함
int get_register(char *); // 어떤 register인지 구해주는 함수
int get_type(char *, Hash *); // string이 label인지 address인지 등등 구해주는 함수
int get_asmd(char *); // 어떤 asmd인지 구해주는 함수.
int get_byte(char **, line_inform *line_info); // byte인 경우 값을 구해주는 함
int loc_count(char *, int asmd, int location ); // location counter
int get_objcode(int opcode, int n, int i, int x,
       int b, int p, int e, int address); // object code 생성함수
int get_argu(char *, char **, Hash *); // 인자 개수를 확인해주는 함수
int obj_byte(FILE *, Symbol_table*, line_inform *line_info,
        object_inform *,int *, int *); // byte 인경우 object code 생성
int obj_opcode(FILE *, Hash *, Symbol_table *, line_inform *,
        object_inform *,int *, int *, int *, int base_address); // opcode인 경우 object code 생성하는 함수
int remove_char(char *, Symbol_table *, int *, int *); // # 이나 @ 없애주는 함수
int asmd_process(char **, line_inform *line_info, int location); // assemble directives 처리해주는 함수
int opcode_location(Pass1 *, Hash *, char **, line_inform *line_info); // locationo 계산해주는 함수
int make_line(char *string, int type, size_t idx, int *flag,
        Symbol_table *, Pass1 *Pinfo, line_inform *line_info); // pass1에서 각 라인 마다 처리해주는 함수
void make_objfile(FILE *fp, int, int ,line_inform *,  object_inform *); // obj file 만드는 함수
int assembler_pass2(char *filename, Symbol_table *, int length,
        Pass1 *Pinfo, line_inform *line_info); // pass2 algorithm 함수
void print_file(char *); // file 내용을 출력하는 함수

#endif
