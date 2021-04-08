#include "limem.h"
#include "liassert.h"
#include "liutil.h"

extern liAlloc_t liDefaultAllocator;
static liAlloc_t *liAllocator = &liDefaultAllocator;

/*
============
LiAlloc
============
*/
void *LiAlloc( size_t size, lityid_t type ) {
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
void *LiRealloc( void *ptr, size_t size, lityid_t type ) {
    liassert( liAllocator );
    liassert( liAllocator->realloc );
    liassert( size != 0 );
    
    return liAllocator->realloc( ptr, size, type );
}

/*
============
LiDealloc
============
*/
void LiDealloc( void *ptr ) {
    liassert( liAllocator );
    liassert( liAllocator->free );
    liassert( ptr );
    
    liAllocator->free( ptr );
}

/*
============
LiSetAllocator
============
*/
void LiSetAllocator( liAlloc_t *alc ) {
    liassert( alc );
    liassert( alc->alloc );
    liassert( alc->realloc );
    liassert( alc->free );
    
    liAllocator = alc;
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
                    li array
================================================
*/

/*
============
LiArrayAlloc
============
*/
liArray_t *LiArrayAlloc( lisize_t siz, lisize_t num ) {
    liassert( siz >= 1 );
    liassert( num >= 1 );
    
    liArray_t *array;
    
    lisize_t memSize = sizeof(liArray_t) + siz * num;
    array = (liArray_t*)LiAlloc( memSize, LI_TYID_ARR );
    aalc(array) = num;
    asiz(array) = siz;
    anum(array) = 0;
    
    return array;
}

/*
============
LiArrayRealloc
============
*/
liArray_t *LiArrayRealloc( liArray_t *array, lisize_t num ) {
    liassert( array );
    liassert( num >= 1 );
    
    num = CeilPow2( num );
    /* need to re-allocate memory? */
    if( num == aalc(array) ) {
        /* no need to reallocate memory */
        return array;
    }
    
    /* reallocate */
    lisize_t memSize = sizeof(liArray_t) + asiz(array) * num;
    array = (liArray_t*)LiRealloc( array, memSize, LI_TYID_ARR );
    aalc(array) = num;
    if( aalc(array) > anum(array) ) {
        anum(array) = aalc(array);
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
    anum(array) = 0;
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
    if( anum(array) + 1 > aalc(array) ) {
        array = LiArrayRealloc( array, anum(array) + 1 );
    }
    
    /* copy data to array */
    MemCpy( aarr(array) + anum(array) * asiz(array), data, asiz(array) );
    anum(array) += 1;
    return array;
}

