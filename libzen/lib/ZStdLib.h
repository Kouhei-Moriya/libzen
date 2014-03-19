#ifndef _DEFINE_ZSTDLIB
#define _DEFINE_ZSTDLIB

#define SIZE_I64 8
#define SIZE_DOUBLE 8
#define SIZE_BOOL 1
#define SIZE_CHAR 1

#if defined(__LP64__) || defined(_WIN64) /* In case of 64 bit system */
#define SIZE_POINTER 8

#else
#define SIZE_POINTER 4

#endif

#define BOOL_TRUE  ((char)-1)
#define BOOL_FALSE ((char) 0)

void ZStdLib_assert(char expr);

#endif //_DEFINE_ZSTDLIB

