#ifndef __LIASSERT_H__
#define __LIASSERT_H__

void __impl_liasserta_verifya( const char *failed, int e, const char* expr,
    const char* file, int line, const char *function, const char* fmt,... );

/* liverify */
#define liverify(e) \
    __impl_liasserta_verifya( "verification failed: ", (int)(!!(e)), #e, \
    __FILE__, __LINE__, __FUNCTION__ , NULL )

/* liverifya */
#define liverifya(e,a,...)  \
    __impl_liasserta_verifya( "verification failed: ",  \
    (int)(!!(e)), #e, __FILE__, __LINE__, __FUNCTION__ , a, ##__VA_ARGS__ )

/* assertion macros */
#if defined(LI_NODBG) || (!defined(DEBUG))
    #define onlydebug(act)
    #define liassert(e)
    #define liasserta(e,a,...)
#else
    #define onlydebug(act)  act

/* liassert */
    #define liassert(e) \
        __impl_liasserta_verifya( "assertion failed: ", \
    (int)(!!(e)), #e, __FILE__, __LINE__, __FUNCTION__ , NULL )
    
/* liasserta */    
    #define liasserta(e,a,...)  \
        __impl_liasserta_verifya( "assertion failed: ", (int)(!!(e)),   \
        #e, __FILE__, __LINE__, __FUNCTION__ , a, ##__VA_ARGS__ )

#endif

#endif //__LIASSERT_H__