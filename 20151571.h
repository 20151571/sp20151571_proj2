#ifndef __20151571__
#define __20151571__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "structure.h"

enum COMMAND{
    help, dir, quit, history, dump, edit,
    Fill, Reset, opcode, opcodelist, assemble,
    type, symbol, Error
};

//project #1
int min(int a, int b); //a,b 값중작은 값을 반환하는 함수
void str_replace(char *, const char *, const char *); //문자열 치환 함수
int get_values(char *, int *); // 명령어에서 값들을 가져오는 함수
int command_find(char *str_cmp); // 입력한 명령어가 있는지 찾고몇번째 명령어 인지  찾는 함수
int get_command(char *buffer); // 입력한 명령어를 사용하기 위해 적절히 숫자를 지정해주는 함수
void add_history(History *head, char *command); // history에 추가하는 함수

int Hash_find(Hash *hashTable, char *); // hash에서 찾는 함수

void get_opcode(Hash *hashTable); // opcode find function
void Hash_insert(Hash *hashTable, int value, char *mnemonic, char *code); // hash에 insert하는 함수

void sp1_init(History *, Shell_Memory *, Hash *); // 초기화
void sp2_init(Symbol_table *); // initialize function
void command_reset(Shell_Memory *); // reset인 경우 처리해주는 함수

void command_help(const char **help_list, int size); // helplist를 출력하는 함수
void command_dir(); // dir를 출력하는 함수
void command_history(History head); // history를 출력하는 함수
void command_opcodelist(Hash *hashTable); // opcodelist를 출력하는 함수
int print_memory(Shell_Memory *, int start, int end); // memory를 start부터 end까지 출력하는 함수
int command_dump(Shell_Memory *, char *bufffer, int *arr); // 입력한 명령어가 dump인 경우 처리해주는 함수
int command_edit(Shell_Memory *, char *buffer, int *arr); // 입력한 명령어가 edit인 경우 처리해주는 함수
int command_fill(Shell_Memory *, char *buffer, int *arr); // fill인 경우 처리해주는 함수
void command_reset(Shell_Memory *); // reset인 경우 처리해주는 함수
int command_opcode(Hash *, char *mnemonic); // opcode인 경우 처리해주는 함수
int command_check(char *buffer); // 명령어를 옳바르게 입력한지 확인해주는 함수력
void command_quit(Hash *hashTable, History *head); // 입력한 명령어가 quit 일 때 process를 종료시켜주는 함수

int command_assemble(Symbol_table *, symbol_info *success_symbol, char *); // assemble 명령어를 수행하는 함수
int command_type(char *); // type 명령어를 수행하는 함수
void command_symbol(symbol_info * ); // symbol 명령어를 수행하는 함수

void main_process(char *buffer, History *, Shell_Memory *, 
        Hash *, Symbol_table *Stable, symbol_info *symbolARr); // 명령어 string을 받아처리해주는 함수
#endif
