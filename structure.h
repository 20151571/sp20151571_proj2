#ifndef __structure__
#define __Structure__

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
    char symbol[30];
    int address;
    symbolPtr next;
}Symbol;


typedef struct{
    const int size; // 37
    Symbol *table[37];
    Hash *hashTable;
}Symbol_table;

typedef struct{
    int location;
    int format; // format 0 : 주석
    int label_flag;
    int n_flag;
    int modi_flag;
    int obj_code;
    char asmd[30];
    char opcode[30];
    char symbol[30];
    char operhand[30];
    char obj_strcode[30];
    char comment[100];
}line_inform;

typedef struct{
    int start;
    int size;
}object_inform;

#endif
