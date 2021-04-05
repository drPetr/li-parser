#include <stdio.h>

#include "li.h"

#define listr           liStr_t
#define li_salloc       LiStrAlloc
#define li_srealloc     LiStrRealloc
#define li_sfree        LiStrFree
#define li_screate      LiStrCreate
#define li_screate_cs   LiStrCreateCstr
#define li_sconcat      LiStrConcat
#define li_sconcat_cs   LiStrConcatCstr

int main( int argc, char **argv ) {
    listr *s;
    
    s = li_salloc( 1024 * 1024 * 8 );
    
    li_sconcat_cs( &s, " Hello world" );
   
    printf( "%d", s->len );
    
    return 0;
}


