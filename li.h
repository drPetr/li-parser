#ifndef __LI_H__
#define __LI_H__

#include <stdint.h>

/* type identificator */
typedef uint32_t        lityid_t;
typedef uint8_t         litype_t;
typedef uint8_t         libool_t;
typedef uint32_t        licode_t;

/* allocator */
typedef void            *(*fnLiAlloc)(size_t,lityid_t);
typedef void            (*fnLiFree)(void*);

typedef struct liAlloc_t {
    fnLiAlloc           alloc;
    fnLiFree            free;
} liAlloc_t;



#define lifalse         ((libool_t)0)
#define litrue          ((libool_t)1)

/* error codes */
#define LI_OK           ((licode_t)0)






void        LiSetAllocator( liAlloc_t *alloc );
liAlloc_t   *LiGetAllocator( void );


#endif //__LI_H__