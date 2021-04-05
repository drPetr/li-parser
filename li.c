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
__impl_liverify
============
*/
static void __impl_liverify( int e, const char* expr, const char* file, 
        int line, const char *function ) {
    if( !e ){
        fprintf( stderr, "verification failed: %s\n", expr );
        fprintf( stderr, "file: %s\n", file );
        fprintf( stderr, "line: %d\n", line );
        fprintf( stderr, "function: %s\n", function );
        exit(1);
    }
}

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
        vfprintf( stderr, fmt, args );
        va_end( args );
        exit(1);
    }
}

#define liverify(e)                                 \
    __impl_liverify( (int)(!!(e)), #e, __FILE__,    \
    __LINE__, __FUNCTION__ )

#define liverifya(e,a,...)                          \
    __impl_liverifya( (int)(!!(e)), #e, __FILE__,  \
    __LINE__, __FUNCTION__, a, ##__VA_ARGS__ )



#if defined(LI_NODBG) || (!defined(DEBUG))
    #define onlydebug(act)
    #define liassert(e)
    #define liasserta(e,a,...)
#else
/*
============
__impl_liassert
============
*/
static void __impl_liassert( int e, const char* expr, const char* file, 
        int line, const char *function ) {
    if( !e ){
        fprintf( stderr, "assertion failed: %s\n", expr );
        fprintf( stderr, "file: %s\n", file );
        fprintf( stderr, "line: %d\n", line );
        fprintf( stderr, "function: %s\n", function );
        exit(1);
    }
}

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
        vfprintf( stderr, fmt, args );
        va_end( args );
        exit(1);
    }
}

    #define liassert(e)                          \
        __impl_liassert( (int)(!!(e)), #e, __FILE__,    \
        __LINE__, __FUNCTION__ )
        
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
    
    liassert( siz != 0 );
    
    siz = (siz + 31) & ~31; 
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
    liassert( siz != 0 );
    
    siz = (siz + 31) & ~31; 
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
================================================
                    li value
================================================
*/

/*
============
LiValArrayAlloc
============
*/
liValArray_t *LiValArrayAlloc( uint32_t num ) {
    liValArray_t *array;
    uint32_t siz;
    
    liassert( num != 0 );
    
    num = (num + 31) & ~31;
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
    liassert( num != 0 );
    
    num = (num + 31) & ~31;
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
    
    return array;
}

/*
============
LiStrFree
============
*/
void LiValArrayFree( liValArray_t *array ) {
    liassert( array );
    LiDealloc( array );
}

/*
============
LiValInit
============
*/
void LiValInit( liVal_t *val, litype_t type ) {
    val->vstr = NULL;
    val->type = type;
    val->flags = 0;
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
    node->id = NULL;
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