#ifndef __LISTR_H__
#define __LISTR_H__

#include "litypes.h"

/* li string */
typedef struct liStr_t {
    lisize_t    alloced;    /* alloced size */
    lisize_t    length;     /* length */
    char        string[0];  /* null-terminated string */
} liStr_t;




#define     salc(s)     ((s)->alloced)
#define     slen(s)     ((s)->length)
#define     sstr(s)     ((s)->string)

liStr_t     *LiSAlloc( lisize_t siz );
liStr_t     *LiSRealloc( liStr_t *s, lisize_t siz );
void        LiSFree( liStr_t *s );

liStr_t     *LiSNew( const char *cs );
liStr_t     *LiSNewL( const char *cs, lisize_t len );

liStr_t     *LiSSet( liStr_t *s, const char *cs );
liStr_t     *LiSSetL( liStr_t *s, const char *cs, lisize_t len );

liStr_t     *LiSCat( liStr_t *s, const char *cs );
liStr_t     *LiSCatL( liStr_t *s, const char *cs, lisize_t len );

int         LiSCmp( liStr_t *s, const char *cs );
int         LiSCmpL( liStr_t *s, const char *cs, lisize_t len );


#endif //__LISTR_H__