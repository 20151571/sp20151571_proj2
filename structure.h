#ifndef __structure__
#define __structure__

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
    char code[5];
    char str_opcode[50];
    Hnode next;
}Hash_Node;

typedef struct MEMORY{
    unsigned char memory[1048576];
    int last_address;
    int max_address;
}Shell_Memory;

typedef struct{
    int base_address;
    int location;
    int line;
    int address;
    int program_len;
    char **argu;
}Pass1;

typedef struct __SYMBOL__ *symbolPtr;
typedef struct __SYMBOL__{
    char symbol[50];
    int address;
    symbolPtr next;
}Symbol;
//symbol linked_list

typedef struct{
    const int size; // 37
    Symbol *table[37];
    Hash *hashTable;
}Symbol_table;
//symbol hashtable

typedef struct{
    char symbol[5];
    int address;
}symbol_info;
//symbol information

typedef struct{
    int location;
    int format; // format -1 : 주석
    int label_flag;
    int base_flag;
    int n_flag;
    int modi_flag;
    int obj_code;
    char asmd[50];
    char opcode[50];
    char symbol[50];
    char operhand[50];
    char obj_strcode[50];
    char comment[100];
}line_inform;
//linf information

typedef struct{
    int start;
    int size;
}object_inform;
//object code information

#endif
