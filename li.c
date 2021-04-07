#include "li.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>



#define LI_ID_MAX_LEN       127
#define LI_STR_MAX_LEN      4095
#define PTRSIZE             (sizeof(void*))

#define liunused(a)         ((void)a)



/*
============
__impl_liverifya
============
*/
static void __impl_liverifya( int e, const char* expr, const char* file, 
        int line, const char *function, const char* fmt, ... ) {
    va_list args;
    if( !e ){
        va_start( args, fmt );
        fprintf( stderr, "verification failed: %s\n", expr );
        fprintf( stderr, "file: %s\n", file );
        fprintf( stderr, "line: %d\n", line );
        fprintf( stderr, "function: %s\n", function );
        if( fmt ) {
            vfprintf( stderr, fmt, args );
        }
        va_end( args );
        exit(1);
    }
}

#define liverify(e)                                 \
    __impl_liverifya( (int)(!!(e)), #e, __FILE__,   \
    __LINE__, __FUNCTION__, NULL )

#define liverifya(e,a,...)                          \
    __impl_liverifya( (int)(!!(e)), #e, __FILE__,   \
    __LINE__, __FUNCTION__, a, ##__VA_ARGS__ )



#if defined(LI_NODBG) || (!defined(DEBUG))
    #define onlydebug(act)
    #define liassert(e)
    #define liasserta(e,a,...)
#else

/*
============
__impl_liasserta
============
*/
static void __impl_liasserta( int e, const char* expr, const char* file, 
        int line, const char *function, const char* fmt, ... ) {
    va_list args;
    if( !e ){
        va_start( args, fmt );
        fprintf( stderr, "assertion failed: %s\n", expr );
        fprintf( stderr, "file: %s\n", file );
        fprintf( stderr, "line: %d\n", line );
        fprintf( stderr, "function: %s\n", function );
        if( fmt ) {
            vfprintf( stderr, fmt, args );
        }
        va_end( args );
        exit(1);
    }
}

    #define onlydebug(act)  act

    #define liassert(e)                                 \
        __impl_liasserta( (int)(!!(e)), #e, __FILE__,   \
        __LINE__, __FUNCTION__, NULL )
        
    #define liasserta(e,a,...)                          \
        __impl_liasserta( (int)(!!(e)), #e, __FILE__,   \
        __LINE__, __FUNCTION__, a, ##__VA_ARGS__ )

#endif

libool_t __charis( char c, uint8_t flag ) {
    uint8_t i = *((uint8_t*)(void*)(&c));
    static const char flags[256] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03,
        0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    return !!(flags[i] & flag);
}

#define is_firstkeych(c)    __charis( c, 0x02 )
#define is_nextkeych(c)     __charis( c, 0x01 )


/*
============
CeilPow2
============
*/
uint32_t CeilPow2( uint32_t v ) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

/*
============
UInt64ToStr
============
*/
size_t UInt64ToStr( uint64_t val, char *str, int base ) {
    static const char alpha[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char *p = str;
    uint8_t digits[64]; /* value digits */
    int dig = 0;        /* amount of digits */
    
    liassert( base >= 2 );
    liassert( base <= 36 );
    
    /* calculate digits */
    do {
        digits[dig++] = (uint8_t)(val % base);
        val /= base;
    } while( val );
    
    /* copy the flipped digits to the output buffer */
    do {
        *p++ = alpha[ digits[--dig] ];
    } while( dig > 0 );
    *p = 0;
    
    return (size_t)(p - str);
}

/*
============
Int64ToStr
============
*/
size_t Int64ToStr( int64_t val, char *str, int base ) {
    liassert( base >= 2 );
    liassert( base <= 36 );
    
    /* check for negative value */
    if( val < 0 ) {
        *str++ = '-';
        return UInt64ToStr( (uint64_t)(-val), str, base ) + 1;
    }
    return UInt64ToStr( (uint64_t)val, str, base );
}



/*
================================================
                   allocator
================================================
*/
int allocCalls = 0;
int allocTyidStrCalls = 0;
int allocTyidNodeCalls = 0;
int deallocCalls = 0;
int reallocCalls = 0;
/*
============
LiDefaultAlloc

Default function for memory allocation. The function
allocates memory of size "size" for the type under
the identifier "type". The function returns a pointer
to a block of memory. Standard malloc is used for
allocation.

return values:
pointer to memory block
============
*/
static void *LiDefaultAlloc( size_t size, lityid_t type ) {
    //liunused( type );    
    allocCalls++;
    if( type == LI_TYID_STR ) {
        allocTyidStrCalls++;
    } else if( type == LI_TYID_NODE ) {
        allocTyidNodeCalls++;
    } else if( type == LI_TYID_ARR ) {
        
    } else {
        liverifya( 0, "error: unknown typeid [%d]", type );
    }
    return malloc( size );
}

/*
============
LiDefaultRealloc

Default function for memory reallocation.

return values:
pointer to new memory block
============
*/
static void *LiDefaultRealloc( void *ptr, size_t size, lityid_t type ) {
    liunused( type );
    reallocCalls++;
    return realloc( ptr, size );
}

/*
============
LiDefaultFree

Removes a block of memory that was previously
allocated. The standard free function is used
============
*/
static void LiDefaultFree( void* ptr ) {
    deallocCalls++;
    free( ptr );
}

static liAlloc_t liDefaultAllocator = { 
    LiDefaultAlloc,
    LiDefaultRealloc,
    LiDefaultFree
};

static liAlloc_t *liAllocator = &liDefaultAllocator;

/*
============
LiAlloc
============
*/
static void *LiAlloc( size_t size, lityid_t type ) {
    liassert( liAllocator );
    liassert( liAllocator->alloc );
    liassert( size != 0 );
    
    return liAllocator->alloc( size, type );
}

/*
============
LiRealloc
============
*/
static void *LiRealloc( void *ptr, size_t size, lityid_t type ) {
    liassert( liAllocator );
    liassert( liAllocator->realloc );
    liassert( size != 0 );
    liassert( ptr );
    
    return liAllocator->realloc( ptr, size, type );
}

/*
============
LiDealloc
============
*/
static void LiDealloc( void *ptr ) {
    liassert( liAllocator );
    liassert( liAllocator->free );
    liassert( ptr );
    
    liAllocator->free( ptr );
}

/*
============
LiSetAllocator

set allocator interface
============
*/
void LiSetAllocator( liAlloc_t *alloc ) {
    liassert( alloc );
    liassert( alloc->alloc );
    liassert( alloc->free );
    
    liAllocator = alloc;
}

/*
============
LiGetAllocator
============
*/
liAlloc_t *LiGetAllocator( void ) {
    return liAllocator;
}



/*
================================================
                li input/output
================================================
*/

/*
============
LiDefaultOpen
============
*/
static liFile_t LiDefaultOpen( const char* fileName, char openMode ) {
    liverify( openMode == 'r' || openMode == 'w' ); /* read/write mode */
    liassert( fileName );
    liassert( fileName[0] );

    switch( openMode ) {
        case 'r':
            return (liFile_t)fopen( fileName, "rb" );
        case 'w':
            return (liFile_t)fopen( fileName, "wb" );
    }
    
    return NULL;
}

/*
============
LiDefaultClose
============
*/
static void LiDefaultClose( liFile_t file ) {
    liassert( file );
    
    fclose( (FILE*)file );
}

/*
============
LiDefaultRead
============
*/
static ssize_t LiDefaultRead( void* dst, size_t size, liFile_t file ) {
    liassert( dst );
    liassert( file );
    
    size_t outSize;
    
    outSize = fread( dst, 1, size, (FILE*)file );
    if( outSize != size && !feof( (FILE*)file ) ) {
        return -(ssize_t)outSize;
    }
    
    return (ssize_t)outSize;
}

/*
============
LiDefaultWrite
============
*/
static ssize_t LiDefaultWrite( const void* src, size_t size, liFile_t file ) {
    liassert( src );
    liassert( file );
    
    size_t outSize;
    
    outSize = fwrite( src, 1, size, (FILE*)file );
    if( outSize != size && !feof( (FILE*)file ) ) {
        return -(ssize_t)outSize;
    }
    
    return (ssize_t)outSize;
}

static liIO_t liDefaultIO = {
    LiDefaultOpen,
    LiDefaultClose,
    LiDefaultRead,
    LiDefaultWrite
};



/*
================================================
                    li string
================================================
*/

/*
============
LiStrAlloc
============
*/
liStr_t *LiStrAlloc( uint32_t siz ) {
    liStr_t *s;
    
    liassert( siz >= 1 );
    
    /* siz = CeilPow2( siz ); */
    s = LiAlloc( sizeof(liStr_t) + siz, LI_TYID_STR );
    s->len = 0;
    s->alloced = siz;
    if( siz ) {
        s->str[0] = 0;
    }
    
    return s;
}

/*
============
LiStrRealloc
============
*/
liStr_t *LiStrRealloc( liStr_t *s, uint32_t siz ) {
    liassert( s );
    liassert( siz >= 1 );
    
    siz = CeilPow2( siz );
    if( s->alloced == siz ) {
        return s;
    }
    s = LiRealloc( s, sizeof(liStr_t) + siz, LI_TYID_STR );
    s->alloced = siz;
    if( s->len >= siz ) {
        s->len = siz - 1;
        s->str[ s->len ] = 0;
    }
    
    return s;
}

/*
============
LiStrFree
============
*/
void LiStrFree( liStr_t *s ) {
    liassert( s );
    LiDealloc( s );
}

/*
============
LiStrCreate
============
*/
liStr_t *LiStrCreate( const char *str, uint32_t len ) {
    liStr_t *s;
    
    liassert( str );
    
    s = LiStrAlloc( len + 1 );
    
    if( len ) {
        s->len = len;
        memcpy( s->str, str, len );
        s->str[len] = 0;
    }
    
    return s;
}

/*
============
LiStrCreateCstr
============
*/
liStr_t *LiStrCreateCstr( const char *cstr ) {
    return LiStrCreate( cstr, (uint32_t)strlen(cstr) );
}

/*
============
LiStrConcat
============
*/
void LiStrConcat( liStr_t **s, const char *cstr, uint32_t len ) {
    liStr_t *str;
    
    liassert( cstr );
    liassert( s );
    liassert( *s );
    
    if( !len ) {
        return;
    }
    
    /* check if additional memory is needed */
    str = *s;
    if( str->len + len > str->alloced ) {
        str = LiStrRealloc( str, str->len + len + 1 );
        *s = str;
    }
    
    /* copy substring */
    memcpy( &(str->str[str->len]), cstr, len );
    str->len += len;
    str->str[str->len] = 0;
}

/*
============
LiStrConcatCstr
============
*/
void LiStrConcatCstr( liStr_t **s, const char *cstr ) {
    liassert( cstr );
    liassert( s );
    liassert( *s );
    
    LiStrConcat( s, cstr, (uint32_t)strlen(cstr) );
}

/*
============
LiStrSet
============
*/
void LiStrSet( liStr_t **s, const char *cstr, uint32_t len ) {
    liassert( cstr );
    liassert( s );
    
    if( *s == NULL ) {
        *s = LiStrAlloc( len );
    } else {
        *s = LiStrRealloc( *s, len );
    }
    /* copy string */
    memcpy( (*s)->str, cstr, len );
    (*s)->len = len;
    (*s)->str[len] = 0;
}

/*
============
LiStrSetCstr
============
*/
void LiStrSetCstr( liStr_t **s, const char *cstr ) {
    liassert( cstr );
    liassert( s );
    
    LiStrSet( s, cstr, (uint32_t)strlen(cstr) );
}



/*
================================================
                    li value
================================================
*/
    
/*
============
LiArrayAlloc
============
*/
liArray_t *LiArrayAlloc( uint32_t siz, uint32_t num ) {
    liArray_t *array;
    
    liassert( siz >= 1 );
    liassert( num >= 1 );
    
    uint32_t memSize = sizeof(liArray_t) + siz * num;
    array = (liArray_t*)LiAlloc( memSize, LI_TYID_ARR );
    array->allocedNum = num;
    array->siz = siz;
    array->num = 0;
    
    return array;
}

/*
============
LiArrayRealloc
============
*/
liArray_t *LiArrayRealloc( liArray_t *array, uint32_t num ) {
    liassert( array );
    liassert( num >= 1 );
    
    num = CeilPow2( num );
    /* need to re-allocate memory? */
    if( num == array->allocedNum ) {
        /* no need to reallocate memory */
        return array;
    }
    
    /* reallocate */
    uint32_t memSize = sizeof(liArray_t) + array->siz * num;
    array = (liArray_t*)LiRealloc( array, memSize, LI_TYID_ARR );
    array->allocedNum = num;
    if( array->allocedNum > array->num ) {
        array->num = array->allocedNum;
    }
    
    return array;
}

/*
============
LiArrayFree
============
*/
void LiArrayFree( liArray_t *array ) {
    liassert( array );
    LiDealloc( array );
}

/*
============
LiArrayEmpty
============
*/
void LiArrayEmpty( liArray_t *array ) {
    liassert( array );
    array->num = 0;
}

/*
============
LiArrayAppend
============
*/
liArray_t *LiArrayAppend( liArray_t *array, void *data ) {
    liassert( array );
    liassert( data );
    
    /* check the size of available memory */
    if( array->num + 1 > array->allocedNum ) {
        array = LiArrayRealloc( array, array->num + 1 );
    }
    
    /* copy data to array */
    memcpy( array->array + (array->num * array->siz), data, array->siz );
    array->num += 1;
    return array;
}



/*
================================================
        li object insert/extract functions
================================================
*/

/*
============
LiInsertionHelper
============
*/
static void LiInsertionHelper( liObj_t *left, liObj_t *right, 
        liObj_t *parent, liObj_t *insert ) {
    /* first node in the insert sequence */
    liObj_t *firstInsert = insert;
    /* last node in the insert sequence */
    liObj_t *lastInsert = insert;
    
    liassert( left || right || parent );
    liassert( insert );

#if !defined(LI_NODBG) && defined(DEBUG)
    liassert( insert != parent );
    liassert( insert != left );
    liassert( insert != right );
    liassert( insert->parent == NULL );
    if( left && right ) {
        liassert( left->parent == right->parent );
        liassert( left->next == right );
        liassert( right->prev == left );
        liassert( left != right );
    }
    if( parent ) {
        int chk;
        liObj_t *it;
        
        liassert( parent != left );
        liassert( parent != right );
        
        /* check left */
        if( left ) {
            liassert( left->parent == parent );
            chk = 0;
            it = parent->firstChild;
            while( it ) {
                if( it == left ) {
                    chk = 1;
                    break;
                }
                it = it->next;
            }
            liasserta( chk, "error: left." );
        }
        
        /* check right */
        if( right ) {
            liassert( right->parent == parent );
            chk = 0;
            it = parent->firstChild;
            while( it ) {
                if( it == right ) {
                    chk = 1;
                    break;
                }
                it = it->next;
            }
            liasserta( chk, "error: right." );
        }
    }
#endif

    /* set the whole sibling sequence of the parent and get a
    pointer to the last node of the sibling sequence */
    insert->parent = parent;
    while( lastInsert->next ) {
        lastInsert = lastInsert->next;
        lastInsert->parent = parent;
    }
    
    while( firstInsert->prev ) {
        firstInsert = firstInsert->prev;
        firstInsert->parent = parent;
    }

    /* restoring "family" relations */
    if( parent ) {
        if( left ) {
			left->next = firstInsert;
			firstInsert->prev = left;
		} else {
			parent->firstChild = firstInsert;
		}
		if( right ) {
			right->prev = lastInsert;
			lastInsert->next = right;
		} else {
			parent->lastChild = lastInsert;
		}
    } else {
		if( left ) {
			left->next = firstInsert;
			firstInsert->prev = left;
        }
		if( right ) {
			right->prev = lastInsert;
			lastInsert->next = right;
		}
    }
}

/*
============
LiInsertFirstChild

Insert an insert node as the first child of a node
============
*/
void LiInsertFirstChild( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type == LI_VTOBJ );
    liassert( insert->type != LI_VTUNDEF );
#if !defined(LI_NODBG) && defined(DEBUG)
    liObj_t *o = insert;
    while( o->prev ) {
        o = o->prev;
    }
    liassert( o->key != NULL );
#endif
    LiInsertionHelper( NULL, node->firstChild, node, insert );
}

/*
============
LiInsertLastChild

Insert the insert node as the last child of the node
============
*/
void LiInsertLastChild( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type == LI_VTOBJ );
    liassert( insert->type != LI_VTUNDEF );
#if !defined(LI_NODBG) && defined(DEBUG)
    liObj_t *o = insert;
    while( o->prev ) {
        o = o->prev;
    }
    if( node->lastChild == NULL ) {
        liassert( o->key != NULL );
    }
#endif
    LiInsertionHelper( node->lastChild, NULL, node, insert );
}

/*
============
LiInsertBefore

Insert an insert node before the node
============
*/
void LiInsertBefore( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type != LI_VTUNDEF );
    liassert( insert->type != LI_VTUNDEF );
    LiInsertionHelper( node->prev, node, node->parent, insert );
}

/*
============
LiInsertAfter

Insert an insert node after a node
============
*/
void LiInsertAfter( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type != LI_VTUNDEF );
    liassert( insert->type != LI_VTUNDEF );
    LiInsertionHelper( node, node->next, node->parent, insert );
}

/*
============
LiInsertFirst

Insert the insert node first in the sequence node
============
*/
void LiInsertFirst( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type != LI_VTUNDEF );
    liassert( insert->type != LI_VTUNDEF );
    
    if( node->parent ) {
        node = node->parent->firstChild;
    } else {
        while( node->prev ) {
            node = node->prev;
        }
    }
    LiInsertionHelper( NULL, node, node->parent, insert );
}

/*
============
LiInsertLast

Insert the insert node as the last node in the sequence
============
*/
void LiInsertLast( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type != LI_VTUNDEF );
    liassert( insert->type != LI_VTUNDEF );
    
    if( node->parent ) {
        node = node->parent->lastChild;
    } else {
        while( node->next ) {
            node = node->next;
        }
    }
    LiInsertionHelper( node, NULL, node->parent, insert );
}

/*
============
LiExtractionHelper
============
*/
static liObj_t *LiExtractionHelper( liObj_t *left, liObj_t *right ) {
    liObj_t *ext = NULL;
    liObj_t *it;
    liObj_t *parent;
    liObj_t *before;
    liObj_t *after;
    
    /* should  be at least one node */
    liassert( left || right );
    
    /* if left node is set and right node is set */
    if( left && right ) {
        ext = left;
#if !defined(LI_NODBG) && defined(DEBUG)
        /* check left and right sequence */
        if( ext != right ) {
            while( ext && ext != right ) {
                ext = ext->next;
            }
        }
        liassert( ext == right );
        ext = left;
#endif
    } else if( left || right ) {
        /* if only one node is set */
        /* if only one left node */
        if( left ) {
            ext = left;
            right = left;
            while( right->next ) {
                right = right->next;
            }
        } else { 
            /* else right node */
            left = right;
            while( left->prev ) {
                left = left->prev;
            }
            ext = left;
        }
    }
    
    parent = left->parent;
    before = left->prev;
    after = right->next;
    
    /* if the left node has a previous sibling and the
    right node has a next sibling */
    if( before && after ) {
        before->next = after;
        after->prev = before;
        left->prev = NULL;
        right->next = NULL;
    } else if( before || after ) {
        /* else only one node has a sibling if the left
        node has a previous sibling and */
        if( before ) {
            before->next = NULL;
            left->prev = NULL;
        } else {
            /* else the right node has a next sibling */
            after->prev = NULL;
            right->next = NULL;
        }
    }
    
    /* disconnect parents */
    if( parent ) {
        if( left == parent->firstChild && right == parent->lastChild ) {
            /* no descendants */
            parent->firstChild = NULL;
            parent->lastChild = NULL;
        } else if( left == parent->firstChild || right == parent->lastChild ){
            /* set new pointers to descendants */
            if( left == parent->firstChild ) {
                parent->firstChild = after;
            } else {
                parent->lastChild = before;
            }
        }
        
        it = left;
        do {
            it->parent = NULL;
            it = it->next;
        } while( it != right );
    }
    
    return ext;
}

/*
============
LiExtract
============
*/
liObj_t *LiExtract( liObj_t *node ) {
    liassert( node );
    return LiExtractionHelper( node, node );
}

/*
============
LiExtractSiblings
============
*/
liObj_t *LiExtractSiblings( liObj_t *left, liObj_t *right ) {
    liassert( left || right );
    return LiExtractionHelper( left, right );
}

/*
============
LiExtractChildren
============
*/
liObj_t *LiExtractChildren( liObj_t *node ) {
    liassert( node );
    if( node->firstChild ) {
        return LiExtractionHelper( node->firstChild, node->lastChild );
    }
    return NULL;
}

/*
============
LiNodeCreate
============
*/
liObj_t *LiNodeCreate( void ) {
    liObj_t *node;
    
    node = (liObj_t*)LiAlloc( sizeof(liObj_t), LI_TYID_NODE );
    
    node->parent = NULL;
    node->next = NULL;
    node->prev = NULL;
    node->firstChild = NULL;
    node->lastChild = NULL;
    node->ptr = NULL;
    node->key = NULL;
    node->type = LI_VTUNDEF;
    node->flags = 0;
    
    return node;
}

/*
============
LiNodeFreeSubtreeHelper_r
============
*/
static void LiNodeFreeSubtreeHelper_r( liObj_t *node, int level ) {
    liObj_t *next;

    liassert( node );
    liverifya( level <= LI_MAX_NESTING_LEVEL,
        "error: the nesting level is too high. "
        "check the tree for looping levels or increase "
        "the constant LI_MAX_NESTING_LEVEL. "
        "LI_MAX_NESTING_LEVEL=%d", LI_MAX_NESTING_LEVEL );

    do {
        next = node->next;
        if( node->firstChild ) {
            /* free subtree */
            LiNodeFreeSubtreeHelper_r( node->firstChild, level + 1 );
        }
        
        /* free key */
        if( node->key ) {
            LiStrFree( node->key );
        }
        
        /* free object value */
        switch( node->type ) {
            case LI_VTUNDEF:
                break;
            case LI_VTNULL:
                break;
            case LI_VTOBJ:
                /* removal is done below */
                break;
                
            case LI_VTSTR:
                if( node->vstr ) {
                    LiStrFree( node->vstr );
                }
                break;
                
            case LI_VTINT:
                break;
            case LI_VTUINT:
                break;
            case LI_VTBOOL:
                break;
                
            default:
                liverifya( 0, "error: unknown object type [%d]", 
                        node->type );
                break;
        }
        
        LiDealloc( node );
        node = next;
    } while( node );
}

/*
============
LiFreeSubtree
============
*/
void LiFreeSubtree( liObj_t *node ) {
    liassert( node );
    node = LiExtract( node );
    liassert( node );
    LiNodeFreeSubtreeHelper_r( node, 0 );
}

/*
============
LiFree
============
*/
void LiFree( liObj_t *li ) {
    liassert( li );
    
    while( li->parent ) {
        li = li->parent;
    }
    LiNodeFreeSubtreeHelper_r( li, 0 );
}



/*
================================================
                    li object
================================================
*/

/*
============
LiIsCorrectKey
============
*/
libool_t LiIsCorrectKey( const char *s, uint32_t len ) {
    if( len ) {
        if( !is_firstkeych(*s) ) {
            return lifalse;
        }
        len--;
        s++;
        while( len-- ) {
            if( !is_nextkeych(*s++) ) {
                return lifalse;
            }
        }
    }
    return litrue;
}

/*
============
LiIsCorrectRefStr

.key.key2...
============
*/
libool_t LiIsCorrectRefStr( const char *s ) {
    liassert( s );
    
    if( *s == '.' ) {
        s++;
    }
    if( *s ) {
        while( is_firstkeych(*s) ) {
            s++;
            for( ; is_nextkeych(*s); s++ );
            if( *s == '.' && s[1] != 0 ) {
                s++;
            }
        }
        return *s == 0;
    }
    
    return lifalse;
}

/*
============
LiSetKeyStr
============
*/
void LiSetKeyStr( liObj_t *o, const char *key, uint32_t len ) {
    liassert( o );
    if( !key ) {
        if( o->key ) {
            LiStrFree( o->key );
            o->key = NULL;
        }
    } else {
        liassert( LiIsCorrectKey( key, len ) );
        if( o->key ) {
            LiStrSet( &(o->key), key, len );
        } else {
            o->key = LiStrCreate( key, len );
        }
    }
}

/*
============
LiSetKeyCstr
============
*/
void LiSetKeyCstr( liObj_t *o, const char *key ) {
    liassert( o );
    LiSetKeyStr( o, key, key ? (uint32_t)strlen(key) : 0 );
}

/*
============
LiSetFlags
============
*/
void LiSetFlags( liObj_t *o, liflag_t flags ) {
    liassert( o );
    o->flags = flags;
}

/*
============
LiTypeIs
============
*/
libool_t LiTypeIs( liObj_t *o, litype_t type ) {
    liassert( o );
    return o->type == type;
}

/*
============
LiIsObj
============
*/
libool_t LiIsObj( liObj_t *o ) {
    liassert( o );
    return o->type == LI_VTOBJ;
}

/*
============
LiObj
============
*/
liObj_t *LiObj( void ) {
    liObj_t *o = LiNodeCreate();
    o->type = LI_VTOBJ;
    return o;
}

/*
============
LiNull
============
*/
liObj_t *LiNull( void ) {
    liObj_t *o = LiNodeCreate();
    o->type = LI_VTNULL;
    return o;
}

/*
============
LiStr
============
*/
liObj_t *LiStr( const char *s, uint32_t len ) {
    liassert( s );
    liObj_t *o = LiNodeCreate();
    o->type = LI_VTSTR;
    o->vstr = s ? LiStrCreate( s, len ) : NULL;
    return o;
}

/*
============
LiCstr
============
*/
liObj_t *LiCstr( const char *s ) {
    return LiStr( s, s ? (uint32_t)strlen(s) : 0 );
}

/*
============
LiInt
============
*/
liObj_t *LiInt( int64_t i ) {
    liObj_t *o = LiNodeCreate();
    o->type = LI_VTINT;
    o->vint = i;
    return o;
}

/*
============
LiUint
============
*/
liObj_t *LiUint( uint64_t u ) {
    liObj_t *o = LiNodeCreate();
    o->type = LI_VTUINT;
    o->vuint = u;
    return o;
}

/*
============
LiBool
============
*/
liObj_t *LiBool( libool_t b ) {
    liObj_t *o = LiNodeCreate();
    o->type = LI_VTBOOL;
    o->vint = b;
    return o;
}



struct parsedPatternData_s{
    const char      *str;
    uint32_t        len;
};

/*
============
LiFindFirst
============
*/
licode_t LiFindFirst( liFindData_t *dat, liObj_t *o, const char *s ) {
    liassert( dat );
    liassert( o );
    liassert( s );
    
    /* init liFindData_t structure */
    dat->startObj = o;
    dat->global = lifalse;
    dat->parsedPattern = NULL;
    dat->index = 0;
    
    if( !LiIsCorrectRefStr(s) ) {
        return LI_EINPDAT;
    }
    
    /* check for global pattern */
    if( *s == '.' ) {
        s++;
        dat->global = litrue;
        
        /* go to root */
        while( o->parent ) {
            o = o->parent;
        }
        while( o->prev ) {
            o = o->prev;
        }
        dat->startObj = o;
    }
    
    /* parse pattern */
    if( *s ) {
        /* init parsedPattern array */
        dat->parsedPattern = LiArrayAlloc(
                sizeof(struct parsedPatternData_s), 16 );
        struct parsedPatternData_s patternData;
        const char *startSubstr;
        while( is_firstkeych(*s) ) {
            patternData.str = startSubstr = s;
            s++;
            for( ; is_nextkeych(*s); s++ );
            patternData.len = (uint32_t)(s - startSubstr);
            dat->parsedPattern = LiArrayAppend( dat->parsedPattern,
                    &patternData );
            if( *s == '.' && s[1] != 0 ) {
                s++;
            }
        }
        if( *s != 0 ) {
            return LI_EINPDAT;
        }
    } else {
        return LI_EINPDAT;
    }
    
    
    /* find first object */
    /*struct parsedPatternData_s *pattern;
    uint32_t maxIndex = dat->parsedPattern->num - 1;
    while( o ) {
        pattern = ((struct parsedPatternData_s*)
                dat->parsedPattern->array) + dat->index;
        liStr_t *key = o->key;
        if( key && (key->len == pattern->len) &&
                (0 == strncmp( key->str, pattern->str, key->len ) ) ) {
            if( dat->index == maxIndex ) {
            
                dat->obj = o;
                dat->startObj = o;
                return LI_OK;
            } else if( o->firstChild && (dat->index < maxIndex) ) {
                o = o->firstChild;
                dat->index++;
                continue;
            }
        }
        if( o->next ) {
            o = o->next;
        } else if( o->parent && (dat->index > 0) ) {
            o = o->parent;
            while( o->parent && !(o->parent->next) && (dat->index > 0) ) {
                dat->index--;
                o = o->parent;
            }
            if( o->next ) {
                o = o->next;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    
    dat->startObj = NULL;
    return LI_FINISHED;*/
    
    struct parsedPatternData_s *pattern = 
            (struct parsedPatternData_s*)dat->parsedPattern->array;
    if( (dat->parsedPattern->num == 1) && o->key &&
            (o->key->len == pattern->len) &&
            (0 == strncmp( o->key->str, pattern->str, o->key->len )) ) {
        dat->obj = o;
        return LI_OK;
    }
    
    return LiFindNext( dat );
}

licode_t LiFindNext( liFindData_t *dat ) {
    liassert( dat );
    
    if( !dat->startObj ) {
        return LI_FINISHED;
    }
    
    return LI_FINISHED;
    return LI_OK;
}

licode_t LiFindClose( liFindData_t *dat ) {
    liassert( dat );
    return LI_FINISHED;
}


/*
================================================
                    li writer
================================================
*/

/*
============
LiWriteIndent
============
*/
static licode_t LiWriteIndent( liFile_t f, fnLiWrite wr, int indent ) {
    int i;
    for( i = 0; i < indent; i++ ) {
        ssize_t ret = wr( "    ", 4, f );
        if( ret < 0 ) {
            return LI_EWRITE;
        }
    }
    return LI_OK;
}

/*
============
LiWriteStr
============
*/
static licode_t LiWriteStr( liFile_t f, fnLiWrite wr,
        const char *s, uint32_t len ) {
    liassert( s );
    ssize_t ret = wr( s, len, f );
    if( ret < 0 ) {
        return LI_EWRITE;
    }
    return LI_OK;
}

/*
============
LiWriteCstr
============
*/
static licode_t LiWriteCstr( liFile_t f, fnLiWrite wr, const char *s ) {
    liassert( s );
    return LiWriteStr( f, wr, s, (uint32_t)strlen(s) );
}

/*
============
LiWriteLiStr
============
*/
static licode_t LiWriteLiStr( liFile_t f, fnLiWrite wr, liStr_t *s ) {
    liassert( s );
    return LiWriteStr( f, wr, s->str, s->len );
}

/*
============
LiWriteInt
============
*/
static licode_t LiWriteInt( liFile_t f, fnLiWrite wr, liObj_t *o ) {
    char buf[256];
    char *p = buf;
    
    liassert( o->firstChild == NULL );
    liassert( o->type == LI_VTINT );
    
    if( o->flags & LI_FSIGN ) {
        if( o->vint >= 0 ) {
            *p++ = '+';
        }
    }

    Int64ToStr( o->vint, p, 10 );
    return LiWriteCstr( f, wr, buf );
}

/*
============
LiWriteUint
============
*/
static licode_t LiWriteUint( liFile_t f, fnLiWrite wr, liObj_t *o ) {
    int baseType = o->flags & LI_FBASE_MASK;
    int base = 10;
    char buf[256];
    char *p = buf;
    
    liassert( o->firstChild == NULL );
    liassert( o->type == LI_VTUINT );
    
    switch( baseType ) {
        case LI_FOCT:
            *p++ = '0';
            base = 8;
            break;
            
        case LI_FBIN:
            *p++ = '0';
            *p++ = 'b';
            base = 2;
            break;
            
        case LI_FHEX:
            *p++ = '0';
            *p++ = 'x';
            base = 16;
            break;
        default:
            /* LI_FDEC */
            base = 10;
            break;
    }

    UInt64ToStr( o->vuint, p, base );
    return LiWriteCstr( f, wr, buf );
}

/*
============
LiWriteHelper_r
============
*/
static licode_t LiWriteHelper_r( liFile_t f, fnLiWrite wr, 
        liObj_t *o, liflag_t flags, int level ) {
    licode_t code = LI_OK;
    int nl = 1;
    
    liverifya( level <= LI_MAX_NESTING_LEVEL,
        "error: the nesting level is too high. "
        "check the tree for looping levels or increase "
        "the constant LI_MAX_NESTING_LEVEL. "
        "LI_MAX_NESTING_LEVEL=%d", LI_MAX_NESTING_LEVEL );
    
    do {
        if( nl ) {
            LiWriteIndent( f, wr, level );
        }
        
        nl = 1;
        
        /* write key */
        if( o->key ) {
            LiWriteLiStr( f, wr, o->key );
            LiWriteCstr( f, wr, " = " );
        } else {
            LiWriteCstr( f, wr, ", " );
        }
        if( o->next && (o->next->key == NULL) ) {
            nl = 0;
        }
        
        switch( o->type ) {
            case LI_VTNULL:
                liassert( o->firstChild == NULL );
                LiWriteCstr( f, wr, "null" );
                break;
                
            case LI_VTOBJ:
                if( o->firstChild ) {
                    /* begin of object */
                    LiWriteCstr( f, wr, "{\n" );
                    code = LiWriteHelper_r( f, wr, o->firstChild, 
                            flags, level + 1 );
                    if( code != LI_OK ) {
                        return code;
                    }
                    LiWriteIndent( f, wr, level );
                    /* end of object */
                    LiWriteCstr( f, wr, "}" );
                } else {
                    /* empty object */
                    LiWriteCstr( f, wr, "{}\n" );
                }
                break;
                
            case LI_VTSTR:
                liassert( o->firstChild == NULL );
                LiWriteCstr( f, wr, "\"" );
                if( o->vstr ) {
                    LiWriteLiStr( f, wr, o->vstr );
                }
                LiWriteCstr( f, wr, "\"" );
                break;
                
            case LI_VTINT: 
                code = LiWriteInt( f, wr, o );
                if( code != LI_OK ) {
                    return code;
                }
                break;
                
            case LI_VTUINT:
                code = LiWriteUint( f, wr, o );
                if( code != LI_OK ) {
                    return code;
                }
                break;
            
            case LI_VTBOOL:
                liassert( o->firstChild == NULL );
                LiWriteCstr( f, wr, o->vint ? "true" : "false" );
                break;
                
            default:
                liverifya( 0, "error: nuknown object type [%d]", o->type );
        }
        
        if( nl ) {
            LiWriteCstr( f, wr, "\n" );
        }
        
    } while( (o = o->next) != NULL );
    
    return code;
}

/*
============
LiWrite
============
*/
licode_t LiWrite( liIO_t *io, liObj_t *o, const char *name, liflag_t flags ) {
    liassert( o );
    
    liFile_t f;
    licode_t code = LI_OK;
    
    if( io == NULL ) {
        io = &liDefaultIO;
    }
    
    f = io->open( name, 'w' );
    if( o ) {
        code = LiWriteHelper_r( f, io->write, o, flags, 0 );
    }
    io->close( f );
    
    return code;
}
