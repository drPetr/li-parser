#include "listr.h"
#include "liassert.h"
#include "limem.h"
#include "liutil.h"

#include <string.h>



/*
============
LiSAlloc
============
*/
liStr_t *LiSAlloc( lisize_t siz ) {
    liassert(siz >= 1);
    
    liStr_t *s = (liStr_t*)LiAlloc( sizeof(liStr_t) + siz, LI_TYID_STR );
    slen(s) = 0;
    salc(s) = siz;
    snref(s) = 0;
    if( siz ) {
        sstr(s)[0] = 0;
    }
    
    return s;
}

/*
============
LiSRealloc
============
*/
liStr_t *LiSRealloc( liStr_t *s, lisize_t siz ) {
    liassert(s);
    liassert(siz >= 1);
    
    siz = CeilPow2( siz );
    if( salc(s) == siz ) {
        return s;
    }
    s = (liStr_t*)LiRealloc( s, sizeof(liStr_t) + siz, LI_TYID_STR );
    salc(s) = siz;
    if( slen(s) >= siz ) {
        slen(s) = siz - 1;
        sstr(s)[ slen(s) ] = 0;
    }
    
    return s;
}

/*
============
LiSFree
============
*/
void LiSFree( liStr_t *s ) {
    liassert(s);
    if( snref(s) ) {
        snref(s)--;
        return;
    }
    LiDealloc( s );
}

/*
============
LiSNew
============
*/
liStr_t *LiSNew( const char *cs ) {
    liassert(cs);
    return LiSNewL( cs, (lisize_t)strlen(cs) );
}

/*
============
LiSNewL
============
*/
liStr_t *LiSNewL( const char *cs, lisize_t len ) {
    liassert(cs);
    liStr_t *s = LiSAlloc( len + 1 );
    if( len ) {
        slen(s) = len;
        memcpy( sstr(s), cs, len );
        sstr(s)[ len ] = 0;
    }
    
    return s;
}

/*
============
LiSRef
============
*/
liStr_t *LiSRef( liStr_t *s ) {
    liassert(s);
    snref(s)++;
    return s;
}

/*
============
LiSSet
============
*/
liStr_t *LiSSet( liStr_t *s, const char *cs ) {
    liassert(s);
    liassert(cs);
    return LiSSetL( s, cs, (lisize_t)strlen(cs) );
}

/*
============
LiSSetL
============
*/
liStr_t *LiSSetL( liStr_t *s, const char *cs, lisize_t len ) {
    liassert(cs);
    
    if( !s ) {
        return LiSNewL( cs, len );
    }
    s = LiSRealloc( s, len + 1 );
    /* copy string */
    memcpy( sstr(s), cs, len );
    slen(s) = len;
    sstr(s)[ len ] = 0;
    return s;
}

/*
============
LiSCat
============
*/
liStr_t *LiSCat( liStr_t *s, const char *cs ) {
    liassert(s);
    liassert(cs);
    return LiSCatL( s, cs, (lisize_t)strlen(cs) );
}

/*
============
LiSCatL
============
*/
liStr_t *LiSCatL( liStr_t *s, const char *cs, lisize_t len ) {
    liassert(s);
    liassert(cs);
    
    if( !len ) {
        return s;
    }
    
    /* check if additional memory is needed */
    if( slen(s) + len > salc(s) ) {
        s = LiSRealloc( s, slen(s) + len );
    }
    
    /* copy substring */
    memcpy( sstr(s) + slen(s), cs, len );
    slen(s) += len;
    sstr(s)[ slen(s) ] = 0;
    return s;
}

/*
============
LiSCmp
============
*/
libool_t LiSCmp( liStr_t *s, const char *cs ) {
    liassert(s);
    liassert(cs);
    return (strncmp( sstr(s), cs, slen(s) ) == 0) && (cs[slen(s)] == 0);
}

/*
============
LiSCmpL
============
*/
libool_t LiSCmpL( liStr_t *s, const char *cs, lisize_t len ) {
    liassert(s);
    liassert(cs);
    return (slen(s) == len) && (strncmp( sstr(s), cs, len ) == 0 );
}
