#ifndef __LI_H__
#define __LI_H__

#include "litypes.h"
#include "limem.h"
#include "listr.h"

#define LI_MAX_NESTING_LEVEL    4096




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


/* li object */
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


/* find data */
typedef struct {
    liObj_t             *obj;
    liArray_t           *pattern;
    uint32_t            index;
} liFindData_t;





liObj_t     *LiParent( liObj_t *o );
liObj_t     *LiNext( liObj_t *o );
liObj_t     *LiPrev( liObj_t *o );
liObj_t     *LiFirstChild( liObj_t *o );
liObj_t     *LiLastChild( liObj_t *o );
liObj_t     *LiFirst( liObj_t *o );
liObj_t     *LiLast( liObj_t *o );
liObj_t     *LiRoot( liObj_t *o );


void        LiInsertFirstChild( liObj_t *node, liObj_t *insert );
void        LiInsertLastChild( liObj_t *node, liObj_t *insert );
void        LiInsertBefore( liObj_t *node, liObj_t *insert );
void        LiInsertAfter( liObj_t *node, liObj_t *insert );
void        LiInsertFirst( liObj_t *node, liObj_t *insert );
void        LiInsertLast( liObj_t *node, liObj_t *insert );
liObj_t     *LiExtract( liObj_t *node );
liObj_t     *LiExtractSiblings( liObj_t *left, liObj_t *right );
liObj_t     *LiExtractChildren( liObj_t *node );
void        LiFreeSubtree( liObj_t *node );
void        LiFree( liObj_t *li );


void        LiSetKey( liObj_t *o, const char *key );
void        LiSetKeyL( liObj_t *o, const char *key, lisize_t len );
void        LiSetFlags( liObj_t *o, liflag_t flags );
libool_t    LiTypeIs( liObj_t *o, litype_t type );
libool_t    LiIsObj( liObj_t *o );
liObj_t     *LiObj( void );
liObj_t     *LiNull( void );
liObj_t     *LiStr( const char *s );
liObj_t     *LiStrL( const char *s, lisize_t len );
liObj_t     *LiInt( int64_t i );
liObj_t     *LiUint( uint64_t u );
liObj_t     *LiBool( libool_t b );


licode_t    LiFindFirst( liFindData_t *dat, liObj_t *o, const char *s );
licode_t    LiFindNext( liFindData_t *dat );
licode_t    LiFindClose( liFindData_t *dat );


licode_t    LiWrite( liObj_t *o, const char *name );
licode_t    LiWriteEx( liIO_t *io, liObj_t *o, const char *name, 
                    liflag_t flags );

licode_t    LiRead( liObj_t **o, const char *name );
licode_t    LiReadEx( liIO_t *io, liObj_t **o, const char *name, 
                    liflag_t flags, char *errbuf, size_t errbufLen );


#endif //__LI_H__