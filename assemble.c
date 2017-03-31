#include "assemble.h"

void sp2_init(Symbol_table *Stable){
    for ( int i = 0; i < 37; ++i)
        Stable->table[i] = NULL;
}

void symbol_init(Symbol_table *Stable){
    symbolPtr ptr;
    symbolPtr nptr;
    for ( int i = 0; i < 37; ++i){
        for ( ptr = Stable->table[i];  ptr != NULL;){
            nptr = ptr->next;
            free ( ptr );
            ptr = nptr;
        }
        Stable->table[i] = NULL;
    }
}

int hash_find(Hash hashTable, char *mnemonic){
    int opcode = -1;
    int i;
    Hnode ptr;
    for ( i = 0; i < hashTable.size; ++i ){ // hash_table에서 일일이 찾는다
        for ( ptr = hashTable.Table[i]; ptr != NULL; ptr = ptr -> next ){
            if ( strcmp( ptr->str_opcode , mnemonic ) == 0)
                return  ( opcode = ptr -> n_opcode );
        }
    }
    return -1; // 존재하지 않는 경우
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
    ptr->next = Stable->table[key];

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

int loc_count(char *string){
    int asmd;
    int len = 0;
    char *token = NULL;
    int location;
    if ( asmd == byte ){
        len = strstr(token + 1, "'") - token;
        if ( strstr ( string, "C") != NULL || strstr ( string, "c") != NULL )
            location +=  len;
        else if (  strstr ( string, "X") != NULL || strstr ( string, "x") != NULL )
            location += len / 2;
        else
            return -1;
    }

    else if ( asmd ==  word )
        location += 3;

    else if( asmd == resb )
        location += (int) strtol(string, NULL, 10);

    else if (asmd == resw )
        location += (int) strtol(string, NULL, 10) * 3;
    return 1;
}

int get_type(char *string, Hash hashTable){
    if ( strcmp (string , "." ) == 0 )
        return 0;
    else if( ( string[0] == '+' && hash_find ( hashTable, string + 1) != -1 )
           ||  hash_find ( hashTable, string ) != -1  )
        return 1; // opcode
    else if ( get_asmd(string) != -1 )
        return 2; // assembly directives
    return 3; // label
}

int get_argu(char *buffer, char *argu[], Hash hashTable){
    int len = 0, num;
    char sep[] = " \t\r\n";
    char *token = NULL;
    int type[5];
    token = strtok(buffer, sep);

    while( token != NULL){
        argu[len] = token;
        type[len++]  = get_type(token, hashTable);
        token = strtok(NULL, sep);
    }

    if ( type[0] == 0) // comment
        return 0;

    if( type[0] == 2){ // assembly directives
        if ( get_asmd(argu[0]) == end )
            return 3;
        return 1;
    }

    else if ( type[0] == 3){ // label
        num = get_asmd(argu[1]);
        if( num == start )
            return 2; // label이 있는 start
        else if ( num == end)
            return 3; // label이 있는 end
        return 4;
    }
    return -1;
}

int make_line(char *string, int type, size_t idx,
        int *flag, Pass1 *Pinfo, line_inform *line_info){
    char *error; 
    if ( type == 0 ){ // comment 인 경우
        line_info[idx].format = 0;
        strncpy( line_info[idx].comment, string,
                sizeof(line_info[idx].comment));
        return 0;
    }

    else if ( type == 1){ // assembly directives
        strcpy(line_info[idx].asmd, Pinfo->argu[0]);
        if ( get_asmd ( Pinfo->argu[0]) == byte ){
            
        }
    }

    else if ( type == 2 ){ // start인 경우
        if ( !(*flag) ) // start가 처음이 아닌 경우
            return -1;
        Pinfo->locr = strtol(Pinfo->argu[2], &error, 16);
        if( error != NULL) // 16진수에 에러 있는 경우
            return -1;
        Pinfo->memory = Pinfo->locr;
        strncpy(line_info[idx].symbol, Pinfo->argu[0], sizeof(line_info[idx].symbol));// start symbol
    }

   else if ( type == 3) // label있는 경우
        Pinfo->memory = Pinfo->locr - Pinfo->memory;

    *flag = 1;

     return -1;
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

int command_assemble(Hash hashTable, Symbol_table *sybolTable, char *command){
    FILE *fp;
    char buffer[256];
    char copy[256];
    char *argu[10];
    char *filename;
    int type, len , flag = 0; // flag는 시작이 start인지 아닌지 check
    size_t idx = 0;
    line_inform line_info[200];
    Pass1 Pinfo;

    filename = strtok(command, " \t");
    filename = strtok(NULL, " \t");
    fp = fopen(filename, "r" );
    if ( fp == NULL ){
        fprintf(stderr, "FILE OPEN ERROR\n");
        return -1;
    }

    symbol_init(sybolTable);
    while ( fgets(buffer, sizeof(buffer) , fp) != NULL) {
        len = strlen(buffer);
        if ( len == 0)
            continue;
        if ( buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        strncpy ( copy , buffer, sizeof(copy) );
        type = get_argu( copy , argu, hashTable);
        Pinfo.argu = argu;
        if ( make_line(copy, type, idx++, &flag, &Pinfo, line_info) == -1 )
            return -1;
    }

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
