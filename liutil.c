#include "liutil.h"
#include "liassert.h"
#include "li.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>



/*
============
__impl_liasserta_verifya
============
*/
void __impl_liasserta_verifya( const char *failed, int e, const char* expr,
    const char* file, int line, const char *function, const char* fmt, ... ) {
    
    va_list args;
    if( !e ){
        va_start( args, fmt );
        fprintf( stderr, "%s%s\n", failed, expr );
        fprintf( stderr, "file: %s\n", file );
        fprintf( stderr, "line: %d\n", line );
        fprintf( stderr, "function: %s\n", function );
        if( fmt ) {
            vfprintf( stderr, fmt, args );
        }
        va_end( args );
        exit(1);
    }
}



/*
================================================
                  li allocator
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

liAlloc_t liDefaultAllocator = { 
    LiDefaultAlloc,
    LiDefaultRealloc,
    LiDefaultFree
};



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

liIO_t liDefaultIO = {
    LiDefaultOpen,
    LiDefaultClose,
    LiDefaultRead,
    LiDefaultWrite
};



/*
============
CeilPow2
============
*/
lisize_t CeilPow2( lisize_t v ) {
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
#if defined(LI_SIZETYPE_64BIT)
    v |= v >> 32;
#endif    
    v++;
    return v;
}

/*
============
UInt64ToStr
============
*/
size_t UInt64ToStr( uint64_t val, char *str, int base ) {
    static const char alpha[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char *p = str;
    uint8_t digits[64]; /* value digits */
    int dig = 0;        /* amount of digits */
    
    liassert( base >= 2 );
    liassert( base <= 36 );
    
    /* calculate digits */
    do {
        digits[dig++] = (uint8_t)(val % base);
        val /= base;
    } while( val );
    
    /* copy the flipped digits to the output buffer */
    do {
        *p++ = alpha[ digits[--dig] ];
    } while( dig > 0 );
    *p = 0;
    
    return (size_t)(p - str);
}

/*
============
Int64ToStr
============
*/
size_t Int64ToStr( int64_t val, char *str, int base ) {
    liassert( base >= 2 );
    liassert( base <= 36 );
    
    /* check for negative value */
    if( val < 0 ) {
        *str++ = '-';
        return UInt64ToStr( (uint64_t)(-val), str, base ) + 1;
    }
    return UInt64ToStr( (uint64_t)val, str, base );
}

/*
============
__charis
============
*/
libool_t __charis( char c, uint8_t flag ) {
    uint8_t i = *((uint8_t*)(void*)(&c));
    static const char flags[256] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03,
        0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    
    return !!(flags[i] & flag);
}

/*
============
StrLen
============
*/
size_t StrLen( const char *s ) {
    return strlen(s);
}

/*
============
MemCpy
============
*/
void *MemCpy( void *dst, const void *src, size_t num ) {
    return memcpy( dst, src, num );
}

/*
============
Log
============
*/
int Log( const char *fmt, ... ) {
    int ret;
    va_list args;
    va_start( args, fmt );
    ret = fprintf( stdout, "[log] " );
    ret += vfprintf( stdout, fmt, args );
    va_end( args );
    return ret;
}

/*
============
SPrint
============
*/
liStr_t *SPrint( liStr_t *s, const char *fmt, ... ) {
    const uint32_t checkSize = 1024 * 2;
    if( !s ) {
        s = LiSAlloc( checkSize + 1024 * 2 );
    }
    /* check available free size */
    if( salc(s) - slen(s) < checkSize ) {
        s = LiSRealloc( s, salc(s) + checkSize );
    }
    
    va_list args;
    va_start( args, fmt );
    
    int len = vsprintf( sstr(s) + slen(s), fmt, args );
    liverify(len < checkSize);
    slen(s) += len;
    sstr(s)[slen(s)] = 0;
    
    va_end( args );
    
    return s;
}