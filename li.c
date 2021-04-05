#include "li.h"

#include <stdlib.h>
#include <stdio.h>


#define liunused(a)     ((void)a)

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

    #define __impl_liassert(e)                          \
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


