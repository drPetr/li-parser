#ifndef __LITYPES_H__
#define __LITYPES_H__

#include <stdint.h>

/*#define LI_SIZETYPE_64BIT*/

/* litypes */
typedef uint32_t        lityid_t;
typedef uint32_t        litype_t;
typedef uint32_t        libool_t;
typedef uint32_t        licode_t;
typedef uint32_t        liflag_t;
#if defined(LI_SIZETYPE_64BIT)
    typedef uint64_t    lisize_t;
#else
    typedef uint32_t    lisize_t;
#endif



/* lityid */
#define LI_TYID_STR     1
#define LI_TYID_NODE    2
#define LI_TYID_ARR     3

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
#define LI_EINPDAT      ((licode_t)2)
#define LI_FINISHED     ((licode_t)3)

/* liflag */
#define LI_FDEC         0x0000
#define LI_FOCT         0x0001
#define LI_FBIN         0x0002
#define LI_FHEX         0x0003
#define LI_FBASE_MASK   0x0003
#define LI_FSIGN        0x0004

/* unused variavle macro */
#define liunused(a)     ((void)a)

#define liptrdiff(max,min)      (lisize_t)((max)-(min))

#endif //__LITYPES_H__