#include "li.h"
#include "liassert.h"
#include "liutil.h"



/*
================================================
        li object insert/extract functions
================================================
*/

/*
============
LiInsertionHelper
============
*/
static void LiInsertionHelper( liObj_t *left, liObj_t *right, 
        liObj_t *parent, liObj_t *insert ) {
    /* first node in the insert sequence */
    liObj_t *firstInsert = insert;
    /* last node in the insert sequence */
    liObj_t *lastInsert = insert;
    
    liassert( left || right || parent );
    liassert( insert );

#if !defined(LI_NODBG) && defined(DEBUG)
    liassert( insert != parent );
    liassert( insert != left );
    liassert( insert != right );
    liassert( insert->parent == NULL );
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

    /* set the whole sibling sequence of the parent and get a
    pointer to the last node of the sibling sequence */
    insert->parent = parent;
    while( lastInsert->next ) {
        lastInsert = lastInsert->next;
        lastInsert->parent = parent;
    }
    
    while( firstInsert->prev ) {
        firstInsert = firstInsert->prev;
        firstInsert->parent = parent;
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
void LiInsertFirstChild( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type == LI_VTOBJ );
    liassert( insert->type != LI_VTUNDEF );
#if !defined(LI_NODBG) && defined(DEBUG)
    liObj_t *o = insert;
    while( o->prev ) {
        o = o->prev;
    }
    liassert( o->key != NULL );
#endif
    LiInsertionHelper( NULL, node->firstChild, node, insert );
}

/*
============
LiInsertLastChild

Insert the insert node as the last child of the node
============
*/
void LiInsertLastChild( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type == LI_VTOBJ );
    liassert( insert->type != LI_VTUNDEF );
#if !defined(LI_NODBG) && defined(DEBUG)
    liObj_t *o = insert;
    while( o->prev ) {
        o = o->prev;
    }
    if( node->lastChild == NULL ) {
        liassert( o->key != NULL );
    }
#endif
    LiInsertionHelper( node->lastChild, NULL, node, insert );
}

/*
============
LiInsertBefore

Insert an insert node before the node
============
*/
void LiInsertBefore( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type != LI_VTUNDEF );
    liassert( insert->type != LI_VTUNDEF );
    LiInsertionHelper( node->prev, node, node->parent, insert );
}

/*
============
LiInsertAfter

Insert an insert node after a node
============
*/
void LiInsertAfter( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type != LI_VTUNDEF );
    liassert( insert->type != LI_VTUNDEF );
    LiInsertionHelper( node, node->next, node->parent, insert );
}

/*
============
LiInsertFirst

Insert the insert node first in the sequence node
============
*/
void LiInsertFirst( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type != LI_VTUNDEF );
    liassert( insert->type != LI_VTUNDEF );
    
    if( node->parent ) {
        node = node->parent->firstChild;
    } else {
        while( node->prev ) {
            node = node->prev;
        }
    }
    LiInsertionHelper( NULL, node, node->parent, insert );
}

/*
============
LiInsertLast

Insert the insert node as the last node in the sequence
============
*/
void LiInsertLast( liObj_t *node, liObj_t *insert ) {
    liassert( node );
    liassert( insert );
    liassert( node->type != LI_VTUNDEF );
    liassert( insert->type != LI_VTUNDEF );
    
    if( node->parent ) {
        node = node->parent->lastChild;
    } else {
        while( node->next ) {
            node = node->next;
        }
    }
    LiInsertionHelper( node, NULL, node->parent, insert );
}

/*
============
LiExtractionHelper
============
*/
static liObj_t *LiExtractionHelper( liObj_t *left, liObj_t *right ) {
    liObj_t *ext = NULL;
    liObj_t *it;
    liObj_t *parent;
    liObj_t *before;
    liObj_t *after;
    
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
liObj_t *LiExtract( liObj_t *node ) {
    liassert( node );
    return LiExtractionHelper( node, node );
}

/*
============
LiExtractSiblings
============
*/
liObj_t *LiExtractSiblings( liObj_t *left, liObj_t *right ) {
    liassert( left || right );
    return LiExtractionHelper( left, right );
}

/*
============
LiExtractChildren
============
*/
liObj_t *LiExtractChildren( liObj_t *node ) {
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
liObj_t *LiNodeCreate( void ) {
    liObj_t *node;
    
    node = (liObj_t*)LiAlloc( sizeof(liObj_t), LI_TYID_NODE );
    
    node->parent = NULL;
    node->next = NULL;
    node->prev = NULL;
    node->firstChild = NULL;
    node->lastChild = NULL;
    node->ptr = NULL;
    node->key = NULL;
    node->type = LI_VTUNDEF;
    node->flags = 0;
    
    return node;
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
            case LI_VTUNDEF:
                break;
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
    node = LiExtract( node );
    liassert( node );
    LiNodeFreeSubtreeHelper_r( node, 0 );
}

/*
============
LiFree
============
*/
void LiFree( liObj_t *li ) {
    liassert( li );
    
    while( li->parent ) {
        li = li->parent;
    }
    LiNodeFreeSubtreeHelper_r( li, 0 );
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
LiObj
============
*/
liObj_t *LiObj( void ) {
    liObj_t *o = LiNodeCreate();
    o->type = LI_VTOBJ;
    return o;
}

/*
============
LiNull
============
*/
liObj_t *LiNull( void ) {
    liObj_t *o = LiNodeCreate();
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
    liObj_t *o = LiNodeCreate();
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
    liObj_t *o = LiNodeCreate();
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
    liObj_t *o = LiNodeCreate();
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
    liObj_t *o = LiNodeCreate();
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
    if( (anum(dat->pattern) == 1) && o->key && (0 == LiSCmpL( o->key, 
            pattern->str, slen(o->key) )) ) {
        dat->obj = o;
        return LI_OK;
    }
    
    return LiFindNext( dat );
}

liObj_t *SkipToNext( liFindData_t *dat, liObj_t *o, int toDown ) {
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
    
    /* fix for unnamed objects */
    if( dat->index == (anum(dat->pattern) - 1) && o && !o->key ) {
        dat->obj = o;
        return LI_OK;
    }
    
    /* find next */
    struct patternData_s *pattern;
    while( o ) {
        pattern = ((struct patternData_s*)aarr(dat->pattern)) + dat->index;
        
       // Log( "%s\n", (o->key ? sstr(o->key) : "(no key)" ) );
        
        if( o->key && (0 == LiSCmpL(o->key, pattern->str, slen(o->key))) ){
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
LiWriteHelper_r
============
*/
static licode_t LiWriteHelper_r( liFile_t f, fnLiWrite wr, 
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
        if( o->key ) {
            LiWriteLiStr( f, wr, o->key );
            LiWriteCstr( f, wr, " = " );
        } else {
            LiWriteCstr( f, wr, ", " );
        }
        if( o->next && (o->next->key == NULL) ) {
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
                    code = LiWriteHelper_r( f, wr, o->firstChild, 
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
licode_t LiWrite( liIO_t *io, liObj_t *o, const char *name, liflag_t flags ) {
    liassert( o );
    
    liFile_t f;
    licode_t code = LI_OK;
    
    if( io == NULL ) {
        extern liIO_t liDefaultIO;
        io = &liDefaultIO;
    }
    
    f = io->open( name, 'w' );
    if( o ) {
        code = LiWriteHelper_r( f, io->write, o, flags, 0 );
    }
    io->close( f );
    
    return code;
}
