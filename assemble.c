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

Hnode opcode_find(Hash *hashTable, char *mnemonic){
    int opcode = -1;
    int i;
    Hnode ptr;
    for ( i = 0; i < hashTable->size; ++i ){ // hash_table에서 일일이 찾는다
        for ( ptr = hashTable->Table[i]; ptr != NULL; ptr = ptr -> next ){
            if ( strcmp( ptr->str_opcode , mnemonic ) == 0)
                return ptr;
        }
    }
    return NULL; // 존재하지 않는 경우
}

int get_Stablekey(Symbol_table *Stable, char *string){
    int key = 0;

    for ( size_t i = 0; i < strlen(string); ++i )
        key += string[i];

    return key % Stable->size;
}
int symbol_find(Symbol_table *Stable, char *string){
    int key = get_Stablekey(Stable, string);
    symbolPtr ptr;
    for ( ptr = Stable->table[key]; ptr != NULL; ptr = ptr -> next)
        if ( strcmp( ptr -> symbol, string) == 0)
            return 1;
    return 0;
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
/*  byte인 경우 line에 operhand에 byte 값을 넣고
 *
 */
int get_byte(char **argu, line_inform *line_info){
    char copy[50];
    strncpy(copy, argu[1], sizeof(copy));
    if ( argu[2] != NULL ) // byte 값에서 추가로 하나 더 적혀 있는 경우 에
        return -1;
    if ( (copy[0] == 'C' || copy[0] == 'c')
            || (copy[0] == 'X' || copy[0] == 'x') ){
        copy[0] = ( 'A' <= copy[0]  && 'Z' >= copy[0] ) ?
            copy[0] : (copy[0] - 'a' + 'A');
    }
}

int loc_count(char *string, int asmd){
    int len = 0;
    char *token = NULL;
    int location;
    token = strstr(string, "''");
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

int get_type(char *string, Hash *hashTable){
    if ( strcmp (string , "." ) == 0 )
        return comment;
    else if( ( string[0] == '+' && opcode_find ( hashTable, string + 1) != NULL )
            ||  opcode_find ( hashTable, string ) != NULL  )
        return opcode; // opcode
    else if ( get_asmd(string) != -1 )
        return asmd; // assembly directives
    return label; // label
}

int get_argu(char *buffer, char *argu[], Hash *hashTable){
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

    argu[len] = NULL;

    if ( type[0] == comment) // comment
        return comment; 
    else if ( type[0] == opcode) // opcode
        return opcode;
    else if( type[0] == asmd){ // assembly directives
        if ( get_asmd(argu[0]) == end )
            return pass1_end;
        return asmd;
    }

    else if ( type[0] == label){ // label
        num = get_asmd(argu[1]);
        if( num == start )
            return pass1_start; // label이 있는 start
        return label;
    }
    return -1;
}

int opcode_process(Pass1 *Pinfo, Hash *hashTable, char **argu, line_inform *line_info){
    Hnode ptr;
    int format = 0;
    if(argu[0][0] == '+')
        format = 4;

    else{
        ptr = opcode_find ( hashTable, argu[0]);
        if ( strcmp(ptr->code, "1" ) == 0)
            format = 1;
        else if ( strcmp(ptr->code, "2") == 0)
            format = 2;
        else if ( strcmp(ptr->code, "3/4") == 0)
            format = 3;
        else
            return -1;
    }
    line_info->location = Pinfo->location;
    line_info->format = format;
    Pinfo->location += format;
    return 1;
}

int asmd_process(char **argu, line_inform *line_info){
    int byte_val;
    int asmd = get_asmd(argu[0]);
    strcpy (line_info->operhand, argu[1]);
    if ( asmd== byte ){
        byte_val = get_byte(argu, line_info);
        return loc_count(line_info->operhand, byte);
    }

    else if ( asmd == word )
        return loc_count(line_info->operhand, word);

    else if ( asmd == resb )
        return loc_count(line_info->operhand, resb);

    else if ( asmd == resw )
        return loc_count(line_info->operhand, resw);
    return -1;
}

/*obj에서 쓸 line을 생성하는 함수이다.
 * label이 있는 경우 argu[0] = label argu[1] = opcode or assembly directives
 */
int make_line(char *string, int type, size_t idx, int *flag,
        Symbol_table *Stable, Pass1 *Pinfo, line_inform *line_info){
    char *error;
    if ( type == comment ){ // comment 인 경우
        line_info[idx].format = 0;
        strncpy( line_info[idx].comment, string,
                sizeof(line_info[idx].comment));
        return 0;
    }

    else if ( type == opcode )
        return opcode_process(Pinfo, Stable->hashTable, Pinfo->argu, &line_info[idx]);

    else if ( type == asmd){ // assembly directives
        strcpy(line_info[idx].asmd, Pinfo->argu[0]);
        line_info[idx].location = Pinfo->location;
        Pinfo->location += asmd_process( Pinfo->argu , &line_info[idx]);
    }

    else if ( type == pass1_start ){ // start인 경우
        if ( !(*flag) ) // start가 처음이 아닌 경우
            return -1;
        Pinfo->location = strtol(Pinfo->argu[2], &error, 16);
        if( error != NULL) // 16진수에 에러 있는 경우
            return -1;
        Pinfo->program_len = Pinfo->location;
        strncpy(line_info[idx].symbol, Pinfo->argu[0], sizeof(line_info[idx].symbol));// start symbol
    }

    else if ( type == pass1_end ) // end인 경우
        Pinfo->program_len = Pinfo->location - Pinfo->program_len;

    else if ( type == label ) {
        strcpy( line_info[idx].symbol , Pinfo->argu[0]);
        // line_info[idx].format = 0;
        strcpy( line_info[idx].asmd, Pinfo->argu[1]);
        asmd_process ( &(Pinfo->argu[1]), &line_info[idx]);
        if ( !symbol_find(Stable, line_info[idx].symbol) )
            symbol_insert(Stable, line_info[idx].symbol,
                    line_info[idx].location);
        else
            return -1;
    }
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

int command_assemble(Symbol_table *symbolTable, char *command){
    FILE *fp;
    char buffer[256];
    char copy[256];
    char *argu[10];
    char *filename;
    int type, len, flag = 0; // flag는 시작이 start인지 아닌지 check
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

    symbol_init(symbolTable);
    while ( fgets(buffer, sizeof(buffer) , fp) != NULL) {
        len = strlen(buffer);
        if ( len == 0)
            continue;
        if ( buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        strncpy ( copy , buffer, sizeof(copy) );
        type = get_argu( copy , argu, symbolTable->hashTable);
        Pinfo.argu = argu;
        if ( make_line(copy, type, idx++, &flag, symbolTable, &Pinfo, line_info) == -1 )
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
