#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#include <gc.h>

#include "throw.h"

//------global var-------
struct try_buf trystack[STACK_MAX] = {};
int stackcounter = -1;
//-----------------------

void ThrowException(const void *exception) {
	if(stackcounter >= 0 && trystack[stackcounter].currentBlock == TRY) {
		trystack[stackcounter].exception = exception;
		trystack[stackcounter].currentBlock = CATCH;
		longjmp(trystack[stackcounter].jbuf, CATCH);
	} else if(stackcounter > 0 && trystack[stackcounter].currentBlock == CATCH) {
		trystack[stackcounter].exception = exception;
		trystack[stackcounter].isExceptionThrown = YES;
		longjmp(trystack[stackcounter].jbuf, FINALLY);
	} else if(stackcounter >= 0 && trystack[stackcounter].currentBlock == FINALLY) {
		--stackcounter;
		ThrowException(exception);
	} else {
		fprintf(stderr, "Error: %s\n", exception);
		exit(EXIT_FAILURE);
	}
}
const void *GetException() {
	return trystack[stackcounter].exception;
}

void *StartTry() {
	memset(trystack + (++stackcounter), 0, sizeof(struct try_buf));
	return (void *)trystack[stackcounter].jbuf;
}
void StartFinally() {
	trystack[stackcounter].currentBlock = FINALLY;
}
void EndTry() {
	if(trystack[stackcounter].isExceptionThrown) {
		const void *exception = trystack[stackcounter--].exception;
		ThrowException(exception);
	} else {
		--stackcounter;
	}
}

