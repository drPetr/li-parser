#ifndef __LI_H__
#define __LI_H__

#include <stdint.h>

/* litypes */
typedef uint32_t        lityid_t;
typedef uint32_t        litype_t;
typedef uint32_t        libool_t;
typedef uint32_t        licode_t;
typedef uint32_t        liflag_t;

/* lityid */
#define LI_TYID_STR     1
#define LI_TYID_NODE    2
#define LI_TYID_VALARR  3

/* litype */
#define LI_TNULL       0
#define LI_TOBJ        1
#define LI_TARR        2
#define LI_TSTR        3

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

/* livalue */
typedef struct liVal_t {
    union {
        void            *p;         /* pointer */
        liStr_t         *vstr;      /* "string" value */
    };
    litype_t            type;       /* value type */
    uint32_t            flags;
} liVal_t;

/* livalue array */
typedef struct {
    uint32_t        arrayMaxNumber; /* maximum number of values in an array */
    uint32_t        arrayNumber;    /* number of values */
    liVal_t         array[0];       /* array of values */
} liValArray_t;


/* liNode_t */
typedef struct liNode_t {
    struct liNode_t *parent;        /* parent */
    struct liNode_t *next;          /* next sibling */
    struct liNode_t *prev;          /* previous sibling */
    
    
    struct liNode_t *firstChild;    /* first child */
    struct liNode_t *lastChild;     /* last child */
    
    liStr_t         *key;           /* key */
    liStr_t         *innerCom;      /* inner ...{# li comment\n...} */
    liStr_t         *afterCom;      /* after {...}# li comment\n... */
    
    liValArray_t    *values;        /* array of values */
} liNode_t;


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


void        LiValInit( liVal_t *val, litype_t type );
void        LiValInitLiStr( liVal_t *val, liStr_t *s );
void        LiValFree( liVal_t *val );
liValArray_t *LiValArrayAlloc( uint32_t num );
liValArray_t *LiValArrayRealloc( liValArray_t *array, uint32_t num );
void        LiValArrayFree( liValArray_t *array );
liVal_t     *LiValArrayAppendEmpty( liValArray_t **array );


void        LiInsertFirstChild( liNode_t *node, liNode_t *insert );
void        LiInsertLastChild( liNode_t *node, liNode_t *insert );
void        LiInsertBefore( liNode_t *node, liNode_t *insert );
void        LiInsertAfter( liNode_t *node, liNode_t *insert );
void        LiInsertFirst( liNode_t *node, liNode_t *insert );
void        LiInsertLast( liNode_t *node, liNode_t *insert );
liNode_t    *LiExtract( liNode_t *node );
liNode_t    *LiExtractSiblings( liNode_t *left, liNode_t *right );
liNode_t    *LiExtractChildren( liNode_t *node );
liNode_t    *LiNodeCreate( void );
void        LiFreeSubtree( liNode_t *node );
void        LiFree( liNode_t *li );


liNode_t    *LiObjCreate( void );

void        LiObjSetKeyStr( liNode_t *o, const char *s, uint32_t len );
void        LiObjSetKeyCstr( liNode_t *o, const char *s );
void        LiObjSetKeyRef( liNode_t *o, liNode_t *ref );

liNode_t    *LiObjCreateStr( const char *key, uint32_t len );
liNode_t    *LiObjCreateCstr( const char *key );
void        LiObjAppendInnerCom( liNode_t *o, const char *com, uint32_t len );
void        LiObjAppendInnerComCstr( liNode_t *o, const char *com );
void        LiObjAppendAfterCom( liNode_t *o, const char *com, uint32_t len );
void        LiObjAppendAfterComCstr( liNode_t *o, const char *com );
void        LiObjAppendValNull( liNode_t *o );
void        LiObjAppendValStr( liNode_t *o, const char *s, uint32_t len );
void        LiObjAppendValCstr( liNode_t *o, const char *s );




void        LiWrite( liIO_t *io, liNode_t *o, const char *name, liflag_t flags );



#endif //__LI_H__