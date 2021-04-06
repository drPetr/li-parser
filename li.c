#include "li.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

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



/*
================================================
                   allocator
================================================
*/

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
    liunused( type );    
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
LiValInit
============
*/
void LiValInit( liVal_t *val, litype_t type ) {
    liassert( val );
    val->vstr = NULL;
    val->type = type;
    val->flags = 0;
}

/*
============
LiValInitLiStr
============
*/
void LiValInitLiStr( liVal_t *val, liStr_t *s ) {
    liassert( val );
    
    LiValInit( val, LI_TSTR );
    val->vstr = s;
}

/*
============
LiValFree
============
*/
void LiValFree( liVal_t *val ) {
    switch( val->type ) {
        case LI_TNULL:
            break;
            
        case LI_TSTR:
            LiStrFree( val->vstr );
            break;
    }
    
    liverifya( 0, "error: unknown value type [%d]", val->type );
}

/*
============
LiValArrayAlloc
============
*/
liValArray_t *LiValArrayAlloc( uint32_t num ) {
    liValArray_t *array;
    uint32_t siz;
    
    liassert( num >= 1 );
    
    /* num = CeilPow2( num ); */
    siz = sizeof(liValArray_t) + sizeof(liVal_t) * num;
    array = (liValArray_t*)LiAlloc( siz, LI_TYID_VALARR );
    array->arrayMaxNumber = num;
    array->arrayNumber = 0;
    
    return array;
}

/*
============
LiValArrayRealloc
============
*/
liValArray_t *LiValArrayRealloc( liValArray_t *array, uint32_t num ) {
    uint32_t siz;
    
    liassert( array );
    liassert( num >= 1 );
    
    num = CeilPow2( num );
    /* need to re-allocate memory? */
    if( num == array->arrayMaxNumber ) {
        /* no need to reallocate memory */
        return array;
    }
    
    /* reallocate */
    siz = sizeof(liValArray_t) + sizeof(liVal_t) * num;
    array = (liValArray_t*)LiRealloc( array, siz, LI_TYID_VALARR );
    array->arrayMaxNumber = num;
    if( array->arrayNumber > array->arrayMaxNumber ) {
        array->arrayNumber = array->arrayMaxNumber;
    }
    
    onlydebug( printf( "LiValArrayRealloc()[%d]\n", num ) );
    
    return array;
}

/*
============
LiValArrayFree
============
*/
void LiValArrayFree( liValArray_t *array ) {
    int i;
    
    liassert( array );
    
    for( i = 0; i < array->arrayNumber; i++ ) {
        LiValFree( &(array->array[i]) );
    }
    LiDealloc( array );
}

/*
============
LiValArrayAppendEmpty
============
*/
liVal_t *LiValArrayAppendEmpty( liValArray_t **array ) {
    liassert( array );
    
    /* check if array not exists */
    if( !*array ) {
        /* array not exist */
        /* create a new array and allocate memory for one element */
        *array = LiValArrayAlloc( 1 );
    } else if( (*array)->arrayNumber == (*array)->arrayMaxNumber ) {
        /* array exists */
        /* reallocate memory for additional element */
        uint32_t num = (*array)->arrayNumber;
        (*array) = LiValArrayRealloc( (*array), num + 1 );
    }
    /* initialize value as null */
    liVal_t *v = &((*array)->array[(*array)->arrayNumber++]);
    LiValInit( v, LI_TNULL );
    
    return v;
}


    



/*
================================================
                     li node
================================================
*/

/*
============
LiInsertionHelper
============
*/
static void LiInsertionHelper( liNode_t *left, liNode_t *right, 
        liNode_t *parent, liNode_t *insert ) {
    /* first node in the insert sequence */
    liNode_t *firstInsert = insert;
    /* last node in the insert sequence */
    liNode_t *lastInsert = NULL;
    liNode_t *tmp = insert;
    
    liassert( left || right || parent );
    liassert( insert );

    /* set the whole sibling sequence of the parent and get a
    pointer to the last node of the sibling sequence */
    while( tmp ) {
        lastInsert = tmp;
        tmp->parent = parent;
        tmp = tmp->next;
    }
    
    while( firstInsert->prev ) {
        firstInsert = firstInsert->prev;
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
void LiInsertFirstChild( liNode_t *node, liNode_t *insert ) {
    liassert( node );
    liassert( insert );
    LiInsertionHelper( NULL, node->firstChild, node, insert );
}

/*
============
LiInsertLastChild

Insert the insert node as the last child of the node
============
*/
void LiInsertLastChild( liNode_t *node, liNode_t *insert ) {
    liassert( node );
    liassert( insert );
    LiInsertionHelper( node->lastChild, NULL, node, insert );
}

/*
============
LiInsertBefore

Insert an insert node before the node
============
*/
void LiInsertBefore( liNode_t *node, liNode_t *insert ) {
    liassert( node );
    liassert( insert );
    LiInsertionHelper( node->prev, node, node->parent, insert );
}

/*
============
LiInsertAfter

Insert an insert node after a node
============
*/
void LiInsertAfter( liNode_t *node, liNode_t *insert ) {
    liassert( node );
    liassert( insert );
    LiInsertionHelper( node, node->next, node->parent, insert );
}

/*
====================
LiInsertFirst

Insert the insert node first in the sequence node
====================
*/
void LiInsertFirst( liNode_t *node, liNode_t *insert ) {
    liassert( node );
    liassert( insert );
    
    while( node->prev ) {
        node = node->prev;
    }
    LiInsertionHelper( NULL, node, node->parent, insert );
}

/*
====================
LiInsertLast

Insert the insert node as the last node in the sequence
====================
*/
void LiInsertLast( liNode_t *node, liNode_t *insert ) {
    liassert( node );
    liassert( insert );
    
    while( node->next ) {
        node = node->next;
    }
    LiInsertionHelper( node, NULL, node->parent, insert );
}

/*
============
LiExtractionHelper
============
*/
static liNode_t *LiExtractionHelper( liNode_t *left, liNode_t *right ) {
    liNode_t *ext = NULL;
    liNode_t *it;
    liNode_t *parent;
    liNode_t *before;
    liNode_t *after;
    
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
liNode_t *LiExtract( liNode_t *node ) {
    liassert( node );
    return LiExtractionHelper( node, node );
}

/*
============
LiExtractSiblings
============
*/
liNode_t *LiExtractSiblings( liNode_t *left, liNode_t *right ) {
    liassert( left || right );
    return LiExtractionHelper( left, right );
}

/*
============
LiExtractChildren
============
*/
liNode_t *LiExtractChildren( liNode_t *node ) {
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
liNode_t *LiNodeCreate( void ) {
    liNode_t *node;
    
    node = (liNode_t*)LiAlloc( sizeof(liNode_t), LI_TYID_NODE );
    
    node->parent = NULL;
    node->next = NULL;
    node->prev = NULL;
    node->firstChild = NULL;
    node->lastChild = NULL;
    node->key = NULL;
    node->innerCom = NULL;
    node->afterCom = NULL;
    node->values = NULL;
    
    return node;
}

/*
============
LiNodeFreeSubtreeHelper_r
============
*/
static void LiNodeFreeSubtreeHelper_r( liNode_t *node ) {
    liNode_t *next;

    liassert( node );

    do {
        next = node->next;
        if( node->firstChild ) {
            /* free array of values */
            if( node->values ) {
                LiValArrayFree( node->values );
            }
            /* free node */
            LiNodeFreeSubtreeHelper_r( node->firstChild );
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
void LiFreeSubtree( liNode_t *node ) {
    liassert( node );
    node = LiExtract( node );
    liassert( node );
    LiNodeFreeSubtreeHelper_r( node );
}

/*
============
LiFree
============
*/
void LiFree( liNode_t *li ) {
    liassert( li );
    
    while( li->parent ) {
        li = li->parent;
    }
    LiNodeFreeSubtreeHelper_r( li );
}



/*
================================================
                    li object
================================================
*/

/*
============
LiObjCreate
============
*/
liNode_t *LiObjCreate( void ) {
    return LiNodeCreate();
}

/*
============
LiObjSetKeyStr
============
*/
void LiObjSetKeyStr( liNode_t *o, const char *s, uint32_t len ) {
    liassert( o );
    
    if( !s ) {
        if( o->key ) {
            LiStrFree( o->key );
            o->key = NULL;
        }
    } else {
        LiStrSet( &(o->key), s, len );
    }
}

/*
============
LiObjSetKeyCstr
============
*/
void LiObjSetKeyCstr( liNode_t *o, const char *s ) {
    liassert( o );
    LiObjSetKeyStr( o, s, (uint32_t)strlen(s) );
}

/*
============
LiObjCreateStr
============
*/
liNode_t *LiObjCreateStr( const char *key, uint32_t len ) {
    liNode_t *node;
    
    node = LiNodeCreate();
    node->key = LiStrCreate( key, len );
    
    return node;
}

/*
============
LiObjCreateCstr
============
*/
liNode_t *LiObjCreateCstr( const char *key ) {
    return LiObjCreateStr( key, (uint32_t)strlen(key) );
}

/*
============
LiObjAppendInnerCom
============
*/
void LiObjAppendInnerCom( liNode_t *o, const char *com, uint32_t len ) {
    liassert( o );
    liassert( com );
    
    if( o->innerCom ) {
        LiStrConcat( &(o->innerCom), com, len );
    } else {
        o->innerCom = LiStrCreate( com, len );
    }
}

/*
============
LiObjAppendInnerComCstr
============
*/
void LiObjAppendInnerComCstr( liNode_t *o, const char *com ) {
    liassert( o );
    liassert( com );
    LiObjAppendInnerCom( o, com, (uint32_t)strlen(com) );
}

/*
============
LiObjAppendAfterCom
============
*/
void LiObjAppendAfterCom( liNode_t *o, const char *com, uint32_t len ) {
    liassert( o );
    liassert( com );
    if( o->afterCom ) {
        LiStrConcat( &(o->afterCom), com, len );
    } else {
        o->afterCom = LiStrCreate( com, len );
    }
}

/*
============
LiObjAppendAfterComCstr
============
*/
void LiObjAppendAfterComCstr( liNode_t *o, const char *com ) {
    liassert( o );
    liassert( com );
    LiObjAppendAfterCom( o, com, (uint32_t)strlen(com) );
}

/*
============
LiObjAppendValNull
============
*/
void LiObjAppendValNull( liNode_t *o ) {
    liassert( o );
    LiValArrayAppendEmpty( &(o->values) );
}

/*
============
LiObjAppendValStr
============
*/
void LiObjAppendValStr( liNode_t *o, const char *s, uint32_t len ) {
    liassert( o );
    liassert( s );
    
    liVal_t *v = LiValArrayAppendEmpty( &(o->values) );
    LiValInitLiStr( v, LiStrCreate( s, len ) );
}

/*
============
LiObjAppendValCstr
============
*/
void LiObjAppendValCstr( liNode_t *o, const char *s ) {
    liassert( o );
    liassert( s );
    
    liVal_t *v = LiValArrayAppendEmpty( &(o->values) );
    LiValInitLiStr( v, LiStrCreateCstr( s ) );
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
LiWriteVal
============
*/
static licode_t LiWriteVal( liFile_t f, fnLiWrite wr, liVal_t *v ) {
    liassert( v );
    
    switch( v->type ) {
        case LI_TNULL:
            liassert( v->p == NULL );
            LiWriteCstr( f, wr, "null" );
            break;
            
        case LI_TSTR:
            if( v->vstr == NULL ) {
                LiWriteCstr( f, wr, "\"\"" );
            } else {
                LiWriteCstr( f, wr, "\"" );
                LiWriteLiStr( f, wr, v->vstr );
                LiWriteCstr( f, wr, "\"" );
            }
            break;
            
        default:
            liverifya( 0, "error: unknown value type [%d]", v->type );
    }
    
    return LI_OK;
}

/*
============
LiWriteValArray
============
*/
static licode_t LiWriteValArray( liFile_t f, fnLiWrite wr, liValArray_t *a ) {
    liassert( a );
    
    
    /* write first value */
    licode_t ret;
    ret = LiWriteVal( f, wr, a->array );
    if( ret != LI_OK ) {
        return ret;
    }
    
    int i;
    for( i = 1; i < a->arrayNumber; i++ ) {
        LiWriteCstr( f, wr, ", " );
        /* write array of values */
        ret = LiWriteVal( f, wr, a->array + i );
        if( ret != LI_OK ) {
            break;
        }
    }
    
    return ret;
}

/*
============
LiWriteHelper_r
============
*/
static void LiWriteHelper_r( liFile_t f, fnLiWrite wr, liNode_t *o,
        int indent, liflag_t flags ) {
    do {
        liassert( (o->firstChild && !o->values) || (!o->firstChild) );
        
        LiWriteIndent( f, wr, indent );
        
        /* write key */
        if( o->key ) {
            LiWriteLiStr( f, wr, o->key );
            LiWriteCstr( f, wr, " = " );
        }
        
        /* write subtree */
        if( o->firstChild ) {
            /* begin of object */
            LiWriteCstr( f, wr, "{\n" );
            LiWriteHelper_r( f, wr, o->firstChild, indent + 1, flags );
            LiWriteIndent( f, wr, indent );
            /* end of object */
            LiWriteCstr( f, wr, "}" );
        } else if( o->values ) {
            LiWriteValArray( f, wr, o->values );
        } else {
            LiWriteCstr( f, wr, "null" );
        }
        LiWriteCstr( f, wr, "\n" );
    } while( (o = o->next) != NULL );
}

/*
============
LiWrite
============
*/
void LiWrite( liIO_t *io, liNode_t *o, const char *name, liflag_t flags ) {
    liassert( o );
    
    liFile_t f;
    
    if( io == NULL ) {
        io = &liDefaultIO;
    }
    
    f = io->open( name, 'w' );
    if( o ) {
        LiWriteHelper_r( f, io->write, o, 0, flags );
    }
    io->close( f );
}
