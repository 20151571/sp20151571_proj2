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
    "opcodelist"
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
    "opcodelist"
};

int values[5]; // 입력한 명령어에서 value들을 저장하는 배열(address, start, end 등등)
char str_copy[256]; // string copy용 배열
Hash hash_table; // hash_table 구조체
History Hhead = NULL; // history를 linked_list로 구현한 head
Shell_Memory Sh_memory; // 메모리 정보를 담고 있는 구조체
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
int get_values(char *buffer){
    char *token;
    char *error;
    char sep[] = " \t";
    int idx= 0, flag = 1; // flag는 에러 체크할 변수
    int value;
    token = strtok(buffer, sep);
    while(token != NULL){
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
            values[idx++] = value;
            flag = 0;
        }
    }
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
void get_opcode(){
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
        Hash_insert(n_opcode, str_opcode); //hash에 insert
    }
    fclose(fp);
}


// history에 입력한 명령어를 linked_list tail에  추가하는 함수
void add_history(char *command){
    History ptr = Hhead;
    History nptr;

    nptr = malloc(sizeof(Linked_list));
    nptr -> next = NULL;
    strncpy(nptr->command, command, sizeof(nptr->command));

    if(ptr != NULL){
        for(; ptr-> next != NULL; ptr = ptr -> next); // 가장 최근에 입력한 명령어를 가장 나중에 넣는다.
        ptr -> next = nptr;
    }
    else
        Hhead = nptr;
}

// 초기화 함수
void init(){
    int i;
    hash_table.size = 20;
    for ( i = 0; i < hash_table.size; ++i )
        hash_table.Table[i] = NULL;

    Sh_memory.last_address = 1048575;
    Sh_memory.max_address = 1048575;
    for( i = 0; i < Sh_memory.max_address; ++i)
        Sh_memory.memory[i] = 0;
    get_opcode(); //opcode 저장
}

/* mnemonic string에 해당하는 opcode가 존재하는지 hash에서 찾는 함수
 * 존재하면 number opcode를 return
 * 존재하지 않으면 return -1
 */
int Hash_find(char *mnemonic){
    int opcode = -1;
    int i;
    Hnode ptr;
    for ( i = 0; i < hash_table.size; ++i ){ // hash_table에서 일일이 찾는다
        for ( ptr = hash_table.Table[i]; ptr != NULL; ptr = ptr -> next ){
            if ( strcmp( ptr->str_opcode , mnemonic ) == 0)
                return  ( opcode = ptr -> n_opcode );
        }
    }
    return -1; // 존재하지 않는 경우
}

/* Hash에 mnemonic을 insert하는 함수
 * 적절한 key를 사용하여 insert한다(여기서는 key % hash_size를 이용한다)
 */
void Hash_insert(int n_opcode, char *mnemonic){
    Hnode ptr;
    Hnode nptr;
    int Hash_size = hash_table.size;
    int key = 0;
    nptr = malloc(sizeof(Hash_Node));
    strncpy ( nptr -> str_opcode, mnemonic, sizeof(nptr -> str_opcode) );

    for( int i = 0; i < (int)strlen(mnemonic); ++i )
        key += mnemonic[i];
    nptr -> n_opcode = n_opcode;
    nptr -> next = NULL;

    key = key % Hash_size;
    ptr = hash_table.Table[key];

    if(ptr != NULL)
        nptr -> next = ptr;

    hash_table.Table[key] = nptr;
}

//가능한 명령어를 출력하는 함수
void print_help(){
    int i;
    int size = sizeof(help_list) / sizeof(char *);
    for(i = 0; i < size; ++i)
        printf("%s\n", help_list[i]);
}

// dir를 출력하는 함수
void print_dir(){
    DIR *dirp;
    struct dirent *direntp;
    struct stat buf;
    if( (dirp = opendir(".")) == NULL){ //diropen error check
        printf("Can not Open Directory\n");
        return;
    }

    while( (direntp = readdir(dirp)) != NULL){ // dir에 있는 모든 파일을 읽을 때 까지
        stat(direntp->d_name, &buf);
        printf("%s", direntp->d_name);

        if( S_ISDIR(buf.st_mode) ) //directory인 경우
            printf("/");

        else if( (buf.st_mode & S_IXUSR) ||
                (buf.st_mode & S_IXGRP) ||
                (buf.st_mode & S_IXOTH) ) // 실행 파일인 경우
            printf("*");
        printf("\t");
    }
    puts("");
    closedir(dirp);
}

//history 목록을 출력하는 함수
void print_history(){
    History ptr;
    int i = 1;
    for(ptr = Hhead; ptr != NULL; ptr = ptr -> next)
        printf("%-5d %s\n", i++, ptr->command);
}

//opcodelist를 출력하는 함수
void print_opcodelist(){
    Hnode ptr;
    for ( int i = 0; i < 20; ++i){
        printf("%d : ", i);
        for ( ptr = hash_table.Table[i]; ptr != NULL; ptr = ptr -> next){
            printf("[%s,%X]", ptr->str_opcode, ptr->n_opcode);
            if(ptr -> next != NULL)
                printf(" -> ");
        }
        puts("");
    }
}

/* start부터 end까지 memory 정보를 출력하는 함수
 * boundary check 에서 boundary error가 발견되면 return -1
 * 정상적인 경우 return 1
 */
int print_memory(int start, int end){
    int str_hex = start / 16 * 16,
        end_hex = end / 16 * 16; // 16단위로 끊어서 세기 위함
    int i, j;
    unsigned char *memory = Sh_memory.memory;

    if(start <= end && start >= 0){ // boundary check
        end = end <= Sh_memory.max_address ? end : Sh_memory.max_address ; // end boundary check
        for ( i = str_hex; i <= end_hex; i += 16){
            printf("%05X ", i);
            for(j = 0; j < 16; ++j){
                if(start <= i + j && i + j <= end)
                    printf("%02X ", memory[i+j]);
                else
                    printf("   ");
            }
            printf("; ");

            for(j = 0; j < 16; ++j){
                if(i + j >= start && i + j <= end){
                    if(memory[i+j] >= 0x20 && memory[i+j] <= 0x7E){
                        printf("%c", memory[i+j]);
                    }
                    else
                        printf(".");
                }
                else
                    printf(".");
            }
            puts("");
        }
        return 1; // 정상인 경우
    }
    else // boundary check에서 에러확인된경우
        return -1;
}

/* dump 명령어를 처리해주는 함수
 * dump만 입력한 경우 last_address 다음 주소에서 160칸 출력(메모리 범위를 넘어가지 않는 범위에서)
 * dump start만 입력한 경우 start에서 160칸 출력(단 메모리 범위를 넘어가지 않는 범위에서)
 * dump start. end만 입력한 경우 start부터 end까지 출력해준다.
 * start. end가 16진수가 아니거나 범위를 넘어가는 경우는 출력하지 않고 에러처리한다.
 */
int command_dump(char *buffer){
    int len = 0, num = 0;
    int start_address = ( Sh_memory.last_address + 1 ) % 
        ( Sh_memory.max_address + 1 ) ,
        end_address = min ( start_address + 159, Sh_memory.max_address ); 
    // start_address와 end_address 기본 dump인 경우를 위한  초기화 부분.
    char tmp[256];
    for ( int i = 0; i  < (int)strlen(buffer); ++i )
        if ( buffer[i] == ',' )
            num++;

    strncpy(tmp, buffer, sizeof(tmp)); 
    str_replace(tmp, "," , " , ");
    if ( ( len = get_values(tmp) ) == -1){
        fprintf(stderr, "get value error!\n");
        return -1;
    }
    if ( len == 1){
        if( num != 0)
            return -1;
        start_address = values[0], end_address = min ( start_address + 159, Sh_memory.max_address );
    }

    else if ( len == 2){
        if ( num != 1)
            return -1;
        start_address = values[0], end_address = values[1];
    }
    else if ( len > 2)
        return -1;

    if ( start_address > Sh_memory.max_address ||
            start_address > end_address){
        printf("Address Error!\n");
        return -1;
    }
    print_memory ( start_address, end_address );
    Sh_memory.last_address = end_address;
    return 1;
}

/* edit 명령어 처리해주는 함수
 * 정상적인 경우 return 1
 * error check에 걸린 경우 return -1
 */
int command_edit(char *buffer){
    int address, value;
    int num = 0;
    char tmp[256];
    strncpy(tmp, buffer, sizeof(tmp));

    for ( int i = 0; i < (int)strlen(tmp); ++i)
        if ( tmp[i] == ',')
            num++; // ','의 개수를 샌다.
    if(num != 1){ // 1개가 아닌 경우 에러
        fprintf( stderr, "Format error!\nFormat is e[dit] address, value\n");
        return -1;
    }

    str_replace(tmp, ",",  " , "); // ','를 ' , '로 다 바꿔준다.
    if( get_values(tmp) == -1){ // values에 에러 있는지 확인
        fprintf( stderr, "get values error!\n");
        return -1;
    }

    address = values[0]; value = values[1];

    if ( ! ( 0 <= value && value <= 0xFF ) ){
        fprintf(stderr, "value boundary error!\n( 0 <= value <= 0xFF )\n");
        return -1;
    }

    if( 0 <= address && address <= Sh_memory.max_address){ // boundary check
        Sh_memory.memory[address] = value;
        return 1;
    }
    return -1;
}

/* fill 명령어 처리해주는 함수 
 * 정상적인 경우 return 1
 * error check에 걸린 경우 return -1
 */
int command_fill(char *buffer){
    int num = 0;
    int start, end, value;
    char tmp[256];

    strncpy(tmp, buffer, sizeof(tmp)); // tmp에 buffer string copy

    for( int i = 0; i < (int)strlen(buffer); ++i)
        if( buffer[i] == ',') // ',' 개수 확인
            num++;
    if(num != 2){ // ','가 2개가 아니면 에러
        fprintf(stderr, "Format Error!\nFormat is f[ill] start, end, value\n");
        return -1;
    }

    str_replace(tmp, ",", " , "); // ',' 를 ' , '로 치환해준다.

    if ( get_values(tmp) == - 1){ // value에 에러 있는지 확인
        fprintf(stderr, "Get values error!\n");
        return -1;
    }

    start = values[0], end = values[1], value = values[2];

    if ( ! (0 <= value && value <= 0xFF) ){
        fprintf ( stderr, "value boundary error!\nvalue must 0 <= value <= 0xFF");
        return -1;
    }
    if(start >= 0 && start <= end && end <= Sh_memory.max_address){ // boundary check
        for(int i = start; i <= end; ++i)
            Sh_memory.memory[i] = value;
        return 1;
    }
    return -1;
}

/* reset 명령어를 처리해주는 함수
 * 메모리를 모두 0x00으로 초기화해준다.
 */
void command_reset(){
    unsigned char *memory = Sh_memory.memory;
    int i;
    for ( i = 0; i < Sh_memory.max_address; ++i )
        memory[i] = 0;
}

/* quit 명령어를 처리해주는 함수 
 * 추가로 동적할당한 부분을 free 해주고 종료한다.
 */
void command_quit(){
    History hptr;

    for ( int i = 0; i < hash_table.size; ++i ){
        for ( ; Hhead != NULL; ){
            hptr = Hhead;
            Hhead = Hhead->next;
            free(hptr);
        }
    }
    exit(0);
}

/* opcode 명령어를 처리해주는 함수
 * 
 */
int command_opcode(char *buffer ){
    char tmp[256];
    int len = 0, n_opcode;
    char sep[] = " \t";
    char *token = NULL;
    char *mnemonic;
    strncpy(tmp, buffer, sizeof(tmp));

    token = strtok(tmp, sep);
    while( token != NULL){
        token = strtok(NULL, sep);
        if(token == NULL)
            break;
        mnemonic = token;
        len++;
    }

    if ( len != 1 ){
        fprintf(stderr, "Format error!\nFormat is [opcode mnemonic]\n");
        return -1;
    }

    if( ( n_opcode = Hash_find(mnemonic) )== -1){
        fprintf(stderr, "There is no mnemonic\n");
        return -1;
    }

    printf("opcode is %X\n", n_opcode);
    return 1;
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
            || command_num == 16 )
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
void main_process(char *buffer){
    int command_num;
    int error_check;
    strncpy( str_copy, buffer, sizeof(str_copy));
    command_num = command_check(str_copy); // 명령어 존재하는지 확인

    if(command_num != -1){
        strncpy(str_copy, buffer, sizeof(str_copy));
        error_check = 1;
        switch(command_num){
            case help:
                print_help();
                break;

            case dir:
                print_dir();
                break;

            case quit:
                command_quit();
                break;

            case history:
                add_history(buffer);
                error_check = 0;
                print_history();
                break;

            case dump:
                error_check = command_dump(str_copy);
                break;

            case edit:
                error_check = command_edit(str_copy);
                break;

            case Fill:
                error_check = command_fill(str_copy);
                break;

            case Reset:
                command_reset();
                break;

            case opcode:
                error_check = command_opcode(str_copy);
                break;

            case opcodelist:
                print_opcodelist();
                break;
        }
        if ( error_check == 1 )
            add_history(buffer);
    }

    else
        printf("There is no command\nplease rewrite command\n");
}
