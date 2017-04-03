#include "20151571.h"

int main(void){
    char buffer[256];
    char tmpc;
    const int Max_len = 256; 
    int len = 0;
    Hash hashTable;
    History Hhead;
    Shell_Memory Shmemory;
    Symbol_table Stable;
    symbol_info symbolArr[500];
    sp1_init ( &Hhead, &Shmemory, &hashTable);
    sp2_init (&Stable);
    while(1){
        printf("sicsim> ");
        len = 0;
        while( ( tmpc = getchar() )  != '\n'){
            if ( tmpc == EOF )
                exit(EXIT_FAILURE);

            if( len > Max_len )
                continue;
            buffer[len++] = tmpc;
        }
        
        if(len > Max_len ){
            fprintf(stderr, "Max length is 256\n");
            continue;
        }
        
        buffer[len] = '\0';
        main_process(buffer, &Hhead, &Shmemory, &hashTable,
                &Stable, symbolArr);
    }
    return 0;
}
