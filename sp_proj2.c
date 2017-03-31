#include "sp_proj2.h"

void sp2_init(Symbol_table *Stable){
//
//
//
//
}

int get_Stablekey(Symbol_table *Stable, char *string){
    int key = 0;

    for ( size_t i = 0; i < strlen(string); ++i )
        key += string[i];

    return key % Stable->size;
}

void symbol_insert(Symbol_table *Stable, char *string, int address){
    int key;
    symbolPtr ptr = NULL;
    ptr = ( symbolPtr ) malloc ( sizeof (Symbol) );
    strncpy ( ptr->symbol, string, sizeof(ptr->symbol));
    ptr->address = address;
    ptr->next = NULL;


    key = get_Stablekey(Stable, string);

    if(Stable->table[key] == NULL)
        Stable->table[key] = ptr;
}

int find_register(char *string){

    for ( size_t i=0; i < strlen(string); ++i )
        string[i] = ( string[i] >= 'a' && string[i] <= 'z') ?
            string[i] -'a' + 'A' : string[i];

    if ( strcmp(string,"A") == 0 )
        return A;
    else if ( strcmp(string,"X") == 0 )
        return X;
    else if ( strcmp(string,"L") == 0 )
        return L;
    else if ( strcmp(string,"B") == 0 )
        return B;
    else if ( strcmp(string,"S") == 0 )
        return S;
    else if ( strcmp(string,"T") == 0 )
        return T;
    else if ( strcmp(string,"F") == 0 )
        return F;
    else if ( strcmp(string,"PC") == 0 )
        return PC;
    else if ( strcmp(string,"SW") == 0 )
        return SW;

    return -1; // error
}

int get_asmd(char *string) {

    for ( size_t i=0; i < strlen(string); ++i )
        string[i] = ( string[i] >= 'a' && string[i] <= 'z') ?
            string[i] -'a' + 'A' : string[i];

    if ( strcmp(string,"START") == 0 )
        return start;
    else if(strcmp(string,"END") == 0)
        return end;
    else if(strcmp(string,"BASE") == 0)
        return base;
    else if(strcmp(string,"BYTE") == 0)
        return byte;
    else if(strcmp(string,"WORD") == 0)
        return word;
    else if(strcmp(string,"RESB") == 0)
        return resb;
    else if(strcmp(string,"RESW") == 0)
        return resw;

    return -1; // error
}


int tokenize(char *buffer, int len){

}

int get_argnum(char *buffer, char *argu[]){
    int type, len = 0;
    char sep[] = " \t";
    char *token = NULL;
    token = strtok(buffer, sep);
    if ( strcmp ( token, ".") == 1 ) // 주석인 경우
        return 0;
    while( token != NULL){ // 주석이 아닌 경우
        argu[len++] = token;
        token = strtok(NULL, sep);
    }

    //type = 
    return type;
}

void print_file(char *filename){
    FILE *fp;
    char c, bf;

    fp = fopen(filename, "r");
    if ( fp == NULL )
        fprintf(stderr, "FILE OPEN ERROR\n");

    while ( (c = fgetc (fp)) != EOF){
        bf = c;
        printf("%c", c);
    }

    if ( bf != '\n')
        puts("");
}

int command_assemble(char *command){
    FILE *fp;
    char buffer[256];
    char copy[256];
    char *argu[10];
    char *filename;
    int type;

    filename = strtok(command, " \t");
    filename = strtok(NULL, " \t");

    fp = fopen(filename, "r" );

    if ( fp == NULL ){
        fprintf(stderr, "FILE OPEN ERROR\n");
        return -1;
    }

    while ( fgets(buffer, sizeof(buffer) , fp) != NULL) {
        strncpy ( copy , buffer, sizeof(copy) );
        // type = get_argnum(copy, argu);
        if ( type > 0) // 주석인 경우
            break;
    }

    if ( type == 1){ // opcode 시작이 START인 경우
        while ( fgets( buffer, sizeof(buffer) , fp )  != NULL ){
            strncpy ( copy , buffer, sizeof(copy) );
            // type = get_argnum(copy, argu);
            // Loc = 
            //
        }
    }

    else{ // opcode 시작이 START가 아닌 경우 시작 Loc = 0
        // Loc = 0;
        //
    }

/*
    if( direc == start){
        while ( fgets( buffer, sizeof(buffer), fp ) != NULL ) {
            strncpy ( copy, buffer, sizeof(copy));
            len = get_argnum(copy, argu);
        }
    }
*/
    return 1;
}

/* 현재 디렉토리에 있는 filename 파일을 읽어서 화면에 출력한다.
 * 현재 디렉토리에 존재하지 않거나 디렉토리인 경우에는 에러 메세지를 출력한다.
 */
int command_type(char *buffer){
    DIR *dirp;
    struct dirent *direntp;
    struct stat buf;
    int flag = -1;
    char *filename;

    filename = strtok(buffer, " \t");
    filename = strtok(NULL, " \t");

    if( (dirp = opendir(".")) == NULL){ //diropen error check
        printf("Can not Open Directory\n");
        return 1;
    }

    while( (direntp = readdir(dirp)) != NULL){ // dir에 있는 모든 파일을 읽을 때 까지
        stat(direntp->d_name, &buf);

        if( S_ISDIR(buf.st_mode) ) //directory인 경우
            continue;
        else if( strcmp (direntp->d_name, filename ) == 0){
            flag = 1;
            print_file(filename);
        }
    }
    closedir(dirp);
    return flag == -1 ?
            printf("type command error!\n") : puts("") ;
}

/* symbol command 수행하는 함수이다.
 *  각 symbol의 주소값을 출력한다.
 */
void command_symbol(){

}
