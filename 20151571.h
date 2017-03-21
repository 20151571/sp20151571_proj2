#ifndef __20151571__
    #define __20151571__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

enum COMMAND{
    help, dir, quit, history, dump, edit,
    Fill, Reset, opcode, opcodelist,
    Error
};
typedef struct LINKED_LIST * History;
typedef struct LINKED_LIST * Lnode;
typedef struct HASH_Linked_List * Hnode;

typedef struct LINKED_LIST{
    char command[50];
    Lnode next;
}Linked_list;

#ifndef HASH_SIZE
#define HASH_SIZE 20

typedef struct _HASH_{
    int size;
    Hnode Table[HASH_SIZE];
}Hash;
#endif

typedef struct HASH_Linked_List{
    int n_opcode;
    char str_opcode[50];
    Hnode next;
}Hash_Node;

typedef struct MEMORY{
    unsigned char memory[1048576];
    int last_address;
    int max_address;
}Shell_Memory;

int min(int a, int b); //a,b 값중작은 값을 반환하는 함수
void str_replace(char *, const char *, const char *); //문자열 치환 함수
int get_values(char *); // 명령어에서 값들을 가져오는 함수
int command_find(char *str_cmp); // 입력한 명령어가 있는지 찾고몇번째 명령어 인지  찾는 함수
int get_command(char *buffer); // 입력한 명령어를 사용하기 위해 적절히 숫자를 지정해주는 함수
void add_history(char *command); // history에 추가하는 함수

void init(); // 초기화

int Hash_find(char *); // hash에서 찾는 함수

void Hash_insert(int value, char *mnemonic); // hash에 insert하는 함수

void print_help(); // helplist를 출력하는 함수
void print_dir(); // dir를 출력하는 함수
void print_history(); // history를 출력하는 함수
void print_opcodelist(); // opcodelist를 출력하는 함수
int print_memory(int start, int end); // memory를 start부터 end까지 출력하는 함수

int command_dump(char *bufffer); // 입력한 명령어가 dump인 경우 처리해주는 함수
int command_edit(char *buffer); // 입력한 명령어가 edit인 경우 처리해주는 함수
int command_fill(char *buffer); // fill인 경우 처리해주는 함수
void command_reset(); // reset인 경우 처리해주는 함수
int command_opcode(char *mnemonic); // opcode인 경우 처리해주는 함수
int command_check(char *buffer); // 명령어를 옳바르게 입력한지 확인해주는 함수력
void command_quit(); // 입력한 명령어가 quit 일 때 process를 종료시켜주는 함수
void main_process(char *buffer); // 명령어 string을 받아처리해주는 함수

#endif
