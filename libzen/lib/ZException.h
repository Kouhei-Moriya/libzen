#include <setjmp.h>

#ifndef _DEFINE_ZEXCEPTION
#define _DEFINE_ZEXCEPTION

#define NO  0
#define YES 1

#define TRY     0
#define CATCH   1
#define FINALLY 2

#define STACK_MAX 256

struct try_buf {
	jmp_buf jbuf;
	char currentBlock;
	char isExceptionThrown;
	const void *exception;	
};


void ThrowException(const void *exception);
void *GetException();
void *StartTry();
void StartFinally();
void EndTry();

#endif //_DEFINE_ZEXCEPTION
