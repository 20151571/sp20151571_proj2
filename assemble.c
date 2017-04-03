#include "assemble.h"

/* 두개의 symbol string을 비교해주는 함수
 *  a < b return -1
 *  a == b return 0
 *  a > b return 1
 */
int symbol_cmp(void *a, void *b){
    symbolPtr A = (symbolPtr)a;
    symbolPtr B = (symbolPtr)b;
    return strcmp(A->symbol, B->symbol);
}

/*
 * symbol들을 출력하는 함수
 * 정렬한 뒤 출력한다.
 */
void print_symbol(symbol_info *symbolTable, int size){
    int idx;
    char copy[256];
    int address;

    //selection sort
    for(int i = 0; i < size; ++i){
        idx = i;
        for( int j = i; j < size; ++j){
            if ( symbol_cmp(symbolTable[j].symbol, symbolTable[idx].symbol) < 0 )
                idx = j;
        }
        //swap
        address = symbolTable[idx].address;
        symbolTable[idx].address = symbolTable[i].address;
        symbolTable[i].address = address;

        strcpy(copy ,symbolTable[idx].symbol);
        strcpy(symbolTable[idx].symbol, symbolTable[i].symbol);
        strcpy(symbolTable[i].symbol, copy);
        
    }

    for( int i = 0; i < size; ++i){
        printf("\t%s\t%04X\n", symbolTable[i].symbol, 
                symbolTable[i].address);
    }
}

/*
 * whitespace('\t', ' ') 같은 것들을 삭제해주는 함수
 */
void delete_whitespace(char* str){
    int white_flag = 0;
    int i, len;
    int idx;
    int first_char = 0;
    int sep_flag = 0;

    len = strlen(str);
    for(i = 0; i <= len; i++){
        if(i == len){
            //EOF 넣음
            if(white_flag >= 1)
                str[i-white_flag] = '\0';
        }
        //whitespace 인경우 flag 증가
        if(str[i] == '\''){
            sep_flag++;
            white_flag = 0;
            if(sep_flag == 2)
                sep_flag = 0;
        }
        if(sep_flag)
            continue;

        if(str[i] == ' ' || str[i] == '\t'){
            white_flag++;
            if(str[i] == '\t')
                str[i] = ' ';
        }

        else{ // whitespace 아닌 경우
            if(white_flag >= 1){ // whitespace가 여려 개인 경우
                if(!first_char)
                    strncpy(str+idx, str+i, len-i+1);
                else if(str[i] == ',')
                    strncpy(str+idx, str+i, len-i+1), i--;
                // whitespace 만큼 당김
                else if(white_flag > 1)
                    strncpy(str+idx+1, str+i, len-i+1);
                str[idx+len-i+1] = '\0';

                len = strlen(str);
                i = idx;
            }
            // ','인 경우
            if(str[i] == ','){
                // Add a space behind of ','
                if(i+1 < 256 && (str[i+1] != ' ' && str[i+1] != '\t')){
                    char* tmp = (char*)malloc((len-i)*sizeof(char));
                    strcpy(tmp, str+i+1);
                    str[i+1] = ' ';
                    str[i+2] = '\0';
                    strcat(str,tmp);
                    len = strlen(str);
                    i--;
                }
            }
            // flag 초기화
            white_flag = 0;
            first_char = 1;
        }
        // idx 초기화
        if(white_flag == 1)
            idx = i;
    }
}

//symbol 초기화 함수
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

//opcode find function
//존재하면 opcode pointer를 return
//없으면 return NULL
Hnode opcode_find(Hash *hashTable, char *mnemonic){
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

//symbol table key 값을 구하는 함수
int get_Stablekey(Symbol_table *Stable, char *string){
    int key = 0;

    for ( size_t i = 0; i < strlen(string); ++i )
        key += string[i]; // get key

    return key % Stable->size;
}

// 해당하는 문자열과 일치하는 symbol 이 있는지 구해주는 함수
// return 값은 symbol pointer
// 없으면 return NULL
symbolPtr symbol_find(Symbol_table *Stable, char *string){
    int key = get_Stablekey(Stable, string);
    symbolPtr ptr;
    for ( ptr = Stable->table[key]; ptr != NULL; ptr = ptr -> next)
        if ( strcmp( ptr -> symbol, string) == 0)
            return ptr;
    return NULL;
}

//symbol insert function
void symbol_insert(Symbol_table *Stable, char *string, int address){
    int key = get_Stablekey(Stable, string);
    symbolPtr ptr = NULL;

    ptr = ( symbolPtr ) malloc ( sizeof (Symbol) );
    strncpy ( ptr->symbol, string, sizeof(ptr->symbol));
    ptr->address = address;
    ptr->next = Stable->table[key];
    Stable->table[key] = ptr;
}

//register find function
int get_register(char *string){

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


//assemble directives find function
int get_asmd(char *string) {

    for ( size_t i=0; i < strlen(string); ++i )
        string[i] = ( string[i] >= 'a' && string[i] <= 'z') ?
            string[i] -'a' + 'A' : string[i]; // 다 대문자로 만듬

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
/* operand가 byte인 경우 line에 operand에 byte 값을 넣고에러 있는지 확인해주는 함수
 */
int get_byte(char **argu, line_inform *line_info){
    char *string = &argu[2][1];
    char *error = NULL;
    int len = 0;

    if ( argu[3] != NULL ) // byte 값에서 추가로 하나 더 적혀 있는 경우 에
        return -1;

    if ( (argu[2][0] == 'C' || argu[2][0] == 'c') // C인지 X 인지 확인
            || (argu[2][0] == 'X' || argu[2][0] == 'x') ){
        argu[2][0] = ( 'A' <= argu[2][0]  && 'Z' >= argu[2][0] ) ?
            argu[2][0] : (argu[2][0] - 'a' + 'A');
    }

    if ( argu[2][0] == 'C' ){ // C 인 경우
        if ( string[0] != 39 || string[len] != 39)
            return -1;
        len = strlen(string) - 2;
    }

    else if ( argu[2][0] == 'X' ) { // X인 경우
        strtol(string + 1, &error, 16);
        if ( string[0] != 39 || *error != 39)
            return -1;
        len = error - string;
    }

    strcpy(line_info->operhand, argu[2]);
    return len;
}

// location counter function
// return counted location
int loc_count(char *string, int asmd, int location){
    int len = 0;
    char *token = NULL;
    char *error = NULL;
    token = strstr(string, "'");
    
    if ( asmd == byte ){ // byte의 경우 string length로 판단
        len = strstr(token + 1, "'") - token - 1;
        if ( strstr ( string, "C") != NULL || strstr ( string, "c") != NULL )
            location =  len;
        else if (  strstr ( string, "X") != NULL || strstr ( string, "x") != NULL )
            location = len / 2;
        else
            return -1;
    }

    else{ // 아닌 경우 word, resb, resw로 판단
        string = strtok(string, " \t\n\r");
        if ( asmd ==  word )
            location = 3;

        else if( asmd == resb )
            location = (int) strtol(string, &error, 10);
        else if (asmd == resw )
            location = (int) strtol(string, &error, 10) * 3;

        if( *error != '\0')
            return -1;
    }
    return location;
}

//해당하는 문자열이  주석인지 opcode인지 assemble directives인지 구해주는 함수
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

/*tokenize를 하여서해당하는라인이 comment인지 opcode인지 label이 없는지 확인해주는 함수
 * error가 발견되면 return 1
 * 정상적이라면 return 0
 */
int get_argu(char *buffer, char *argu[], Hash *hashTable){
    int len = 0, num;
    char sep[] = " \t\r\n";
    char *token = NULL;
    int type[5];
    
    //tokenize 과정
    delete_whitespace(buffer);
    if ( (token = strstr(buffer, ",")) != NULL){
        if ( *(token -1 ) == ' '){
            strcpy(token-1, token);
            token--;
        }
        if( *(token + 1) == ' ')
            strcpy(token+1, token + 2);
    }
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
        if ( strcmp(argu[0], "END") == 0 )
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

/*
 * object code를 구하는 함수
 * e = 0인 경우 3 byte로 계산
 * e = 1 인 경우 4 byte로 계산
 */
int get_objcode(int opcode, int n, int i, int x,
        int b, int p, int e, int address) {
    int code = 0;
    int bit[34] = { 0, };
    int len= 0 , idx = 0;
    opcode /= 4;

    if ( e == 1) // format 4
        len  = 31;
    else
        len = 23;

    for ( idx = 5; idx >= 0; --idx){ // get binary code 
        bit[idx] = opcode % 2;
        opcode /= 2;
    }

    bit[6] = n; bit[7] = i; bit[8] = x;
    bit[9] = b; bit[10] = p; bit[11] = e;

    for( idx = len; address >= 2; --idx){ // get binary code
        bit[idx] = address % 2;
        address /= 2;
    }

    bit[idx] = address;

    for ( idx = 0; idx <= len; ++idx){ // get decimal code
        code *= 2;
        code += bit[idx];
    }

    return code;
}

/*
 * 해당하는라인이 opcode 일 때  location counter function
 * error가 발견 되면 return -1
 * 정상적이라면 return 1을 한다
 */
int opcode_location(Pass1 *Pinfo, Hash *hashTable, char **argu, line_inform *line_info){
    Hnode ptr;
    int format = 0;
    if(argu[0][0] == '+')
        format = 4;

    else{
        ptr = opcode_find ( hashTable, argu[0]);
        if ( strcmp(ptr->code, "1" ) == 0)
            format = 1; // format 1
        else if ( strcmp(ptr->code, "2") == 0)
            format = 2; // format 2
        else if ( strcmp(ptr->code, "3/4") == 0)
            format = 3; // format 3
        else
            return -1;
    }

    line_info->location = Pinfo->location;
    line_info->format = format;
    Pinfo->location += format;
    return 1;
}

/*
 * immediate이나 indirect의 경우 붙는 '#' '@'를 제거해주고 오류가 있는지 확인하는 함수
 * 오류가 발견되면 return -1
 * else return 0
 */
int remove_char(char *string, Symbol_table *symbolTable, int *n, int *i){
    char *error = NULL;
    if(string[0] == '#' || string[0] == '@') { // immediate or indirect addressing mode
        string[0] == '#' ? (*i = 1) : (*n = 1);
        strcpy(string,string + 1);
        strtol(string, &error, 10);
        if ( *error != '\0' && symbol_find(symbolTable, string) == NULL)//symbol이 아니면 10진수
            return -1;
    }

    else //#도 @도 아니면 n은 1, i는 1(simple mode)
        *i = 1; *n = 1;
    if(strchr(string,'#') != NULL && strchr(string,'@') != NULL) //두 개다 있으면 에러
        return -1;
    return 0;
}

/*
 * assemble directive인 경우 line location을 처리 해주는 함수
 * assemble directive 처리과정에서 에러 난  경우 return -1
 * else return location
 */
int asmd_process(char **argu, line_inform *line_info, int location){
    //int byte_val;
    int asmd = get_asmd(argu[1]);
    strcpy(line_info->operhand, argu[2]);

    if ( asmd == byte )
        return loc_count(line_info->operhand, byte, location);

    else if ( asmd == word )
        return loc_count(line_info->operhand, word, location);

    else if ( asmd == resb )
        return loc_count(line_info->operhand, resb, location);

    else if ( asmd == resw )
        return loc_count(line_info->operhand, resw, location);
    return -1;
}

/*
 * obj에서 쓸 line을 생성하는 함수이다.
 * label이 있는 경우 argu[0] = label argu[1] = opcode or assembly directives
 */
int make_line(char *string, int type, size_t idx, int *flag,
        Symbol_table *Stable, Pass1 *Pinfo, line_inform *line_info) {

    char *error;
    int label_type;

    if ( type == comment ){ // comment 인 경우
        line_info[idx].format = -1;
        strncpy( line_info[idx].comment, string,
                sizeof(line_info[idx].comment));
        return 0;
    }

    else if ( type == opcode ){ // opcode인 경우
        if ( opcode_location(Pinfo, Stable->hashTable,
                    Pinfo->argu, &line_info[idx]) == -1 ) // opcode location counter function
            return -1; // error 
        strcpy( line_info[idx].opcode, Pinfo->argu[0]);
        if(Pinfo->argu[1] != NULL)
            strcpy( line_info[idx].operhand, Pinfo->argu[1]);
    }

    else if ( type == asmd){ // assembly directives
        strcpy(line_info[idx].asmd, Pinfo->argu[0]);
        strcpy(line_info[idx].operhand, Pinfo->argu[1]);
        line_info[idx].location = Pinfo->location;

        if ( strcmp( line_info[idx].asmd, "BASE" ) == 0 ){ // base인경우
            line_info[idx].base_flag = 1; // base-flag 
            strcpy(line_info[idx].operhand, Pinfo->argu[1]);
        }

        else
            Pinfo->location += asmd_process( Pinfo->argu , &line_info[idx], Pinfo->location);
        // base가 아닌 경우 location 구해줌
    }

    else if ( type == pass1_start ){ // start인 경우
        if ( (*flag) ) // start가 처음이 아닌 경우
            return -1;
        Pinfo->location = (int)strtol(Pinfo->argu[2], &error, 16);
        if( *error != '\0' ) // 16진수에 에러 있는 경우
            return -1;
        Pinfo->program_len = Pinfo->location;
        line_info[idx].location = Pinfo->location;
        strcpy(line_info[idx].asmd, "START");
        strncpy(line_info[idx].symbol, Pinfo->argu[0], sizeof(line_info[idx].symbol));// start symbol
        strcpy(line_info[idx].asmd, Pinfo->argu[1]);
        strcpy(line_info[idx].operhand, Pinfo->argu[2]); // file start line
        line_info[idx].label_flag = 1; // label이 있으므로 1
        if ( !symbol_find(Stable, line_info[idx].symbol) ){
            symbol_insert(Stable, line_info[idx].symbol,
                    Pinfo->location);
        }
        else
            return -1;
    }

    else if ( type == pass1_end ){ // end인 경우
        strcpy( line_info[idx].asmd, Pinfo->argu[0] );
        strcpy( line_info[idx].operhand, Pinfo->argu[1]);
        line_info[idx].location = Pinfo->location;
        Pinfo->program_len = line_info[idx-1].location - Pinfo->program_len + 1; //program length check
        return 0;
    }

    else if ( type == label ) { // label이 잇는 경우
        strcpy( line_info[idx].symbol , Pinfo->argu[0]);
        if ( !symbol_find(Stable, line_info[idx].symbol) ){
            symbol_insert(Stable, line_info[idx].symbol,
                    Pinfo->location); // symbol 삽입
        }
        else // 이미 있는 경우 에러 처리
            return -1;
        line_info[idx].label_flag = 1;
        label_type = get_type(Pinfo->argu[1], Stable->hashTable);

        if ( label_type == opcode ) { // opcode인 경우
            if ( opcode_location(Pinfo, Stable->hashTable,
                        &(Pinfo->argu[1]), &line_info[idx]) == -1 )
                return -1;
            strcpy( line_info[idx].opcode, Pinfo->argu[1]);
            strcpy( line_info[idx].operhand, Pinfo->argu[2]);
        }

        else if ( label_type == asmd){ // assemble directives
            strcpy(line_info[idx].asmd, Pinfo->argu[1]);
            strcpy(line_info[idx].operhand, Pinfo->argu[2]);
            line_info[idx].location = Pinfo->location;
            if ( strcmp( line_info[idx].asmd, "BASE" ) == 0 )
                line_info[idx].base_flag = 1;
            else
                Pinfo->location += asmd_process( Pinfo->argu , &line_info[idx], Pinfo->location);
        }

        else
            return -1;

        strcpy( line_info[idx].asmd, Pinfo->argu[1]);
    }

    else // 아무것도 아닌 경우 에러 처리
        return -1;
    *flag = 1; // start가 나온 경우 start가 처음에 나왓는지 체크
    return 1;
}

/*
 * command type 일 때 file 내용을 보여주는 함수
 */
void print_file(char *filename){
    FILE *fp;
    char buffer[500];
    fp = fopen(filename, "r");
    if ( fp == NULL )
        fprintf(stderr, "FILE OPEN ERROR\n");

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
        printf("%s", buffer);
}

/*
 * asseble command를 수행하는 함수
 * symbolTable은 symbol Table의 정보를 가지고 있고 success_symbol이란 변수는 assemble 성공하였을 때 symbol을 저장하는 구조체다.
 * 에러가 있는 경우 return 1
 * 없는 경우 return 0
 */
int command_assemble(Symbol_table *symbolTable, symbol_info *success_symbol, char *command){
    FILE *fp;
    char buffer[256];
    char copy[256];
    char *argu[10];
    char *filename;
    char *ptr;
    int type, len, flag = 0; // flag는 시작이 start인지 아닌지 check
    size_t idx = 0;
    line_inform line_info[200];
    Pass1 Pinfo;
    filename = strtok(command, " \t");
    filename = strtok(NULL, " \t");

    ptr = strstr(filename, "."); // get filename
    if ( ptr == NULL || strcmp( ptr, ".asm") != 0){ // not assembly file
        printf("please input assembly file\n");
        return 1; // error
    }

    fp = fopen(filename, "r");
    if( fp == NULL){ // no file error
        printf("NO file error!\n");
        return 1;
    }

    for ( int i = 0; i < 200; ++i){ // initialize
        line_info[i].label_flag = 0;
        line_info[i].location = 0;
    }

    symbol_init(symbolTable);

    // algorithm for pass1 of assembler
    while ( fgets(buffer, sizeof(buffer) , fp) != NULL) {
        strtok(buffer, "\n\r\0");
        len = strlen(buffer);
        if ( len == 0)
            continue;
        if ( buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        len = strlen(buffer);
        if ( len == 0)
            continue;
        strncpy ( copy , buffer, sizeof(copy) );

        type = get_argu( copy , argu, symbolTable->hashTable); // 해당하는 라인이 어떤 형식인지 구해주는 함수
        Pinfo.argu = argu;
        //make line function
        if ( make_line(copy, type, idx++, &flag, symbolTable, &Pinfo, line_info) == -1 )
            return 1;
    }

    fclose(fp);

    //pass algorithm function
    flag = assembler_pass2(filename, symbolTable, idx, &Pinfo, line_info);

    if ( flag == -1 ) //start가나왔는데 처음이 아니면
        return 1; // error

    // assemble 성공한 경우 symbol 저장
    idx = 0;
    for ( int i = 0; i < symbolTable->size; ++i){
        for( symbolPtr sptr = symbolTable->table[i]; sptr != NULL; sptr = sptr->next){
            success_symbol[idx].address = sptr->address;
            strcpy(success_symbol[idx].symbol, sptr->symbol);
            idx++;
        }
    }
    success_symbol[idx].address = -1;
    return 0;
}

/*
 * byte인 경우 object 코드를 생성해주는 함수
 * error가 발견 되면 return 1
 * else return 0
 */

int obj_byte(FILE *fp, Symbol_table *symbolTable,
        line_inform *line_info, object_inform *obj_info,
        int *obj_flag, int *idx){
    char c = line_info->operhand[0];
    char copy[30];
    char tmp[5];
    char *error = NULL;
    int len = 0, s = 0;
    strcpy(copy, line_info->operhand);

    c = ( 'A' <= c && c <= 'Z') ? c : c - 'a' + 'A';
    if ( c == 'C') { // char
        if ( strstr(copy, "'") == NULL){ // byte : C'~'
            printf("Error in byte\n");
            return 1;
        }

        line_info->obj_code = -2; // objcode 생성하기 위한 정보

        if ( strlen(copy + 2) > 31 ){ // byte는 최대 31길이
            printf("Error in byte length\n");
            return 1;
        }
        for ( int i = 2; copy[i] != 39 && copy[i] != '\0'; ++i){ // byte code 출력
            fprintf(fp, "%02X",(int)copy[i]);
            sprintf(tmp, "%02X", (int)copy[i]);
            strcat(line_info->obj_strcode, tmp);
            len++;
        }
        fprintf(fp, "\n");
    }

    else if( c == 'X'){ // hex 인 경우
        strtol(copy + 2, &error , 16); // 16진수인지 체크
        if ( strstr(copy, "'") == NULL || *error != 39){
            printf("Error in byte\n");
            return 1;
        }

        for ( int i = 2; copy[i] != 39 && copy[i] != '\0'; ++i ){
            len = i - 1;
            if( i >= 62){ // hex 이므로 31 bit 넘는지 체크.
                printf("Error in byte\n");
                return 1;
            }
            s++;
        }

        len /= 2;
        strtok (copy +2 , "'");
        fprintf(fp, "%s\n", copy + 2 );
        
        if(obj_info[*idx].size + len <= 30) // objcode information 저장
            obj_info[*idx].size += len;
        else{
            obj_info[++(*idx)].size += len;
            obj_info[*idx].start = line_info->location;
        }
        line_info->obj_code = -2;
    }

    else{
        printf("Error in byte\n");
        return 1;
    }

    if(*obj_flag == 1){ // object code 시작줄인 경우
        obj_info[++(*idx)].start = line_info->location;
        *obj_flag = 0;
    }

    if ( obj_info[*idx].size + len <= 30) // object code 길이가 30이하
        obj_info[*idx].size += len;

    else{
        obj_info[(*idx)].size += len;
        obj_info[*idx].start = line_info->location; // object code 새로운 라인
        //line_info->n_flag = s;
    }
    return 0;
}

/*
 * opcode인 경우 object code 생성하는 함수
 * if error found return 1
 * else return 0
 */
int obj_opcode(FILE *fp, Hash *hashTable, Symbol_table *symbolTable,
        line_inform *line_info, object_inform *obj_info,
        int *obj_idx, int *obj_flag, int *arr, int base_address) {

    Hnode hashptr;
    symbolPtr sptr;
    char tmp1[50] , tmp2[50], tmp3[50];
    char *error;
    int regi_num;
    int symbol_num;
    int immediate;
    int format = line_info->format;
    fprintf(fp,"%-10s\t%-10s\t",line_info->opcode,line_info->operhand);

    hashptr = opcode_find(hashTable, line_info->opcode); // opcode

    if(format == 1) { //format 1인 경우
        fprintf(fp,"\n");
        line_info->obj_code = (int)strtol(line_info->operhand,NULL,16);
    }

    else if ( format == 2) { //format 2인 경우
        sprintf(tmp1,"%X",hashptr->n_opcode);
        strcpy(tmp2,line_info->operhand);

        if(strstr(tmp2,",") != NULL) {// ','가 있는 경우
            regi_num = get_register(strtok(tmp2,",")); // register 구함
            sprintf(tmp3,"%d", regi_num);
            if(regi_num == -1) { //error 
                printf("ERROR in format 2\n");
                return -1;
            }
            strcat(tmp1, tmp3);
            regi_num = get_register(strtok(NULL,"\t\n\r "));
            sprintf(tmp3,"%d",regi_num);
            if(regi_num == -1) {
                printf("ERROR in format 2\n");
                return -1;
            }
            strcat(tmp1,tmp3);

            fprintf(fp,"%s\n",tmp1);
            line_info->obj_code = (int)strtol(tmp1,NULL,16);
        }

        else { //인자가 하나인 경우
            regi_num = get_register(tmp2);
            sprintf(tmp3,"%d",regi_num);
            if(regi_num == -1) { // error 
                printf("ERROR in format 2\n");
                return 1;
            }
            strcat(tmp1,tmp3);
            strcat(tmp1,"0");
            fprintf(fp,"%s\n",tmp1);
            line_info->obj_code = (int)strtol(tmp1,NULL,16);
        }
    }

    else if ( format == 3 ) { //format 3인 경우
        if(strcmp(line_info->opcode,"RSUB") == 0) {	//RSUB는 특이
            fprintf(fp,"4F0000\n");
            line_info->obj_code = 0x4F0000;
        }

        else {
            strcpy(tmp1, line_info->operhand);
            if(strstr(tmp1,",")!=NULL) { //x를 담당, index mode 확인
                if(strstr(tmp1,"X") != NULL || strstr(tmp1,"x") != NULL) {
                    arr[2] = 1; // x = 1
                    strtok(tmp1,",");
                }
                else {
                    printf("Error in format 3\n");
                    return 1;
                }
            }

            if(remove_char(tmp1,symbolTable, &arr[0], &arr[1] ) == -1) { //#, @ 및 n, i를 담당
                printf("ERROR in immediate or indirect addressing\n");
                return 1;
            }

            if( ( sptr = symbol_find(symbolTable, tmp1)) != NULL) {	//operhand가 symbol인 경우
                symbol_num = sptr->address;

                if(symbol_num - line_info->location >= -2048 && symbol_num - line_info->location <= 2047) {
                    arr[4] = 1; // p = 1
                    symbol_num = symbol_num - line_info->location - 3; //pc 가능 범위에 있으면 pc

                    if(symbol_num < 0) // hex code를 위해
                        symbol_num = symbol_num & 0x00000FFF;
                }

                else if(symbol_num - base_address >= 0 && symbol_num - base_address <=4095) {
                    arr[3] = 1; // b = 1
                    symbol_num = symbol_num - base_address;
                }

                else { //둘 다 아니면 에러
                    printf("ERROR in range\n");
                    return 1;
                }


                line_info->obj_code = get_objcode( hashptr->n_opcode, arr[0], arr[1], arr[2],
                        arr[3], arr[4], arr[5], symbol_num);
                //obj code calculate

                fprintf(fp,"%06X\n",line_info->obj_code);
            }
            else { //immediate인 경우
                arr[0] = 0; arr[1] = 1;
                immediate = (int)strtol(tmp1, &error, 10);
                if ( *error != '\0' ) { //10진수가 아니면
                    printf("Error in format 3\n");
                    return 1;
                }

                if ( immediate > 0x1000 ) {
                    printf("Error in immediate\n");
                    return 1;
                }

                line_info->obj_code = get_objcode(hashptr->n_opcode, arr[0], arr[1], arr[2],
                        arr[3], arr[4], arr[5], immediate);
                fprintf(fp,"%06X\n",line_info->obj_code);
            }

        }
    }

    else if ( format == 4 ) {								//format 4인 경우
        arr[5] = 1; // e = 1
        strcpy(tmp2, line_info->operhand);
        strcpy(tmp3, line_info->opcode);
        strcpy(tmp3, tmp3+1);

        hashptr = opcode_find(hashTable, tmp3); // opcode 인지 확인

        if(strstr(tmp2,",")!=NULL) {
            if(strstr(tmp2,"X") != NULL || strstr(tmp2,"x")) { // index mode check
                arr[2] = 1; // x = 1
                strtok(tmp2,",");
            }
            else {
                printf("ERROR in format 4\n");
                return 1;
            }
        }

        if(remove_char(tmp2, symbolTable, &arr[0], &arr[1]) == -1) { // immeidate or indirect check
            printf("ERROR in immediate or indirect addressing\n");
            return 1;
        }

        sptr = symbol_find(symbolTable, tmp2); // symbol check
        if(sptr != NULL) { //operhand가 symbol인 경우
            line_info->obj_code = get_objcode(hashptr->n_opcode,arr[0], arr[1], arr[2],
                    arr[3],arr[4],arr[5],sptr->address);
            fprintf(fp,"%08X\n",line_info->obj_code);
            line_info->modi_flag = 1;
        }

        else { //immediate인 경우
            symbol_num = (int)strtol(tmp2,&error,10);
            arr[0] = 0; arr[1] = 1;
            if(*error != '\0') { //10진수가 아니면
                printf("Error in immediate\n");					//에러 처리
                return 1;
            }

            if(symbol_num>0x1000) {
                printf("ERROR in format 4\n");
                return 1;
            }
            line_info->obj_code = get_objcode(hashptr->n_opcode,arr[0], arr[1], arr[2],
                    arr[3],arr[4],arr[5],symbol_num);
            fprintf(fp,"%08X\n",line_info->obj_code);
        }
    }

    //ojbect code 만들때 쓸 정보 저장
    if(*obj_flag == 1) {
        obj_info[++(*obj_idx)].start = line_info->location;
        *obj_flag = 0;
    }

    if(obj_info[*obj_idx].size + format <= 30)
        obj_info[*obj_idx].size += format;

    else {
        obj_info[++(*obj_idx)].size += format;
        obj_info[*obj_idx].start = line_info->location;
        line_info->n_flag = 1;
    }
    return 0;
}

/*
 * object file 만드는 함수
 * 위에서 저장한 obj information들을 이용하여 만듬
 */
void make_objfile(FILE *fp, int start_address, int start_line, line_inform *line_info, object_inform *obj_info) {	//오브젝트 파일 생성 함수
    int i = -1, idx = start_line; //임시 변수
    int e_flag = 0;	//end 플래그
    fprintf(fp,"H%s  %06X%06X",line_info[idx].symbol, line_info[idx].location, start_address); // start line

    while(1) {	//end가 아닐 때 까지
        if(e_flag == 1)
            break;
        i++;
        fprintf(fp,"\nT%06X%02X",obj_info[i].start, obj_info[i].size); // start address and size
        while(1) { // line 끝날 대 까지
            idx++;

            if(get_asmd(line_info[idx].asmd) == end) { // 도중에 end 들어온 경우
                e_flag = 1;
                break;
            }

            if(get_asmd(line_info[idx].asmd) == resb
                     || get_asmd(line_info[idx].asmd) == resw) { //resb, resw를 처리하는 부분	
                if(line_info[idx].n_flag == 1) { //줄바꿈을 한다
                    while(line_info[++idx].n_flag == 1);
                }
            }
            else if(line_info[idx].obj_code != -1 
                    && line_info[idx].obj_code != -2) {
                if(line_info[idx].format == 2) //format 2는 4글자
                    fprintf(fp, "%04X", line_info[idx].obj_code);					
                else //아니면 6글자
                    fprintf(fp, "%06X", line_info[idx].obj_code);
            }
            else if(line_info[idx].obj_code == -2) //BYTE의 경우는 문자열을 출력
                fprintf(fp, "%s", line_info[idx].obj_strcode);

            if(line_info[ idx + 1 ].n_flag == 1) // 줄바꿈
                break;
        }
    }

    for(idx = 0; idx<200; idx++) {	// Modification
        if(line_info[idx].modi_flag == 1) {
            fprintf(fp,"\nM%06X05",line_info[idx].location + 1);
        }
    }

    fprintf(fp,"\nE%06X\n",line_info[start_line].location);	//마지막줄
}

/*
 * pass2 algorithm을 구현한 함수이다
 * if error found return 1
 * else return 0
 */
int assembler_pass2(char *filename, Symbol_table *symbolTable, int length,
        Pass1 *Pinfo, line_inform *line_info){
    FILE *fp, *lstfp , *objfp;
    char lstname[30];
    char objname[30];
    char buffer[256];
    char bef[256];
    char copy[256];
    char *errorp;
    int arr[7];
    int linenum  = 0, type,  num;
    size_t idx = 0, len;
    int end_flag = 0, error = 0, obj_flag = 0, obj_idx = 0;
    int n, i, x, b, p , e;
    int asmd, start_line;
    Hash *hashTable = symbolTable->hashTable;
    symbolPtr  sptr;
    object_inform obj_info[300];

    strcpy(copy, filename);
    strtok(copy, ".");
    strcpy(lstname, copy);
    strcat(lstname, ".lst");
    strcpy(objname, copy);
    strcat(objname, ".obj");

    fp = fopen(filename, "r");
    lstfp = fopen(lstname, "w"); // lst file pointer
    objfp = fopen(objname, "w"); // object file pointer
    if ( lstfp == NULL || objfp == NULL){
        fprintf(stderr, "FILE OPEN ERROR!\n");
        return -1;
    }

    while ( fgets(buffer, sizeof(buffer), fp) != NULL){
        if(error == 1) { //error find code
            printf("************* ERROR LINE **************\n");
            printf("line : %d : %s", linenum , bef);
            printf("***************************************\n");
            fclose(lstfp);
            fclose(objfp);
            remove(lstname);
            remove(objname);
            return -1;
        }

        n = 0, i = 0, x = 0, b = 0, p = 0, e = 0;
        strtok(buffer, "\n\r\0");
        len = strlen(buffer);
        if ( len == 0)
            continue;
        if ( buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';
        len = strlen(buffer);
        if ( len == 0)
            continue;
        strcpy(bef, buffer); // error check 할 때 출력할 string
        linenum += 5;

        fprintf(lstfp,"%-5d\t",linenum); // line number 
        strcpy(copy, buffer);
        asmd = get_asmd(line_info[idx].asmd); // assemble directives check

        if(line_info[idx].format == -1) { //comment인 경우
            fprintf(lstfp,"\t%s\n",buffer);
            idx++;
            continue;
        }

        else if( asmd == base || asmd == end ) { //base나 end인 경우
            if( asmd == base) {	//base인 경우
                if ( ( sptr = symbol_find(symbolTable, line_info[idx].operhand)) != NULL)
                    Pinfo->base_address = sptr->address; // base address 저장

                else {
                    printf("Error in base\n");
                    error = 1;
                    continue;
                }
            }

            else // base 아니면 end
                end_flag = 1;
            fprintf(lstfp,"\t\t%-10s\t%-10s\n",
                    line_info[idx].asmd,line_info[idx].operhand);
            idx++;
            continue;
        }

        fprintf(lstfp,"%04X\t",line_info[idx].location); //주소 출력


        if ( line_info[idx].label_flag ) // label이 있는 경우
            fprintf(lstfp, "%s\t", line_info[idx].symbol); // label 출력

        else
            fprintf(lstfp, "\t");

        if(opcode_find(hashTable, line_info[idx].opcode) == NULL &&
                opcode_find(hashTable, (line_info[idx].opcode)+1) == NULL &&
                get_asmd(line_info[idx].asmd) == -1) {	//opcode 및 assemble directives 둘 다 없는 경우

            printf("Error in opcode or symbol\n");
            error = 1;
            continue;
        }

        if(get_asmd(line_info[idx].asmd) != -1) { //asmd가 있는 경우
            fprintf(lstfp,"%-10s\t%-10s\t",line_info[idx].asmd, line_info[idx].operhand); //asmd 출력
            type = get_asmd(line_info[idx].asmd);
            if(type == 0) { //start인 경우
                line_info[idx].obj_code = -1;
                fprintf(lstfp,"\n");
                obj_info[obj_idx].start = line_info[idx].location;	//오브젝트 파일 시작 주소
                start_line = idx;
                idx++;
                continue;
            }

            else if(type == 3) { //byte인 경우
                error = obj_byte(lstfp, symbolTable, &line_info[idx],
                        obj_info, &obj_flag, &obj_idx);
                // object byte code 생성
                idx++;
                continue;
            }

            else if(type == 4){ //word인 경우
                num = strtol(line_info[idx].operhand, &errorp, 10);
                if(*errorp != '\0') {
                    printf("Error in WORD\n");
                    error = 1;
                    continue;
                }
                line_info[idx].obj_code = num;

                if(line_info[idx].obj_code < 0)
                    line_info[idx].obj_code &= 0x00FFFFFF; // 3byte
                fprintf(lstfp,"%06X\n",line_info[idx].obj_code);


                //object code 생성중
                if(obj_flag == 1) {
                    obj_info[++obj_idx].start = line_info[idx].location; // 라인 시작 주소
                    obj_flag = 0;
                }

                if(obj_info[i].size + 3 <= 30)
                    obj_info[obj_idx].size += 3; // object code line size check

                else {
                    obj_info[++obj_idx].size += 3;
                    obj_info[obj_idx].start= line_info[idx].location; // 새로운 줄
                    line_info[idx].n_flag = 1;
                }
            }
            else if(type == 5) { //resb인 경우
                num = strtol(line_info[idx].operhand, &errorp, 10);
                if( *errorp != '\0') {
                    printf("Error in WORD\n");
                    error = 1;
                    continue;
                }
                fprintf(lstfp,"\n");
                line_info[idx].n_flag = 1;
                obj_flag = 1;
            }

            else if(type == 6) { //resw인 경우
                num = strtol(line_info[idx].operhand, &errorp, 10);
                if( *errorp != '\0') {
                    printf("Error in RESW\n");
                    error = 1;
                    continue;
                }
                fprintf(lstfp,"\n");
                line_info[idx].n_flag = 1;
                obj_flag = 1;
            }
        }
        else { //opcode 인 경우
            arr[0] = n; arr[1] = i; arr[2] = x; arr[3] = b; arr[4] = p; arr[5] = e;
            error = obj_opcode(lstfp, hashTable, symbolTable, &line_info[idx],
                    obj_info,&obj_idx, &obj_flag, arr, Pinfo->base_address);
            if ( error )
                continue;
        }
        idx++;
    }

    if ( !end_flag && !error){
        printf("NO END in assemble code\n");
        remove(lstname);
        remove(objname);
        return -1;
    }
    // 위에서 저장한 obj information으로 작성
    make_objfile(objfp, Pinfo->program_len , start_line, line_info, obj_info);
    printf("\toutput file : [%s], [%s]\n", lstname, objname);
    fclose(lstfp);
    fclose(objfp);
    return 1;
}

/* 
 * 현재 디렉토리에 있는 filename 파일을 읽어서 화면에 출력한다.
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

    if(filename == NULL){
        printf("Please input filename\n");
        return -1;
    }
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

    if ( flag == 1)
        return 1;
    else{
        printf("type command error!\n");
        return -1;
    }
}

/*
 * symbol command 수행하는 함수이다.
 *  각 symbol의 주소값을 출력한다.
 */
void command_symbol(symbol_info *symbolTable) {
    int size = 0;
    for ( int i = 0; i < 500; ++i){
        if ( symbolTable[i].address == -1)
            size = i;
    }
    if(size != 0)
        print_symbol(symbolTable, size);
 
    else
        printf("NO SYMBOL TABLE\n");
}
