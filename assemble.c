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
symbolPtr symbol_find(Symbol_table *Stable, char *string){
	int key = get_Stablekey(Stable, string);
	symbolPtr ptr;
	for ( ptr = Stable->table[key]; ptr != NULL; ptr = ptr -> next)
		if ( strcmp( ptr -> symbol, string) == 0)
			return ptr;
	return NULL;
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
        return 1;
	}
    return -1;
}

int loc_count(char *string, int asmd, int location){
	int len = 0;
	char *token = NULL;
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

int get_objcode(int opcode, int n, int i, int x, int b,
        int p, int e, int address){
    int opcode_tmp, code = 0, code_tmp = 1;
    int bit[34] = { 0, };
    int len= 0 , idx = 0;
    opcode_tmp = opcode / 4;

    if ( e == 1)
        len  = 32;
    else
        len = 23;

    for ( idx = 5; idx >= 0; --i){
        bit[idx] = opcode_tmp % 2;
        opcode_tmp /= 2;
    }

    bit[6] = n; bit[7] = i; bit[8] = x;
    bit[9] = b; bit[10] = p; bit[11] = e;
    for( idx = len; address >= 2; --idx){
        bit[idx] = address % 2;
        address /= 2;
    }
    bit[idx] = address;
    
    for( idx = 0; idx <= 23 + 8 * e; ++i ){
        if(bit[i] == 1)
            for ( int j = 0; j < 23 + 8 *e -i; ++j)
                code_tmp *= 2;
        code += code_tmp;
        code_tmp = 1;
    }

    return code;
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

int asmd_process(char **argu, line_inform *line_info, int location){
	int byte_val;
	int asmd = get_asmd(argu[0]);
	strcpy (line_info->operhand, argu[1]);
	if ( asmd== byte ){
		byte_val = get_byte(argu, line_info);
		return loc_count(line_info->operhand, byte, location);
	}

	else if ( asmd == word )
		return loc_count(line_info->operhand, word, location);

	else if ( asmd == resb )
		return loc_count(line_info->operhand, resb, location);

	else if ( asmd == resw )
		return loc_count(line_info->operhand, resw, location);
	return -1;
}

/*obj에서 쓸 line을 생성하는 함수이다.
 * label이 있는 경우 argu[0] = label argu[1] = opcode or assembly directives
 */
int make_line(char *string, int type, size_t idx, int *flag,
		Symbol_table *Stable, Pass1 *Pinfo, line_inform *line_info){
	char *error;
	int label_type;
	if ( type == comment ){ // comment 인 경우
		line_info[idx].format = 0;
		strncpy( line_info[idx].comment, string,
				sizeof(line_info[idx].comment));
		return 0;
	}

	else if ( type == opcode )
		return opcode_process(Pinfo, Stable->hashTable,
				Pinfo->argu, &line_info[idx]);

	else if ( type == asmd){ // assembly directives
		strcpy(line_info[idx].asmd, Pinfo->argu[0]);
		line_info[idx].location = Pinfo->location;
		Pinfo->location += asmd_process( Pinfo->argu , &line_info[idx], Pinfo->location);
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
		if ( !symbol_find(Stable, line_info[idx].symbol) )
			symbol_insert(Stable, line_info[idx].symbol,
					line_info[idx].location);
		else
			return -1;
        line_info[idx].label_flag = 1;
		label_type = get_type(Pinfo->argu[1], Stable->hashTable);

		if(label_type == opcode){
			return opcode_process(Pinfo, Stable->hashTable,
					&(Pinfo->argu[1]), &line_info[idx]);
		}

		else if ( label_type == asmd){
			strcpy(line_info[idx].asmd, Pinfo->argu[0]);
			line_info[idx].location = Pinfo->location;
			Pinfo->location += asmd_process( Pinfo->argu, &line_info[idx], Pinfo->location);
		}

		else 
			return -1;
		strcpy( line_info[idx].symbol , Pinfo->argu[0]);
		// line_info[idx].format = 0;
		strcpy( line_info[idx].asmd, Pinfo->argu[1]);
		asmd_process ( &(Pinfo->argu[1]), &line_info[idx], Pinfo->location);
	}

	else
		return -1;

	*flag = 1;
	return 1;
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

    for ( int i = 0; i < 200; ++i){
        line_info[i].label_flag = 0;
        line_info[i].location = 0;
    }

	symbol_init(symbolTable);
	// algorithm for pass1 of assembler 
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
	fclose(fp);
	assembler_pass2(filename, symbolTable, idx, &Pinfo, line_info);

	return 1;
}

int obj_byte(file *fp, Symbol_table *symbolTable, line_inform *line_info ){
    if ( line_info->operhand[0] == '')
}

int assembler_pass2(char *filename, Symbol_table *symbolTable, int length,
		Pass1 *Pinfo, line_inform *line_info){
	FILE *fp, *lstfp , *objfp;
	char lstname[30];
	char objname[30];
	char buffer[256];
	char copy[256];
	int linenum  = 0, type, symbol_address;
	size_t idx = 0;
	int e_flag = 0, error = 0;
	int n, i, x, b, p , e;
    Hash *hashTable = symbolTable->hashTable;
    symbolPtr  sptr;
    Hnode hashptr;
	strcpy(lstname, filename);
	strcat(lstname, ".lst");
	strcpy(objname, filename);
	strcat(objname, ".obj");

	fp = fopen(filename, "r");
	lstfp = fopen(lstname, "w");
	objfp = fopen(objname, "w");
	if ( lstfp == NULL || objfp == NULL){
		fprintf(stderr, "FILE OPEN ERROR!\n");
		return -1;
	}

	while ( fgets(buffer, sizeof(buffer), fp) != NULL){
		n = 0, i = 0, x = 0, b = 0, p = 0, e = 0;
		if ( buffer[strlen(buffer) - 1] == '\n')
			buffer[strlen(buffer) - 1] = '\0';

		linenum += 5;

		fprintf(lstfp,"%-5d\t",linenum);
		strcpy(copy, buffer);

		if(line_info[idx].format == 0) {										//comment인 경우
			fprintf(lstfp,"\t%s",buffer);
			continue;
		}

        else if ( ( sptr = symbol_find(symbolTable, line_info[idx].symbol) ) != NULL ){ //  label이 있는 경우
            symbol_address = sptr->address - line_info[idx].location - 3;
            if ( -2048 <= symbol_address && symbol_address <= 2047 ){
                p = 1;
                if ( symbol_address < 0)
                    symbol_address = symbol_address & 0x00000FFF;

                hashptr = opcode_find(hashTable, line_info[idx].symbol);
                line_info[idx].obj_code = make_objcode(hashptr->n_opcode, n, i, x, b, p, e, symbol_address); 
            }
        }

		else if(get_asmd(line_info[idx].asmd) == base ||
                    get_asmd(line_info[idx].asmd) == end) {				//base나 end인 경우

            if(get_asmd(line_info[idx].asmd) == base) {								//base인 경우
				if ( ( sptr = symbol_find(symbolTable, line_info[idx].operhand)) != NULL)
					Pinfo->base_address = line_info[idx].location;
                else {
					printf("Error in base\n");
					error = 1;
					continue;
				}
			}

			else
                e_flag = 1;

            fprintf(lstfp,"\t\t%-10s\t%-10s\n",
                    line_info[idx].asmd,line_info[idx].operhand);
			continue;
		}
		fprintf(lstfp,"%04X\t",line_info[idx].location);								//주소 출력

        if ( line_info[idx].label_flag )
            fprintf(lstfp, "%s\t", line_info[idx].symbol);
        else
            fprintf(lstfp, "\t");

		if(opcode_find(hashTable, line_info[idx].opcode) == NULL &&
				opcode_find(hashTable, (line_info[idx].opcode)+1) == NULL &&
				get_asmd(line_info[idx].asmd) == -1) {								//opcode 및 asmd 둘 다 없는 경우
			printf("Error in opcode or symbol\n");
			error = 1;
			continue;
		}

		if(symbol_find(symbolTable,line_info[idx].symbol) != NULL)						//symbol 출력
			fprintf(lstfp,"%s\t",line_info[idx].symbol);
		else 
			fprintf(lstfp,"\t");

		if(get_asmd(line_info[idx].asmd) != -1) {									//asmd가 있는 경우
			fprintf(lstfp,"%-10s\t%-10s\t",line_info[idx].asmd,line_info[idx].operhand);				//asmd 출력
			type = get_asmd(line_info[idx].asmd);
			if(type == 0) {											//start인 경우
				line_info[idx].obj_code = -1;
				fprintf(lstfp,"\n");
                /*
				obj_info[i].start = line_info[idx].location;						//오브젝트 파일 시작 주소
				start_line = idx;
                */
				continue;
			}
			else if(type == 3) {										//byte인 경우
				if(asmd_char[0] == 'C' || asmd_char[0] == 'c') {					//character인 경우
					if(strstr(asmd_char,"'") == NULL) {						//에러 처리
						printf("ERROR in BYTE\n");
						error = 1;
						continue;
					}

					line_info[idx].obj_code = -2;							//obj code를 문자열로 저장
					strcpy(line_info[idx].obj_scode,"");
					if(strlen(asmd_char+2)>31) {
						printf("ERROR in BYTE long\n");
						error = 1;
						continue;
					}

					for(j=2; ((asmd_char[j]!=39||asmd_char[j+1]!='\0') && s<=29);j++) {
						fprintf(lstfp,"%02X",(int)asmd_char[j]);
						sprintf(t_char1,"%02X",(int)asmd_char[j]);
						strcat(line_info[idx].obj_scode,t_char1);
						s++;
					}
					fprintf(lstfp,"\n");
					if(obj_flag == 1) {															
						i++;															
						obj_info[i].start = line_info[idx].location;						
						obj_flag = 0;
					}
					if(obj_info[i].size + s <= 30) {						//오브젝트 파일 출력을
						obj_info[i].size += s;							//하기 위해서 설정하는 부분
					}										//길이를 담당하는 부분
					else {										//아래에도 같은 코드 반복
						i++;
						obj_info[i].size += s;
						obj_info[i].start = line_info[idx].location;
						line_info[idx].n_flag = 1;
					}
					continue;

				}
				else if(asmd_char[0] == 'X' || asmd_char[0] =='x') {					//hex인 경우
					if(strstr(asmd_char,"'") == NULL) {
						printf("ERROR in BYTE\n");
						error = 1;
						continue;
					}

					strcpy(t_char2,asmd_char);
					asmd_charp = strtok(t_char2+2,"'");
					if(str_determinent(asmd_charp,16) == -1) {					//16진수인지 확인
						printf("ERROR in BYTE\n");
						error = 1;
						continue;
					}
					line_info[idx].obj_code = -2;							//obj code를 문자열로 저장
					strcpy(line_info[idx].obj_scode,asmd_charp);
					if(strlen(asmd_charp)%2 != 0) {
						printf("ERROR in BYTE\n");
						error = 1;
						continue;
					}
					if(strlen(asmd_charp) > 60) {
						printf("ERROR in BYTE");
						error = 1;
						continue;
					}
					s = strlen(asmd_charp)/2;

					fprintf(lstfp,"%s\n",asmd_charp);
					if(obj_flag == 1) {
						i++;
						obj_info[i].start = line_info[idx]].location;
						obj_flag = 0;
					}
					if(obj_info[i].size + s <= 30) {
						obj_info[i].size += s;
					}
					else {
						i++;
						obj_info[i].size += s;
						obj_info[i].start = line_info[idx].location;
						line_info[idx].n_flag = 1;
					}

					continue;
				}
				else {
					printf("ERROR in BYTE\n");
					error = 1;
					continue;
				}
			}
			else if(key == 4) {										//word인 경우
				if(str_determinent(line_info[idx].operhand,10) == -1) {
					printf("ERROR in WORD\n");
					error = 1;
					continue;
				}

				strcpy(asmd_char,line_info[idx].operhand);
				line_info[idx].obj_code = (int)strtol(asmd_char,NULL,10);					//10진수인지 확인
				if(line_info[idx].obj_code < 0) {
					line_info[idx].obj_code = line_info[idx].obj_code & 0x00FFFFFF;
				}
				fprintf(lstfp,"%06X\n",line_info[idx].obj_code);
				if(obj_flag == 1) {
					i++;
					obj_info[i].start = line_info[idx].location;
					obj_flag = 0;
				}	

				if(obj_info[i].size + 3 <= 30) {
					obj_info[i].size += 3;
				}
				else {
					i++;
					obj_info[i].size += 3;
					obj_info[i].start= line_info[idx].location;
					line_info[idx].n_flag = 1;
				}
			}
			else if(type == 5) {										//resb인 경우
				if(str_determinent(line_info[idx].operhand,10) == -1) {
					printf("ERROR in RESB\n");
					error = 1;
					continue;
				}
				fprintf(lstfp,"\n");
				line_info[idx].n_flag = 1;
				obj_flag = 1;
			}
			else if(type == 6) {										//resw인 경우
				if(str_determinent(line_info[idx].operhand,10) == -1) {
					printf("ERROR in RESW\n");
					error = 1;
					continue;
				}
				fprintf(lstfp,"\n");
				line_info[idx].n_flag = 1;
				obj_flag = 1;
			}

		}
		else {													//opcode인 경우
			fprintf(lstfp,"%-10s\t%-10s\t",line_info[idx].opcode,line_info[idx].operhand);
			if(line_info[idx].format == 1) {									//format 1인 경우
				fprintf(lstfp,"\n");
				t_opcode = optext_search2(line_info[idx].opcode, h_table);
				line_info[idx].obj_code = strtol(line_info[idx].operhand,NULL,16);

				if(obj_flag == 1) {
					i++;
					obj_info[i].start = line_info[idx].location;
					obj_flag = 0;
				}
				if(obj_info[i].size + 1 <= 30) {
					obj_info[i].size += 1;
				}
				else {
					i++;
					obj_info[i].size += 1;
					obj_info[i].start = line_info[idx].location;
					line_info[idx].n_flag = 1;
				}
			}
			else if(line_info[idx].format == 2) {								//format 2인 경우
				t_opcode = optext_search2(line_info[idx].opcode,h_table);
				sprintf(t_char1,"%X",t_opcode->opcode);
				strcpy(t_char2,line_info[idx].operhand);

				if(strstr(t_char2,",") != NULL) {							// ','가 있는 경우
					register_int = find_register1(strtok(t_char2,","));				//그러니까 인자가 두 개인 경우


					sprintf(t_char3,"%d",register_int);
					if(register_int == -1) {
						printf("ERROR in format 2\n");
						error = 1;
						continue;
					}
					strcat(t_char1,t_char3);

					register_int = find_register1(strtok(NULL,","));
					sprintf(t_char3,"%d",register_int);
					if(register_int == -1) {
						printf("ERROR in format 2\n");
						error = 1;
						continue;
					}
					strcat(t_char1,t_char3);

					fprintf(lstfp,"%s\n",t_char1);
					line_info[idx].obj_code = (int)strtol(t_char1,NULL,16);
				}
				else {											//인자가 하나인 경우
					register_int = find_register1(t_char2);
					sprintf(t_char3,"%d",register_int);
					if(register_int == -1) {
						printf("ERROR in format 2\n");
						error = 1;
						continue;
					}
					strcat(t_char1,t_char3);
					strcat(t_char1,"0");
					fprintf(lstfp,"%s\n",t_char1);
					line_info[idx].obj_code = (int)strtol(t_char1,NULL,16);
				}

				if(obj_flag == 1) {
					i++;
					obj_info[i].start = line_info[idx].location;
					obj_flag = 0;
				}
				if(obj_info[i].size + 2 <= 30) {
					obj_info[i].size += 2;
				}
				else {
					i++;
					obj_info[i].size += 2;
					obj_info[i].start = line_info[idx].location;
					line_info[idx].n_flag = 1;
				}
			}
			else if(line_info[idx].format == 3) {								//format 3인 경우
				t_opcode = optext_search2(line_info[idx].opcode,h_table);
				if(strcmp(line_info[idx].opcode,"RSUB") == 0) {						//RSUB은 특별하다
					fprintf(lstfp,"4F0000\n");
					line_info[idx].obj_code = 0x4F0000;
				}
				else {
					strcpy(t_char1, line_info[idx].operhand);

					if(strstr(t_char1,",")!=NULL) {							//x를 담당
						if(strstr(t_char1,"X") != NULL || strstr(t_char1,"x") != NULL) {
							x = 1;
							strtok(t_char1,",");
						}
						else {
							printf("ERROR in format 3\n");
							error = 1;
							continue;
						}
					}

					if(hash_remove(t_char1,&n,&ia,s_table) == -1) {					//#, @ 및 n, i를 담당
						printf("ERROR in immediate or indirect addressing\n");
						error = 1;
						continue;						
					}

					if(symbol_search(s_table,t_char1) != NULL) {					//operhand가 symbol인 경우
						symbol_int = symbol_search(s_table,t_char1)->location;
						if(symbol_int - line_info[idx].location - 3 >= -2048 && symbol_int - line_info[k].location - 3 <= 2047) {
							p = 1;
							symbol_int = symbol_int - line_info[idx].location - 3;		//pc 가능 범위에 있으면 pc
							if(symbol_int < 0) 
								symbol_int = symbol_int & 0x00000FFF;
							t_opcode = optext_search2(line_info[idx].opcode,h_table);
							line_info[idx].obj_code = solve_objcode(t_opcode->opcode,n,ia,x,b,p,e,symbol_int);
							fprintf(lstfp,"%06X\n",line_info[k].obj_code);
						}
						else if(symbol_int - base >= 0 && symbol_int - base <=4095) {
							b = 1;
							symbol_int = symbol_int - base;					//그 외에 base 가능 범위에 있으면 base
							t_opcode = optext_search2(line_info[idx].opcode,h_table);
							line_info[idx].obj_code = solve_objcode(t_opcode->opcode,n,ia,x,b,p,e,symbol_int);
							fprintf(lstfp,"%06X\n",line_info[idx].obj_code);
						}
						else {									//둘 다 아니면 에러
							printf("ERROR in range\n");
							error = 1;
							continue;
						}
					}
					else {										//immediate인 경우
						if(str_determinent(t_char1,10) == -1) {					//10진수가 아니면
							printf("ERROR in format 3\n");					//에러 처리
							error = 1;
							continue;
						}						

						imme_int = strtol(t_char1,NULL,10);
						if(imme_int>0x1000) {
							printf("ERROR in immediate\n");
							error = 1;
							continue;
						}

						t_opcode = optext_search2(line_info[idx].opcode,h_table);
						line_info[idx].obj_code = solve_objcode(t_opcode->opcode,n,ia,x,b,p,e,imme_int);
						fprintf(lstfp,"%06X\n",line_info[k].obj_code);
					}

				}

				if(obj_flag == 1) {
					i++;
					obj_info[i].start = line_info[idx].location;
					obj_flag = 0;
				}
				if(obj_info[i].size + 3 <= 30) {
					obj_info[i].size += 3;
				}
				else {
					i++;
					obj_info[i].size += 3;
					obj_info[i].start = line_info[k].location;
					line_info[idx].n_flag = 1;
				}

			}
			else if(line_info[idx].format == 4) {								//format 4인 경우
				e = 1;
				strcpy(t_char2,line_info[idx].operhand);
				strcpy(t_char3,line_info[idx].opcode);
				strcpy(t_char3,t_char3+1);
				if(strstr(t_char2,",")!=NULL) {
					if(strstr(t_char2,"X") != NULL || strstr(t_char2,"x")) {
						x = 1;
						strtok(t_char2,",");
					}
					else {
						printf("ERROR in format 4\n");
						error = 1;
						continue;
					}
				}

				if(hash_remove(t_char2,&n,&ia,s_table) == -1) {
					printf("ERROR in immediate or indirect addressing\n");
					error = 1;
					continue;						
				}

				t_symbol = symbol_search(s_table,t_char2);
				if(t_symbol != NULL) {									//operhand가 symbol인 경우
					t_opcode = optext_search2(t_char3,h_table);
					line_info[idx].obj_code = solve_objcode(t_opcode->opcode,n,ia,x,b,p,e,t_symbol->location);
					fprintf(lstfp,"%06X\n",line_info[k].obj_code);
					line_info[idx].m_flag = 1;
				}
				else {											//immediate인 경우
					if(str_determinent(t_char2,10) == -1) {						//10진수가 아니면
						printf("ERROR in immediate\n");					//에러 처리
						error = 1;
						continue;
					}
					symbol_int = strtol(t_char2,NULL,10);

					if(symbol_int>0x1000) {
						printf("ERROR in format 4\n");
						error = 1;
						continue;
					}

					t_opcode = optext_search2(t_char3,h_table);
					line_info[idx].obj_code = solve_objcode(t_opcode->opcode,n,ia,x,b,p,e,symbol_int);
					fprintf(lstfp,"%06X\n",line_info[idx].obj_code);

				}

				if(obj_flag == 1) {
					i++;
					obj_info[i].start = line_info[idx].location;
					obj_flag = 0;
				}

				if(obj_info[i].size + 4 <= 30) {
					obj_info[i].size += 4;
				}
				else {
					i++;
					obj_info[i].size += 4;
					obj_info[i].start = line_info[idx].location;
					line_info[idx].n_flag = 1;
				}
			}
		}
		if(error == 1) {											//error
			printf("**************** ERROR2 LINE ****************\n");
			printf("line : %d : %s",linenum, copy);
			printf("********************************************\n");
			remove(lstname);
			remove(objname);
			return -1;
		}
	}
	idx++;
	if ( !e_flag){

		return -1;
	}
	for ( int idx = 0; idx < length; ++i ){
		fprintf(lstfp, "%-5d\t", linenum);

	}
	make_objfile();
	printf("\toutput file : [%s], [%s]\n", lstname, objname);

	fclose(lstfp);
	fclose(objfp);
	return 1;
}

void make_objfile(){

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
