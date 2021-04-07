#ifndef __LI_H__
#define __LI_H__

#include <stdint.h>

#define LI_MAX_NESTING_LEVEL    4096

/* litypes */
typedef uint32_t        lityid_t;
typedef uint32_t        litype_t;
typedef uint32_t        libool_t;
typedef uint32_t        licode_t;
typedef uint32_t        liflag_t;

/* lityid */
#define LI_TYID_STR     1
#define LI_TYID_NODE    2

/* litype (li value type) */
#define LI_VTUNDEF      0
#define LI_VTNULL       1
#define LI_VTOBJ        2
#define LI_VTSTR        4
#define LI_VTINT        5
#define LI_VTUINT       6
#define LI_VTBOOL       7

/* libool */
#define lifalse         ((libool_t)0)
#define litrue          ((libool_t)1)

/* licodes */
#define LI_OK           ((licode_t)0)
#define LI_EWRITE       ((licode_t)1)


/* li allocator */
typedef void            *(*fnLiAlloc)(size_t,lityid_t);
typedef void            *(*fnLiRealloc)(void*,size_t,lityid_t);
typedef void            (*fnLiFree)(void*);

typedef struct liAlloc_t {
    fnLiAlloc           alloc;      /* allocate memory */
    fnLiRealloc         realloc;    /* reallocate memory */
    fnLiFree            free;       /* free memory */
} liAlloc_t;

/* li I/O interface */
typedef void*           liFile_t;
typedef liFile_t        (*fnLiOpen)(const char*,char);
typedef void            (*fnLiClose)(liFile_t);
typedef ssize_t         (*fnLiRead)(void*,size_t,liFile_t);
typedef ssize_t         (*fnLiWrite)(const void*,size_t,liFile_t);

typedef struct liIO_t {
    fnLiOpen            open;       /* open file */
    fnLiClose           close;      /* close file */
    fnLiRead            read;       /* read file */
    fnLiWrite           write;      /* write file */
} liIO_t;

/* li string */
typedef struct liStr_t {
    uint32_t            len;        /* length */
    uint32_t            alloced;    /* alloced size */
    char                str[0];     /* null-terminated string */
} liStr_t;

/* liObj_t */
typedef struct liObj_t {
    struct liObj_t      *parent;    /* parent */
    struct liObj_t      *next;      /* next sibling */
    struct liObj_t      *prev;      /* previous sibling */
    struct liObj_t      *firstChild;/* first child */
    struct liObj_t      *lastChild; /* last child */
    
    union {
        void            *ptr;
        liStr_t         *vstr;
        int64_t         vint;
        uint64_t        vuint;
    };
    
    liStr_t             *key;       /* key */
    litype_t            type;       /* object type */
    liflag_t            flags;      /* object flags */
} liObj_t;


void        LiSetAllocator( liAlloc_t *alloc );
liAlloc_t   *LiGetAllocator( void );


liStr_t     *LiStrAlloc( uint32_t siz );
liStr_t     *LiStrRealloc( liStr_t *s, uint32_t siz );
void        LiStrFree( liStr_t *s );
liStr_t     *LiStrCreate( const char *str, uint32_t len );
liStr_t     *LiStrCreateCstr( const char *cstr );
void        LiStrConcat( liStr_t **s, const char *cstr, uint32_t len );
void        LiStrConcatCstr( liStr_t **s, const char *cstr );
void        LiStrSet( liStr_t **s, const char *cstr, uint32_t len );
void        LiStrSetCstr( liStr_t **s, const char *cstr );


void        LiInsertFirstChild( liObj_t *node, liObj_t *insert );
void        LiInsertLastChild( liObj_t *node, liObj_t *insert );
void        LiInsertBefore( liObj_t *node, liObj_t *insert );
void        LiInsertAfter( liObj_t *node, liObj_t *insert );
void        LiInsertFirst( liObj_t *node, liObj_t *insert );
void        LiInsertLast( liObj_t *node, liObj_t *insert );
liObj_t     *LiExtract( liObj_t *node );
liObj_t     *LiExtractSiblings( liObj_t *left, liObj_t *right );
liObj_t     *LiExtractChildren( liObj_t *node );
liObj_t     *LiNodeCreate( void );
void        LiFreeSubtree( liObj_t *node );
void        LiFree( liObj_t *li );


void        LiSetKeyStr( liObj_t *o, const char *key, uint32_t len );
void        LiSetKeyCstr( liObj_t *o, const char *key );
liObj_t     *LiObj( void );
liObj_t     *LiNull( void );
liObj_t     *LiStr( const char *s, uint32_t len );
liObj_t     *LiCstr( const char *s );
liObj_t     *LiInt( int64_t i );
liObj_t     *LiUint( uint64_t u, liflag_t flags );
liObj_t     *LiBool( libool_t b );




licode_t    LiWrite( liIO_t *io, liObj_t *o, const char *name, liflag_t flags );



#endif //__LI_H__