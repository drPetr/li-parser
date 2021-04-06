#include <stdio.h>
#include <time.h>

#include "li.h"

#define listr           liStr_t
#define li_salloc       LiStrAlloc
#define li_srealloc     LiStrRealloc
#define li_sfree        LiStrFree
#define li_screate      LiStrCreate
#define li_screate_cs   LiStrCreateCstr
#define li_sconcat      LiStrConcat
#define li_sconcat_cs   LiStrConcatCstr

#define livalarr        liValArray_t
#define lival           liVal_t
#define li_varralloc    LiValArrayAlloc
#define li_varrrealloc  LiValArrayRealloc
#define li_varrfree     LiValArrayFree
#define li_vinit        LiValInit
#define li_vinit_s      LiValInitStr

void PrintArray( liValArray_t *array ) {
    int i;
    for( i = 0; i < array->arrayNumber; i++ ) {
        if( array->array[i].type == LI_TSTR ) {
            //printf( "[%d]: \"%s\"\n", i, array->array[i].vstr->str );
        }
    }
    printf( "max number: %d\n", array->arrayMaxNumber );
    printf( "array number: %d\n", array->arrayNumber );
}

int main( int argc, char **argv ) {
    livalarr *array;
    int i;
    
    array = li_varralloc( 1 );
    
    clock_t start = clock();
    
    for( i = 0; i < 33554432; i++ ) {
        listr *s = li_screate_cs( "Hello world!" );
        
        if( array->arrayMaxNumber == array->arrayNumber ) {
            array = li_varrrealloc( array, array->arrayNumber + 1 );
        }
        
        
        
        li_vinit_s( &array->array[array->arrayNumber++], s );
    }
    
    PrintArray( array );
    
    clock_t stop = clock();
    double d = (double)(stop - start) / CLOCKS_PER_SEC;
    printf( "Loop required %f seconds", d );
    
    return 0;
}


