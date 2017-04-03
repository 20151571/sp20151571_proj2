#include "20151571.h"

const char *help_list[] = {
    "h[elp]",
    "d[ir]",
    "q[uit]",
    "hi[story]",
    "du[mp] [start, end]",
    "e[dit] address, value",
    "f[ill] start, end, value",
    "reset",
    "opcode mnemonic",
    "opcodelist",
    "assemble fileneme",
    "type filename",
    "symbol"
};

const char *Help[] = {
    "h",
    "help",
    "d",
    "dir",
    "q",
    "quit",
    "hi",
    "history",
    "du",
    "dump",
    "e",
    "edit",
    "f",
    "fill",
    "reset",
    "opcode",
    "opcodelist",
    "assemble",
    "type",
    "symbol"
};

// a ,b  값중 작은 값을 반환해준다.
int min(int a, int b){
    return a < b ? a : b;
}

/* target string에서 orig과 같은 부분을 repl으로 replace 시켜주는 함수이다.
 * char *target이바꿀 string이고 orig은replace될 string, repl은 orig을 replace할 string이다.
 */
void str_replace(char *target, const char *orig, const char *repl){
    char buffer[256] = { 0 }; // target string을 copy할 배열
    char *insert_point = buffer; // replace할 point를 저장할 pointer
    const char *tmp = target; // target의원래 주소를 저장한 pointer
    size_t orig_len = strlen(orig);
    size_t repl_len = strlen(repl);
    while (1) {
        const char *p = strstr(tmp, orig); // p = orig을 포함한 문자열의 위치
        if (p == NULL) { // 더이상 orig을 포함한 문자열이 존재하지 않음
            strcpy(insert_point, tmp); // insert_point에 tmp string copy
            break;
        }
        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;
        memcpy(insert_point, repl, repl_len);
        insert_point += repl_len;
        tmp = p + orig_len; // insert_point를 찾고 buffer에 저장하는 부분
    }
    strcpy(target, buffer); //targer에 replacement한string을복사함
}

/* 입력한 명령어에서 값들을 저장하는 함수
 * 값들이 정상적인 경우 return 1
 * 이상한 입력이 들어온 경우 return -1
 */
int get_values(char *buffer, int *arr){
    char *token, *bef;
    char *error;
    char sep[] = " \t";
    int idx= 0, flag = 1; // flag는 에러 체크할 변수
    int value;
    token = strtok(buffer, sep);
    while(token != NULL){
        bef = token;
        token = strtok(NULL, sep);
        if ( token == NULL)
            break;
        if ( strcmp( token , "," ) == 0 ){ // ',' 에러 처리
            if ( flag )
                return -1;
            else
                flag = 1;
        }
        else{
            if(flag != 1)
                return -1;
            value = (int)strtol(token, &error, 16); // 16진수의 값을 10진수로 바꿈
            if( *error != '\0'){ // 16진수가 아닌 경우 에러 처리
                fprintf(stderr, "No Hex number!\n");
                return -1;
            }
            arr[idx++] = value;
            flag = 0;
        }
    }
    if(*bef == ',')
        return -1;
    return idx;
}

/* 입력한 명령어가존재하는지 찾고 몇번째와 일치하는지 알려주는 함수
 * 존재하는 경우 return 번호
 * 존재하지 않는 경우 return -1
 */
int command_find(char *str_cmp){
    int i;
    int size = sizeof(Help) / sizeof(char *);
    for ( i = 0; i < size; ++i)
        if ( strcmp(Help[i], str_cmp) == 0)
            return i;
    printf("The corresponding command does not exist.\n");
    return -1;
}

/* 입력한 명령어가 존재하면 enum command에 있는 값으로 적절히 값을return하는 함수
 * 명령어가 없는 경우만 return -1을 한다.
 */
int get_command(char *buffer){
    char sep[] = " \t";
    char *token = strtok(buffer, sep);
    int command_num;
    while(token != NULL){
        if ( ( command_num = command_find(token) ) != -1 )
            return ( command_num <= 11 ? command_num / 2 : command_num - 5);
        // dir와 같은 명령어들은 d[ir]이런 형식으로 사용할 수 있어서 이 부분을 처리해주는 부분이다.
        token = strtok(NULL, sep);
    }
    return -1;
}

// opcode.txt에서 opcode를입력받아서 저장하는 함수
void get_opcode(Hash *hashTable){
    FILE *fp;
    char buffer[256];
    int n_opcode;
    char str_opcode[100];
    char code[100];

    fp = fopen("opcode.txt", "r");
    if ( fp == NULL){
        printf("FILE OPEN ERROR\n");
        return;
    }

    while ( fgets(buffer, sizeof(buffer), fp) != NULL ){
        sscanf(buffer, "%x %s %s", &n_opcode, str_opcode, code ); //space나 tab 같은 것들 처리
        Hash_insert(hashTable, n_opcode, str_opcode, code); //hash에 insert
    }
    fclose(fp);
}


// history에 입력한 명령어를 linked_list tail에  추가하는 함수
void add_history(History *head, char *command){
    History ptr = *head;
    History nptr;

    nptr = malloc(sizeof(Linked_list));
    nptr -> next = NULL;
    strncpy(nptr->command, command, sizeof(nptr->command));

    if(ptr != NULL){
        for(; ptr-> next != NULL; ptr = ptr -> next); // 가장 최근에 입력한 명령어를 가장 나중에 넣는다.
        ptr -> next = nptr;
    }
    else
        *head = nptr;
}

// 초기화 함수
void sp1_init(History *Hhead, Shell_Memory *Shmemory, Hash *hashTable){
    int i;
    *Hhead = NULL;
    hashTable->size = 20;
    for ( i = 0; i < hashTable->size; ++i )
        hashTable->Table[i] = NULL;

    Shmemory->last_address = 1048575;
    Shmemory->max_address = 1048575;
    for( i = 0; i < Shmemory->max_address; ++i)
        Shmemory->memory[i] = 0;
    get_opcode(hashTable); //opcode 저장
}

//초기화 함수
void sp2_init(Symbol_table *Stable){
    for ( int i = 0; i < 37; ++i)
        Stable->table[i] = NULL;
}

/* mnemonic string에 해당하는 opcode가 존재하는지 hash에서 찾는 함수
 * 존재하면 number opcode를 return
 * 존재하지 않으면 return -1
 */
int Hash_find(Hash *hashTable, char *mnemonic){
    int opcode = -1;
    int i;
    Hnode ptr;
    for ( i = 0; i < hashTable->size; ++i ){ // hash_table에서 일일이 찾는다
        for ( ptr = hashTable->Table[i]; ptr != NULL; ptr = ptr -> next ){
            if ( strcmp( ptr->str_opcode , mnemonic ) == 0)
                return  ( opcode = ptr -> n_opcode );
        }
    }
    return -1; // 존재하지 않는 경우
}

/* Hash에 mnemonic을 insert하는 함수
 * 적절한 key를 사용하여 insert한다(여기서는 key % hash_size를 이용한다)
 */
void Hash_insert(Hash *hashTable, int n_opcode, char *mnemonic, char *code){
    Hnode ptr;
    Hnode nptr;
    int Hash_size = hashTable->size;
    int key = 0;
    nptr = malloc(sizeof(Hash_Node));
    strncpy ( nptr->str_opcode, mnemonic, sizeof(nptr->str_opcode) );
    strncpy( nptr->code , code, sizeof(nptr->code) );

    for( int i = 0; i < (int)strlen(mnemonic); ++i )
        key += mnemonic[i];
    nptr -> n_opcode = n_opcode;
    nptr -> next = NULL;

    key = key % Hash_size;
    ptr = hashTable->Table[key];

    if(ptr != NULL)
        nptr -> next = ptr;

    hashTable->Table[key] = nptr;
}

/* 입력한 명령어가 존재하는지 확인하는 함수
 * 입력한 명령어가 존재하는 경우 enum Command에 해당하는 값에 맞게 출력해준다.
 * 에러인 경우만 return -1
 */
int command_check(char *user_str){
    char sep[] = " \t";
    char *token;
    int i = 0, command_num = -1, len = 0;
    token = strtok(user_str, sep);

    if(token == NULL) // 입력한 것이 없는 경우
        return -1;

    command_num = command_find(token); // 명령어가 존재하는지 확인
    if(command_num == -1) // 없는 경우
        return -1;

    while ( token != NULL ){
        if(i > 6) // 이상한 명령어 입력해준경우 체크
            return -1;
        len++;
        token = strtok(NULL, sep);
    }

    if ( ( 0 <= command_num && command_num <= 7 ) || command_num ==  14
            || command_num == 16 || command_num == 19)
        if ( len > 1 )
            return -1; // dir나 help 같은 추가로 입력할 것이 없는 명령어 에러 확인

    if( 0 <= command_num && command_num <= 13)
        return command_num / 2; // du[mp] 같이 입력할 수 있는 경우
    else if ( command_num > 13)
        return command_num - 7; // reset 같이 하나인 경우.
    return -1; // error인 경우
}

/* 사용자가 입력한 명령어를 처리해주는 함수
 * 명령어에 따라 적절하게 처리해준다.
 */
void main_process(char *buffer, History *head, Shell_Memory *Shmemory,\
        Hash *hashTable, Symbol_table *Stable, symbol_info symbolArr[]){
    int command_num;
    int error_check;
    char str_copy[256];
    int arr[5];
    History Hhead = *head;
    Stable->hashTable = hashTable;
    strncpy( str_copy, buffer, sizeof(str_copy));
    command_num = command_check(str_copy); // 명령어 존재하는지 확인
    if(command_num != -1){
        strncpy(str_copy, buffer, sizeof(str_copy));
        error_check = 1;
        switch(command_num){
            case help:
                command_help(help_list, sizeof(help_list) / sizeof(char * ) );
                break;

            case dir:
                command_dir();
                break;

            case quit:
                command_quit(hashTable, &Hhead);
                break;

            case history:
                add_history(&Hhead, buffer);
                error_check = 0;
                command_history(Hhead);
                break;

            case dump:
                error_check = command_dump(Shmemory, str_copy, arr);
                break;

            case edit:
                error_check = command_edit(Shmemory, str_copy, arr);
                break;

            case Fill:
                error_check = command_fill(Shmemory, str_copy, arr);
                break;

            case Reset:
                command_reset(Shmemory);
                break;

            case opcode:
                error_check = command_opcode(hashTable, str_copy);
                break;

            case opcodelist:
                command_opcodelist(hashTable);
                break;

            case assemble:
                error_check = command_assemble( Stable, symbolArr, str_copy );
                break;

            case type:
                error_check = command_type(str_copy);
                break;

            case symbol:
                command_symbol(symbolArr);
                break;
        }
        if ( error_check == 1 )
            add_history(&Hhead, buffer);
    }

    else
        printf("There is no command\nplease rewrite command\n");
    *head = Hhead;
}
