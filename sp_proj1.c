#include "20151571.h"

//가능한 명령어를 출력하는 함수
void command_help(const char **help_list, int size){
    int i;
    for(i = 0; i < size; ++i)
        printf("%s\n", help_list[i]);
}

// dir를 출력하는 함수
void command_dir(){
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
void command_history(History head){
    History ptr;
    int i = 1;
    for(ptr = head; ptr != NULL; ptr = ptr -> next)
        printf("%-5d %s\n", i++, ptr->command);
}

//opcodelist를 출력하는 함수
void command_opcodelist(Hash hashTable){
    Hnode ptr;
    for ( int i = 0; i < 20; ++i){
        printf("%d : ", i);
        for ( ptr = hashTable.Table[i]; ptr != NULL; ptr = ptr -> next){
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
int print_memory(Shell_Memory Shmemory, int start, int end){
    int str_hex = start / 16 * 16,
        end_hex = end / 16 * 16; // 16단위로 끊어서 세기 위함
    int i, j;
    unsigned char *memory = Shmemory.memory;

    if(start <= end && start >= 0){ // boundary check
        end = end <= Shmemory.max_address ? end : Shmemory.max_address ; // end boundary check  
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
int command_dump(Shell_Memory Shmemory, char *buffer, int *arr){
    int len = 0, num = 0;
    int start_address = ( Shmemory.last_address + 1 ) % 
        ( Shmemory.max_address + 1 ) ,
        end_address = min ( start_address + 159, Shmemory.max_address ); 
    // start_address와 end_address 기본 dump인 경우를 위한  초기화 부분.
    char tmp[256];
    for ( int i = 0; i  < (int)strlen(buffer); ++i )
        if ( buffer[i] == ',' )
            num++;

    strncpy(tmp, buffer, sizeof(tmp)); 
    str_replace(tmp, "," , " , ");
    if ( ( len = get_values(tmp, arr) ) == -1){
        fprintf(stderr, "get value error!\n");
        return -1;
    }
    if ( len == 1){
        if( num != 0)
            return -1;
        start_address = arr[0], end_address = min ( start_address + 159, Shmemory.max_address );
    }

    else if ( len == 2){
        if ( num != 1)
            return -1;
        start_address = arr[0], end_address = arr[1];
    }
    else if ( len > 2)
        return -1;

    if ( start_address > Shmemory.max_address ||
            start_address > end_address){
        printf("Address Error!\n");
        return -1;
    }
    print_memory (Shmemory, start_address, end_address );
    Shmemory.last_address = end_address;
    return 1;
}

/* edit 명령어 처리해주는 함수
 * 정상적인 경우 return 1
 * error check에 걸린 경우 return -1
 */
int command_edit(Shell_Memory Shmemory, char *buffer, int *arr){
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
    if( get_values(tmp, arr) == -1){ // values에 에러 있는지 확인
        fprintf( stderr, "get values error!\n");
        return -1;
    }

    address = arr[0]; value = arr[1];

    if ( ! ( 0 <= value && value <= 0xFF ) ){
        fprintf(stderr, "value boundary error!\n( 0 <= value <= 0xFF )\n");
        return -1;
    }

    if( 0 <= address && address <= Shmemory.max_address){ // boundary check
        Shmemory.memory[address] = value;
        return 1;
    }
    return -1;
}

/* fill 명령어 처리해주는 함수 
 * 정상적인 경우 return 1
 * error check에 걸린 경우 return -1
 */
int command_fill(Shell_Memory Shmemory, char *buffer, int *arr){
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

    if ( get_values(tmp, arr ) == - 1){ // value에 에러 있는지 확인
        fprintf(stderr, "Get values error!\n");
        return -1;
    }

    start = arr[0], end = arr[1], value = arr[2];

    if ( ! (0 <= value && value <= 0xFF) ){
        fprintf ( stderr, "value boundary error!\nvalue must 0 <= value <= 0xFF");
        return -1;
    }
    if(start >= 0 && start <= end && end <= Shmemory.max_address){ // boundary check
        for(int i = start; i <= end; ++i)
            Shmemory.memory[i] = value;
        return 1;
    }
    return -1;
}

/* reset 명령어를 처리해주는 함수
 * 메모리를 모두 0x00으로 초기화해준다.
 */
void command_reset(Shell_Memory Shmemory){
    unsigned char *memory = Shmemory.memory;
    int i;
    for ( i = 0; i < Shmemory.max_address; ++i )
        memory[i] = 0;
}

/* quit 명령어를 처리해주는 함수 
 * 추가로 동적할당한 부분을 free 해주고 종료한다.
 */
void command_quit(Hash hashTable, History *hi_head){
    Hnode hptr, Hhead;
    for ( int i = 0; i < hashTable.size; ++i ){
        Hhead = hashTable.Table[i];
        for ( ; Hhead != NULL; ){
            hptr = Hhead;
            Hhead = Hhead->next;
            free(hptr);
        }
    }
    *hi_head = NULL;
    exit(0);
}

/* opcode 명령어를 처리해주는 함수
 * 
 */
int command_opcode(Hash hashTable, char *buffer ){
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

    if( ( n_opcode = Hash_find(hashTable, mnemonic) )== -1){
        fprintf(stderr, "There is no mnemonic\n");
        return -1;
    }

    printf("opcode is %X\n", n_opcode);
    return 1;
}
