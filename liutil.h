#ifndef __LIUTIL_H__
#define __LIUTIL_H__

#include "litypes.h"
#include "listr.h"

lisize_t    CeilPow2( lisize_t v );
size_t      UInt64ToStr( uint64_t val, char *str, int base );
size_t      Int64ToStr( int64_t val, char *str, int base );

libool_t    __charis( char c, uint8_t flag );

size_t      StrLen( const char *s );
void        *MemCpy( void *dst, const void *src, size_t num );

#define     is_space(c)         __charis( c, 0x04 )
#define     is_firstkeych(c)    __charis( c, 0x02 )
#define     is_nextkeych(c)     __charis( c, 0x01 )

int         Log( const char *fmt, ... );

liStr_t     *SPrint( liStr_t *s, const char *fmt, ... );

int         SPrintf( char *s, const char *fmt, ... );

#endif //__LIUTIL_H__