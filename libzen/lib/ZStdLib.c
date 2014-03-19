#include <stdio.h>
#include <stdlib.h>

#include "ZStdLib.h"

void ZStdLib_assert(char expr) {
	if(expr == 0) {
		fprintf(stderr, "Assertion Failure\n");
		exit(EXIT_FAILURE);
	}
}

