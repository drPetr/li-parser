#ifndef __LIMEM_H__
#define __LIMEM_H__

#include "litypes.h"

/* li allocator */
typedef void        *(*fnLiAlloc)(size_t,lityid_t);
typedef void        *(*fnLiRealloc)(void*,size_t,lityid_t);
typedef void        (*fnLiFree)(void*);

/* li alloc object */
typedef struct liAlloc_t {
    fnLiAlloc       alloc;      /* allocate memory */
    fnLiRealloc     realloc;    /* reallocate memory */
    fnLiFree        free;       /* free memory */
} liAlloc_t;

/* dynamic array */
typedef struct {
    lisize_t        allocedNum; /* number of alloced elements */
    lisize_t        sizOfElem;  /* size of one item */
    lisize_t        number;     /* number of used array elements */
    char            array_[0];   /* array */
} liArray_t;


#define     aalc(a)     ((a)->allocedNum)
#define     asiz(a)     ((a)->sizOfElem)
#define     anum(a)     ((a)->number)
#define     aarr(a)     ((a)->array_)


void        *LiAlloc( size_t size, lityid_t type );
void        *LiRealloc( void *ptr, size_t size, lityid_t type );
void        LiDealloc( void *ptr );

void        LiSetAllocator( liAlloc_t *alc );
liAlloc_t   *LiGetAllocator( void );

liArray_t   *LiArrayAlloc( lisize_t siz, lisize_t num );
liArray_t   *LiArrayRealloc( liArray_t *array, lisize_t num );
void        LiArrayFree( liArray_t *array );
void        LiArrayEmpty( liArray_t *array );
liArray_t   *LiArrayAppend( liArray_t *array, void *data );


#endif //__LIMEM_H__