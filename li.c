#include "li.h"
#include "liassert.h"
#include "liutil.h"



/*
================================================
              li object navigation
================================================
*/

/*
============
LiParent
============
*/
liObj_t *LiParent( liObj_t *o ) {
    liassert(o);
    return o->parent;
}

/*
============
LiNext
============
*/
liObj_t *LiNext( liObj_t *o ) {
    liassert(o);
    return o->next;
}

/*
============
LiPrev
============
*/
liObj_t *LiPrev( liObj_t *o ) {
    liassert(o);
    return o->prev;
}

/*
============
LiFirstChild
============
*/
liObj_t *LiFirstChild( liObj_t *o ) {
    liassert(o);
    return o->firstChild;
}

/*
============
LiLastChild
============
*/
liObj_t *LiLastChild( liObj_t *o ) {
    liassert(o);
    return o->lastChild;
}

/*
============
LiFirst
============
*/
liObj_t *LiFirst( liObj_t *o ) {
    liassert(o);
    if( o->parent ) {
        liassert(o->parent->firstChild);
        return o->parent->firstChild;
    }
    while( o->prev ) {
        o = o->prev;
    }
    return o;
}

/*
============
LiLast
============
*/
liObj_t *LiLast( liObj_t *o ) {
    liassert(o);
    if( o->parent ) {
        liassert(o->parent->lastChild);
        return o->parent->lastChild;
    }
    while( o->next ) {
        o = o->next;
    }
    return o;
}

/*
============
LiRoot
============
*/
liObj_t *LiRoot( liObj_t *o ) {
    liassert(o);
    while( o->parent ) {
        o = o->parent;
    }
    while( o->prev ) {
        o = o->prev;
    }
    return o;
}



/*
================================================
        li object insert/extract functions
================================================
*/

/*
============
InsertionHelper

left - insert it node before firstInsert
right - insert it node after lastInsert
parent - insert it node as parent of the insert sequence
firstInsert - first node in the insert sequence
lastInsert - last node in the insert sequence
============
*/
static void InsertionHelper( liObj_t *left, liObj_t *right, 
        liObj_t *parent, liObj_t *firstInsert, liObj_t *lastInsert ) {    
#if !defined(LI_NODBG) && defined(DEBUG)
    liassert( left || right || parent );
    liassert( firstInsert && lastInsert );
    liassert( firstInsert->prev == NULL );
    liassert( lastInsert->next == NULL );
    liassert( firstInsert != parent );
    liassert( firstInsert != left );
    liassert( firstInsert != right );
    liassert( lastInsert != parent );
    liassert( lastInsert != left );
    liassert( lastInsert != right );
    
    if( 1 ) {
        liObj_t *it = firstInsert;
        do {
            liassert( it->parent == NULL );
        } while( (it = it->next) );
    }
    
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
    /* restoring "family" relations */
    if( parent ) {
        /* set the whole sibling sequence of the parent and get a
        pointer to the last node of the sibling sequence */
        liObj_t *it = firstInsert;
        do {
            it->parent = parent;
        } while( (it = it->next) );
        
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
RestoreKeys
============
*/
static void RestoreKeys( liObj_t *left, liObj_t *parent, liObj_t *restore ){
    liassert( left || parent );
    liassert( restore );
    liassert( restore->prev == NULL );
        
    if( left ) {
        liassert( left->key );
        while( restore && !restore->key ) {
            restore->key = LiSRef( left->key );
            restore = restore->next;
        }
    } else if( parent ) {
        /* TODO */
        /* restore keys by parent */
    }
}

/*
============
LiInsertFirstChild

Insert an insert node as the first child of a node
============
*/
void LiInsertFirstChild( liObj_t *obj, liObj_t *insert ) {
    liassert( obj );
    liassert( insert );
    liassert( obj->type == LI_VTOBJ );
    liassert( LiFirst( insert )->key != NULL );

    liObj_t *firstInsert = LiFirst( insert );
    liObj_t *lastInsert = LiLast( insert );
    RestoreKeys( NULL, obj, firstInsert );
    InsertionHelper( NULL, obj->firstChild, obj, firstInsert, lastInsert );
}



/*
============
LiInsertLastChild

Insert the insert node as the last child of the node
============
*/
void LiInsertLastChild( liObj_t *obj, liObj_t *insert ) {
    liassert( obj );
    liassert( insert );
    liassert( obj->type == LI_VTOBJ );
#if !defined(LI_NODBG) && defined(DEBUG)
    if( obj->lastChild == NULL ) {
        liassert( LiFirst( insert )->key != NULL );
    }
#endif

    liObj_t *firstInsert = LiFirst( insert );
    liObj_t *lastInsert = LiLast( insert );
    if( !firstInsert->key ) {
        RestoreKeys( obj->lastChild, obj, firstInsert );
    }
    InsertionHelper( obj->lastChild, NULL, obj, firstInsert, lastInsert );
}

/*
============
LiInsertBefore

Insert an insert node before the node
============
*/
void LiInsertBefore( liObj_t *obj, liObj_t *insert ) {
    liassert( obj );
    liassert( insert );
    
    liObj_t *firstInsert = LiFirst( insert );
    liObj_t *lastInsert = LiLast( insert );
    if( !firstInsert->key ) {
        RestoreKeys( obj->prev, obj->parent, firstInsert );
    }
    InsertionHelper( obj->prev, obj, obj->parent, firstInsert, lastInsert );
}

/*
============
LiInsertAfter

Insert an insert node after a node
============
*/
void LiInsertAfter( liObj_t *obj, liObj_t *insert ) {
    liassert( obj );
    liassert( insert );
    
    liObj_t *firstInsert = LiFirst( insert );
    liObj_t *lastInsert = LiLast( insert );
    if( !firstInsert->key ) {
        RestoreKeys( obj, obj->parent, firstInsert );
    }
    InsertionHelper( obj, obj->next, obj->parent, firstInsert, lastInsert );
}

/*
============
LiInsertFirst

Insert the insert node first in the sequence node
============
*/
void LiInsertFirst( liObj_t *obj, liObj_t *insert ) {
    liassert( obj );
    liassert( insert );
    
    obj = LiFirst( obj );
    liObj_t *firstInsert = LiFirst( insert );
    liObj_t *lastInsert = LiLast( insert );
    if( !firstInsert->key ) {
        RestoreKeys( NULL, obj->parent, firstInsert );
    }
    InsertionHelper( NULL, obj, obj->parent, firstInsert, lastInsert );
}

/*
============
LiInsertLast

Insert the insert node as the last node in the sequence
============
*/
void LiInsertLast( liObj_t *obj, liObj_t *insert ) {
    liassert( obj );
    liassert( insert );
    
    obj = LiLast( obj );
    liObj_t *firstInsert = LiFirst( insert );
    liObj_t *lastInsert = LiLast( insert );
    if( !firstInsert->key ) {
        RestoreKeys( obj, obj->parent, firstInsert );
    }
    InsertionHelper( obj, NULL, obj->parent, firstInsert, lastInsert );
}

/*
============
ExtractionHelper
============
*/
static liObj_t *ExtractionHelper( liObj_t *left, liObj_t *right ) {
    liassert( left || right );
    
    /* restore pointers to both left and right nodes */
    if( !right ) {
        right = LiLast( left );
    } else if( !left ) {
        left = LiFirst( right );
    }
    
    liObj_t *parent = left->parent;
    liObj_t *before = left->prev;
    liObj_t *after = right->next;
    liObj_t *it;
    
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
        if( left == parent->firstChild ) {
            if( right == parent->lastChild ) {
                /* no descendants */
                parent->firstChild = NULL;
                parent->lastChild = NULL;
            } else {
                parent->firstChild = after;
            }
        } else if( right == parent->lastChild ) {
            parent->lastChild = before;
        }
        
        it = left;
        do {
            it->parent = NULL;
            it = it->next;
        } while( it != right );
    }
    
    return left;
}

/*
============
LiExtract
============
*/
liObj_t *LiExtract( liObj_t *obj ) {
    liassert( obj );
    return ExtractionHelper( obj, obj );
}

/*
============
LiExtractSiblings
============
*/
liObj_t *LiExtractSiblings( liObj_t *left, liObj_t *right ) {
    liassert( left || right );
#if !defined(LI_NODBG) && defined(DEBUG)
    if( left && right ) {
        liObj_t *it;
        while( it && it != right ) {
            it = it->next;
        }
        liasserta( it == right, "error: left and right are not siblings "
                " or left node is to the right of the right node" );
    }
#endif
    return ExtractionHelper( left, right );
}

/*
============
LiExtractChildren
============
*/
liObj_t *LiExtractChildren( liObj_t *obj ) {
    liassert( obj );
    liassert( obj->firstChild );
    return ExtractionHelper( obj->firstChild, obj->lastChild );
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
            LiSFree( node->key );
        }
        
        /* free object value */
        switch( node->type ) {
            case LI_VTNULL:
                break;
            case LI_VTOBJ:
                /* removal is done below */
                break;
                
            case LI_VTSTR:
                if( node->vstr ) {
                    LiSFree( node->vstr );
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
    LiNodeFreeSubtreeHelper_r( LiExtract(node), 0 );
}

/*
============
LiFree
============
*/
void LiFree( liObj_t *li ) {
    liassert( li );
    LiNodeFreeSubtreeHelper_r( LiRoot(li), 0 );
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
libool_t LiIsCorrectKey( const char *s, lisize_t len ) {
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
LiSetKey
============
*/
void LiSetKey( liObj_t *o, const char *key ) {
    liassert( o );
    LiSetKeyL( o, key, key ? (lisize_t)StrLen(key) : 0 );
}

/*
============
LiSetKeyL
============
*/
void LiSetKeyL( liObj_t *o, const char *key, lisize_t len ) {
    liassert( o );
    if( !key ) {
        if( o->key ) {
            LiSFree( o->key );
            o->key = NULL;
        }
    } else {
        liassert( LiIsCorrectKey( key, len ) );
        if( o->key ) {
            o->key = LiSSetL( o->key, key, len );
        } else {
            o->key = LiSNewL( key, len );
        }
    }
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
NodeCreate
============
*/
static liObj_t *NodeCreate( void ) {
    liObj_t *node;
    
    node = (liObj_t*)LiAlloc( sizeof(liObj_t), LI_TYID_NODE );
    
    node->parent = NULL;
    node->next = NULL;
    node->prev = NULL;
    node->firstChild = NULL;
    node->lastChild = NULL;
    node->ptr = NULL;
    node->key = NULL;
    node->type = 0;
    node->flags = 0;
    
    return node;
}


/*
============
LiObj
============
*/
liObj_t *LiObj( void ) {
    liObj_t *o = NodeCreate();
    o->type = LI_VTOBJ;
    return o;
}

/*
============
LiNull
============
*/
liObj_t *LiNull( void ) {
    liObj_t *o = NodeCreate();
    o->type = LI_VTNULL;
    return o;
}

/*
============
LiStr
============
*/
liObj_t *LiStr( const char *s ) {
    return LiStrL( s, s ? (lisize_t)StrLen(s) : 0 );
}

/*
============
LiStrL
============
*/
liObj_t *LiStrL( const char *s, lisize_t len ) {
    liObj_t *o = NodeCreate();
    o->type = LI_VTSTR;
    o->vstr = s ? LiSNewL( s, len ) : NULL;
    return o;
}

/*
============
LiInt
============
*/
liObj_t *LiInt( int64_t i ) {
    liObj_t *o = NodeCreate();
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
    liObj_t *o = NodeCreate();
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
    liObj_t *o = NodeCreate();
    o->type = LI_VTBOOL;
    o->vint = b;
    return o;
}



struct patternData_s{
    const char      *str;
    lisize_t        len;
    libool_t        checked;
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
    dat->obj = NULL;
    dat->pattern = NULL;
    dat->index = 0;
    
    if( !LiIsCorrectRefStr(s) ) {
        return LI_EINPDAT;
    }
    
    /* check for global pattern (start from the root) */
    if( *s == '.' ) {
        s++;
        /* go to root */
        while( o->parent ) {
            o = o->parent;
        }
        while( o->prev ) {
            o = o->prev;
        }
    }
    
    /* parse the search pattern */
    if( *s ) {
        /* init pattern array */
        dat->pattern = LiArrayAlloc( sizeof(struct patternData_s), 16 );
        dat->obj = o;
        
        struct patternData_s patternData;
        const char *startSubstr;
        while( is_firstkeych(*s) ) {
            patternData.str = startSubstr = s;
            s++;
            for( ; is_nextkeych(*s); s++ );
            patternData.len = (lisize_t)(s - startSubstr);
            patternData.checked = lifalse;
            dat->pattern = LiArrayAppend( dat->pattern,
                    &patternData );
            if( *s == '.' && s[1] != 0 ) {
                s++;
            }
        }
        if( *s != 0 ) {
            /* the string is invalid */
            return LI_EINPDAT;
        }
    } else {
        /* the string is empty */
        return LI_EINPDAT;
    }
    
    /* check if the first element is found */
    struct patternData_s *pattern = (struct patternData_s*)aarr(dat->pattern);
    if( (anum(dat->pattern) == 1) && o->key && LiSCmpL( o->key, pattern->str,
            slen(o->key) ) ) {
        dat->obj = o;
        return LI_OK;
    }
    
    return LiFindNext( dat );
}

static liObj_t *SkipToNext( liFindData_t *dat, liObj_t *o, int toDown ) {
    if( toDown && (dat->index < (anum(dat->pattern) - 1)) && o->firstChild ) {
        /* skip down */
        dat->index++;
        o = o->firstChild;
    } else if( o->next ) {
        /* skip to next sibling */
        o = o->next;
    } else if( (dat->index > 0) && o->parent ) {
        /* skip up */
        do {
            dat->index--;
            o = o->parent;
        } while( (dat->index > 0) && (!o->next) && (o->parent) );
        o = o->next;
    } else {
        /* no unvisited nodes left */
        return NULL;
    }
    
    return o;
}

/*
============
LiFindNext
============
*/
licode_t LiFindNext( liFindData_t *dat ) {
    liassert( dat );
    liassert( dat->pattern );
    
    liObj_t *o = dat->obj;
    if( !o ) {
        return LI_FINISHED;
    }
    
    /* skip to next */
    o = SkipToNext( dat, o, 1 );
    
    /* find next */
    struct patternData_s *pattern;
    while( o ) {
        pattern = ((struct patternData_s*)aarr(dat->pattern)) + dat->index;
        
       // Log( "%s\n", (o->key ? sstr(o->key) : "(no key)" ) );
        
        if( o->key && LiSCmpL(o->key, pattern->str, slen(o->key)) ){
            if( dat->index == (anum(dat->pattern) - 1) ) {
                /* object found */
                dat->obj = o;
                return LI_OK;
            }
            o = SkipToNext( dat, o, 1 );
            continue;
        } else if( !o->key ) {
            o = SkipToNext( dat, o, 1 );
        } else {
            o = SkipToNext( dat, o, 0 );
        }
    }
    
    dat->obj = NULL;
    return LI_FINISHED;
}

/*
============
LiFindClose
============
*/
licode_t LiFindClose( liFindData_t *dat ) {
    liassert( dat );
    
    if( dat->pattern ) {
        LiArrayFree( dat->pattern );
        dat->pattern = NULL;
    }
    dat->obj = NULL;
    dat->index = 0;
    
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
        const char *s, lisize_t len ) {
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
    return LiWriteStr( f, wr, s, (lisize_t)StrLen(s) );
}

/*
============
LiWriteLiStr
============
*/
static licode_t LiWriteLiStr( liFile_t f, fnLiWrite wr, liStr_t *s ) {
    liassert( s );
    return LiWriteStr( f, wr, sstr(s), slen(s) );
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
WriteHelper_r
============
*/
static licode_t WriteHelper_r( liFile_t f, fnLiWrite wr, 
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
        if( !o->prev || (o->prev->key != o->key) ) {
            LiWriteLiStr( f, wr, o->key );
            LiWriteCstr( f, wr, " = " );
        } else {
            LiWriteCstr( f, wr, ", " );
        }
        if( o->next && (o->next->key == o->key) ) {
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
                    code = WriteHelper_r( f, wr, o->firstChild, 
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
licode_t LiWrite( liObj_t *o, const char *name ) {
    liassert(o);
    return LiWriteEx( NULL, o, name, 0 );
}

/*
============
LiWriteEx
============
*/
licode_t LiWriteEx( liIO_t *io, liObj_t *o, const char *name,
        liflag_t flags ) {
    liassert(o);
    
    liFile_t f;
    licode_t code = LI_OK;
    
    if( io == NULL ) {
        extern liIO_t liDefaultIO;
        io = &liDefaultIO;
    }
    
    f = io->open( name, 'w' );
    if( f == NULL ) {
        return LI_EFILEOPEN;
    }
    if( o ) {
        code = WriteHelper_r( f, io->write, o, flags, 0 );
    }
    io->close( f );
    
    return code;
}



/*
================================================
                    li parser
================================================
*/

typedef struct {
    liFile_t    f;
    fnLiRead    rd;
    liStr_t     *scanBuf;   /* buffered file data */
    liStr_t     *errBuf;    

    char        *tkBeg;
    char        *tkFwd;
    lisize_t    tkLen;
    int         tk;

    char        *lnBeg;
    int         ln;
    int         col;
    
    liStr_t     *storage;
    
    libool_t    chProc;     /* char processed */
} liScan_t;

/*
============
ScanInit
============
*/
static void ScanInit( liScan_t *scan, liFile_t f, fnLiRead rd ) {
    scan->f = f;
    scan->rd = rd;
    scan->scanBuf = LiSAlloc( 1024 );
    scan->errBuf = NULL;    

    scan->tkBeg = sstr(scan->scanBuf);
    scan->tkFwd = sstr(scan->scanBuf);
    scan->tkLen = 0;
    scan->tk = 0;

    scan->lnBeg = sstr(scan->scanBuf);
    scan->ln = 1;
    scan->col = 1;
    
    scan->storage = NULL;
    
    scan->chProc = lifalse;
}

/*
============
ScanFree
============
*/
static void ScanFree( liScan_t *scan ) {
    if( scan->scanBuf ) {
        LiSFree( scan->scanBuf );
    }
    if( scan->errBuf ) {
        LiSFree( scan->errBuf );
    }
}


#define tb      (scan->tkBeg)
#define tf      (scan->tkFwd)
#define tl      (scan->tkLen)
#define tk      (scan->tk)
#define lb      (scan->lnBeg)
#define ln      (scan->ln)
#define lc      (scan->col)

/*
============
UpdateBuffer
============
*/
static ssize_t UpdateBuffer( liScan_t *scan ) {
    char *min;
    if( lb < tb ) {
        if( (size_t)(tb - lb) > 64 ) {
            lb = tb - 64;
        }
        min = lb;
    } else {
        min = tb;
    }
    ssize_t left = min - sstr(scan->scanBuf);
    ssize_t right = salc(scan->scanBuf) - slen(scan->scanBuf);
    
    liassert( left >= 0 );
    liassert( right >= 0 );
    
    if( left ) {
        ssize_t siz = slen(scan->scanBuf) - left;
        liassert( siz > 0 );
        MemCpy( sstr(scan->scanBuf), min, siz );
        tb -= left;
        tf -= left;
        lb -= left;
        slen(scan->scanBuf) -= left;
        right += left;
    }
    
    if( right ) {
        char *to = sstr(scan->scanBuf) + slen(scan->scanBuf);
        ssize_t rsiz = scan->rd( to, right, scan->f );
        if( rsiz < 0 ) {
            return rsiz;
        }
        slen(scan->scanBuf) += rsiz;
        liassert( slen(scan->scanBuf) <= salc(scan->scanBuf) );
        return rsiz;
    }
    
    return 0;
}

/*
============
StoreBufData
============
*/
static void StoreBufData( liScan_t *scan ) {
    if( lb < tb ) {
        if( (size_t)(tb - lb) > 64 ) {
            lb = tb - 64;
        }
    }
    
    if( scan->storage ) {
        scan->storage = LiSCatL( scan->storage, tb, tl );
    } else {
        scan->storage = LiSNewL( tb, tl );
    }
    tb = tf;
    tl = 0;
}

#define CH_EOF  0x7f100000
#define CH_ERD  0x7f200000

/*
============
GetChar
============
*/
static int GetChar( liScan_t *scan ) {
    if( tf >= sstr(scan->scanBuf) + slen(scan->scanBuf) ) {
        ssize_t rsiz = UpdateBuffer( scan );
        if( rsiz < 0 ) {
            /* return reading error */
            return CH_ERD;
        }
        if( rsiz == 0 ) {
            if( salc(scan->scanBuf) == slen(scan->scanBuf) ) {
                /* end of buffer */
                StoreBufData( scan );
                static int lvl = 0;   /* recursion level */
                lvl++;
                liassert( lvl <= 2 ); /* check recursion level */
                int ch = GetChar( scan );
                lvl--;
                return ch;
            }
            /* return end of file */
            return CH_EOF;
        }
    }
    if( !scan->chProc ) {
        if( *tf == '\n' ) {
            ln++;
            lc = 1;
            lb = tf + 1;
        } else {
            lc++;
        }
        scan->chProc = litrue;
    }
    liassert( tf <= sstr(scan->scanBuf) + slen(scan->scanBuf) );
    return *tf;
}

/*
============
GetNextChar
============
*/
static int GetNextChar( liScan_t *scan ) {
    tf++;
    scan->chProc = lifalse;
    return GetChar( scan );
}



#define TK_ERD      0x80000000
#define TK_EOF      0x71000000
#define TK_STR      0x72000000
#define TK_KEY      0x73000000
#define TK_ERR      0x74000000
#define TK_NUM      0x75000000

/*
============
ScanKeyword
============
*/
static int ScanKeyword( liScan_t *scan ) {
    int ch = GetChar( scan );
    liassert( is_firstkeych( (char)ch ) );
    tb = tf;
    tl = 1;
    while( 1 ) {
        ch = GetNextChar( scan );
        if( !is_nextkeych( (char)ch ) ) {
            break;
        }
        tl++;
    }
    /* store string to storage and return */
    StoreBufData( scan );
    return TK_KEY;
}

/*
============
ScanNumber

0x123123        hex (int)
0123            oct (int)
[+/-]123123     dec (int)
[+/-]123.123        (double)
[+/-](nan/inf/inf)  (double)
[+/-]1e[+/-]12           (double)
[+/-](0-9)[.(0-9)]
============
*/
static int ScanNumber( liScan_t *scan ) {
    int ch = GetChar( scan );
    /* if( ch >= '0' && ch <= '9' || ch == '-' || ch == '+' ) {
    */
    return TK_NUM;
}

/*
============
ScanString

"string... \t ...string... \n \t \a" ...
============
*/
static int ScanString( liScan_t *scan ) {
    int ch = GetChar( scan );
    liassert( ch == '"' );
    tb = tf;
    tl = 0;
    while( 1 ) {
        ch = GetNextChar( scan );
        if( ch == '"' ) {
            /* check for end of string character */
            break;
        } else if( ch == '\\' ) {
            /* escape character */
            tl++;
            ch = GetNextChar( scan );
            if( !(ch >= 32 && ch <= 255 && ch != 127) ) {
                /* not printable character */
                /* TODO error message */
                goto goErr;
            }
        } else if( ch >= 32 && ch <= 255 && ch != 127 ) {
            /* if character is printable */
        } else {
            /* handle as error */
            /* check for special characters */
            if( ch == CH_EOF ) {
                /* TODO error message */
                goto goErr;
            } else if( ch == CH_ERD ) {
                /* TODO error message */
                goto goErr;
            } else {
                liasserta( 0, "unknown character" );
            }
        }
        tl++;
    }
    /* skip current char '"' */
    GetNextChar( scan );
    /* store string to storage */
    StoreBufData( scan );
    return TK_STR;
goErr:
    tb = tf;
    tl = 0;
    return TK_ERR;
}

/*
============
ScanToken
============
*/
static int ScanToken( liScan_t *scan ) { 
    int ch = GetChar( scan );
    tl = 0;
    while( 1 ) {
        tb = tf;
        switch( ch ) {
            case CH_EOF:
                /* end of file */
                return tk = TK_EOF;
            case CH_ERD:
                /* reading error */
                return tk = TK_ERD;
            case ' ': case '\t': case '\n': case '\v': case '\f': case '\r':
                /* skip space character */
                break;
            case '"':
                return tk = ScanString( scan );
            default:
                /* check for keyword */
                if( is_firstkeych( (char)ch ) ) {
                    return tk = ScanKeyword( scan );
                }
                /* check for number */
                if( (ch >= '0' && ch <= '9') || (ch == '-') || (ch == '+') ){
                    return tk = ScanNumber( scan );
                }
                /* return character as token */
                tk = ch;
                GetNextChar( scan );
                return tk;
        }
        ch = GetNextChar( scan );
    }
}

#undef tb
#undef tf
#undef tl
#undef tk
#undef lb
#undef ln
#undef lc

/*
============
ParseFile_r
============
*/
static liObj_t *ParseFile_r( liScan_t *scan, liflag_t flags, int level ) {
            
    liverifya( level <= LI_MAX_NESTING_LEVEL,
        "error: the nesting level is too high. "
        "increase the constant LI_MAX_NESTING_LEVEL. "
        "LI_MAX_NESTING_LEVEL=%d", LI_MAX_NESTING_LEVEL );

    return NULL;
}

/*
============
LiRead
============
*/
licode_t LiRead( liObj_t **o, const char *name ) {
    liassert(o);
    return LiReadEx( NULL, o, name, 0, NULL, 0 );
}

#include <stdio.h>
#include <stdlib.h>
/*
============
LiReadEx
============
*/
licode_t LiReadEx( liIO_t *io, liObj_t **o, const char *name, 
                    liflag_t flags, char *errbuf, size_t errbufLen ) {
    liassert(o);
    liassert(*o == NULL);
    liverifya( (errbuf && (errbufLen >= 1024)) || 
            (!errbuf && (errbufLen == 0)),
            "error: the buffer size must be at least 1024" );
    
    liFile_t f;
    licode_t code = LI_OK;
    liScan_t scan;
    
    if( io == NULL ) {
        extern liIO_t liDefaultIO;
        io = &liDefaultIO;
    }
    
    f = io->open( name, 'r' );
    if( f == NULL ) {
        return LI_EFILEOPEN;
    }
    ScanInit( &scan, f, io->read );
    *o = ParseFile_r( &scan, flags, 0 );
    
    
    size_t rd;
    FILE *file = fopen( "out.txt", "wb" );
    
    liStr_t *s = NULL;
    int ch = GetChar( &scan );
    char *prev = scan.tkBeg;
    while( ch != CH_EOF && ch != CH_ERD ) {
        scan.tkLen++;
        char c = (char)ch;
        fwrite( &c, 1, 1, file );
        ch = GetNextChar( &scan );
    }
    
    if( ch == CH_EOF ) {
        Log( "CH_EOF\n" );
    } else if( ch == CH_ERD ) {
        Log( "CH_ERD\n" );
    }
    
    s = scan.storage;
    scan.storage = NULL;
    
    LiSFree(s);
    
    fclose( file );
    
    ScanFree( &scan );
    io->close( f );
    
    return code;
}

