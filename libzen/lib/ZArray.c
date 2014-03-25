#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <gc.h>

#include "ZStdLib.h"
#include "ZArray.h"


// -----------------
//  general purpose
// -----------------
ZArray *ZArray_new(size_t bufsize) {
	ZArray *ZArr = (ZArray *)GC_malloc(ZArray_size);
	if(ZArr == NULL) {
		exit(EXIT_FAILURE);
	}
	ZArr->bufsize = (uint64_t)bufsize;
	ZArr->len = 0;
	ZArr->elements = NULL;

	if(bufsize > 0) {
		void *o = GC_malloc(bufsize);
		memset(o, 0, bufsize);
		if(o == NULL) {
			exit(EXIT_FAILURE);
		}
		ZArr->elements = o;
	}
	return ZArr;
}

ZArray *ZArray_Clone(ZArray *ZArr) {
	size_t len = (size_t)ZArr->len;
	ZArray *RetZArr = ZArray_new(len);
	memcpy(RetZArr->elements, ZArr->elements, len);
	RetZArr->len = (uint64_t)len;
	return RetZArr;
}

//destructive
void ZArray_EnsureSize(ZArray *ZArr, size_t bufsize) {
	size_t currentsize = (size_t)ZArr->bufsize;
	if(currentsize < bufsize) {
		void *newarr = GC_malloc(bufsize);
		memset(newarr, 0, bufsize);
		if(newarr == NULL) {
			exit(EXIT_FAILURE);
		}
		memcpy(newarr, ZArr->elements, currentsize);
		ZArr->bufsize = bufsize;
		ZArr->elements = newarr;
	}
	return;
}

ZArray *ZArray_Construct(const void *arr, int64_t len, size_t elmsize) {
	size_t bufsize = elmsize * (size_t)len;
	ZArray *ZArr = ZArray_new(bufsize);
	memcpy(ZArr->elements, arr, bufsize);
	ZArr->len = (uint64_t)bufsize;
	return ZArr;
}

int64_t ZArray_Length(ZArray *ZArr, size_t elmsize) {
	return (int64_t)((size_t)ZArr->len / elmsize);
}

void *ZArray_Get(ZArray *ZArr, int64_t index, size_t elmsize) {
	if(index < 0) {
		exit(EXIT_FAILURE);
	}
	size_t bufindex = (size_t)index * elmsize;

	if((size_t)ZArr->len > bufindex) {
		return ZArr->elements + bufindex;
	} else {
		exit(EXIT_FAILURE);
	}
}

//destructive
void ZArray_Set(ZArray *ZArr, int64_t index, void *setelement, size_t elmsize) {
	if(index < 0) {
		exit(EXIT_FAILURE);
	}
	size_t bufindex = (size_t)index * elmsize;
	size_t len = (size_t)ZArr->len;

	ZArray_EnsureSize(ZArr, bufindex + elmsize);
	memcpy(ZArr->elements + bufindex, setelement, elmsize);
	if(len <= bufindex) {
		ZArr->len = (uint64_t)(bufindex + elmsize);
	}
}

//destructive
void ZArray_Add(ZArray *ZArr, void *addelement, size_t elmsize) {
	size_t len = (size_t)ZArr->len;
	ZArray_EnsureSize(ZArr, len + elmsize);

	memcpy(ZArr->elements + len, addelement, elmsize);
	ZArr->len = (uint64_t)(len + elmsize);
}

//destructive
void ZArray_Insert(ZArray *ZArr, int64_t index, void *addelement, size_t elmsize) {
	if(index < 0) {
		exit(EXIT_FAILURE);
	}
	size_t bufindex = (size_t)index * elmsize;
	size_t len = (size_t)ZArr->len;
	if(len < bufindex) {
		ZArray_Set(ZArr, index, addelement, elmsize);
	} else if(len == bufindex) {
		ZArray_Add(ZArr, addelement, elmsize);
	} else {
		ZArray_EnsureSize(ZArr, len + elmsize);
	
		memmove(ZArr->elements + bufindex + elmsize, ZArr->elements + bufindex, len - bufindex);
		memcpy(ZArr->elements + bufindex, addelement, elmsize);
		ZArr->len = (uint64_t)(len + elmsize);
	}
}

//destructive
void ZArray_Clear(ZArray *ZArr, int64_t index, size_t elmsize) {
	size_t len = (size_t)ZArr->len;
	size_t bufindex = (size_t)index * elmsize;
	if(len > bufindex) {
		ZArray_EnsureSize(ZArr, len);
	
		memmove(ZArr->elements + bufindex, ZArr->elements + bufindex + elmsize, len - bufindex - elmsize);
		ZArr->len = (uint64_t)(len - elmsize);
	}
}

int64_t ZArray_IndexOf(ZArray *ZArr, int64_t startindex, ZArray *SearchArr, size_t elmsize) {
	void *elements = ZArr->elements;
	size_t len = (size_t)ZArr->len;
	void *searchelements = SearchArr->elements;
	size_t searchelmlen = (size_t)SearchArr->len;
	if(searchelmlen == 0) {
		return -1;
	}

	size_t i = 0;
	if(startindex >= 0) {
		i = (size_t)startindex * elmsize;
	}
	for(; i <= len - searchelmlen; i += elmsize) {
		if(memcmp(elements + i, searchelements, searchelmlen) == 0) {
			return (int64_t)(i / elmsize);
		}
	}
	return -1;
}


// -------------------
//  method for String
// -------------------
char ZArray_Equals(ZArray *ZArr1, ZArray *ZArr2) {
	uint64_t len = ZArr2->len;
	if(ZArr1->len == len) {
		if(memcmp(ZArr1->elements, ZArr2->elements, (size_t)len) == 0) {
			return BOOL_TRUE;
		}
	}
	return BOOL_FALSE;
}
char ZArray_StartsWith(ZArray *ZArr1, ZArray *ZArr2) {
	uint64_t len = ZArr2->len;
	if(ZArr1->len >= len) {
		if(memcmp(ZArr1->elements, ZArr2->elements, (size_t)len) == 0) {
			return BOOL_TRUE;
		}
	}
	return BOOL_FALSE;
}
char ZArray_EndsWith(ZArray *ZArr1, ZArray *ZArr2) {
	uint64_t len1 = ZArr1->len;
	uint64_t len2 = ZArr2->len;
	if(len1 >= len2) {
		if(memcmp(ZArr1->elements + len1 - len2, ZArr2->elements, (size_t)len2) == 0) {
			return BOOL_TRUE;
		}
	}
	return BOOL_FALSE;
}

ZArray *ZArray_ConCat(ZArray *ZArr1, ZArray *ZArr2) {
	size_t len1 = (size_t)ZArr1->len;
	size_t len2 = (size_t)ZArr2->len;
	ZArray *RetZArr = ZArray_new(len1 + len2);

	memcpy(RetZArr->elements       , ZArr1->elements, len1);
	memcpy(RetZArr->elements + len1, ZArr2->elements, len2);
	RetZArr->len = (uint64_t)(len1 + len2);
	return RetZArr;
}

ZArray *ZArray_SubArray(ZArray *ZArr, int64_t startindex, int64_t endindex, size_t elmsize) {
	size_t startbufindex = 0;
	if(startindex >= 0) {
		startbufindex = (size_t)startindex * elmsize;
	}
	size_t endbufindex = (size_t)ZArr->len;
	int64_t arrsize = ZArray_Length(ZArr, elmsize);
	if(startindex < endindex && endindex <= arrsize) {
		endbufindex = (size_t)endindex * elmsize;
	}
	size_t newlen = endbufindex - startbufindex;	

	ZArray *RetZArr = ZArray_new(newlen);
	memcpy(RetZArr->elements, ZArr->elements + startbufindex, newlen);
	RetZArr->len = (uint64_t)newlen;
	return RetZArr;
}


// -----------
//  int Array
// -----------
ZArray *ZIntArray_Construct(const void *arr, int64_t len) {
	return ZArray_Construct(arr, len, SIZE_I64);
}
int64_t ZIntArray_Length(ZArray *ZArr) {
	return ZArray_Length(ZArr, SIZE_I64);
}
int64_t ZIntArray_Get(ZArray *ZArr, int64_t index) {
	return *((int64_t *)ZArray_Get(ZArr, index, SIZE_I64));
}
//destructive
void ZIntArray_Set(ZArray *ZArr, int64_t index, int64_t setelement) {
	ZArray_Set(ZArr, index, (void *)&setelement, SIZE_I64);
}
//destructive
void ZIntArray_Add(ZArray *ZArr, int64_t addelement) {
	ZArray_Add(ZArr, (void *)&addelement, SIZE_I64);
}
//destructive
void ZIntArray_Insert(ZArray *ZArr, int64_t index, int64_t addelement) {
	ZArray_Insert(ZArr, index, (void *)&addelement, SIZE_I64);
}
//destructive
void ZIntArray_Clear(ZArray *ZArr, int64_t index) {
	ZArray_Clear(ZArr, index, SIZE_I64);
}
int64_t ZIntArray_IndexOf_withIndex(ZArray *ZArr, int64_t startindex, int64_t searchvalue) {
	ZArray SearchArr = {0, SIZE_I64, (void *)&searchvalue};
	return ZArray_IndexOf(ZArr, startindex, &SearchArr, SIZE_I64);
}
int64_t ZIntArray_IndexOf(ZArray *ZArr, int64_t searchvalue) {
	return ZIntArray_IndexOf_withIndex(ZArr, 0, searchvalue);
}


// -------------
//  float Array
// -------------
ZArray *ZFloatArray_Construct(const void *arr, int64_t len) {
	return ZArray_Construct(arr, len, SIZE_DOUBLE);
}
int64_t ZFloatArray_Length(ZArray *ZArr) {
	return ZArray_Length(ZArr, SIZE_DOUBLE);
}
double ZFloatArray_Get(ZArray *ZArr, int64_t index) {
	return *((double *)ZArray_Get(ZArr, index, SIZE_DOUBLE));
}
//destructive
void ZFloatArray_Set(ZArray *ZArr, int64_t index, double setelement) {
	ZArray_Set(ZArr, index, (void *)&setelement, SIZE_DOUBLE);
}
//destructive
void ZFloatArray_Add(ZArray *ZArr, double addelement) {
	ZArray_Add(ZArr, (void *)&addelement, SIZE_DOUBLE);
}
//destructive
void ZFloatArray_Insert(ZArray *ZArr, int64_t index, double addelement) {
	ZArray_Insert(ZArr, index, (void *)&addelement, SIZE_DOUBLE);
}
//destructive
void ZFloatArray_Clear(ZArray *ZArr, int64_t index) {
	ZArray_Clear(ZArr, index, SIZE_DOUBLE);
}
int64_t ZFloatArray_IndexOf_withIndex(ZArray *ZArr, int64_t startindex, double searchvalue) {
	ZArray SearchArr = {0, SIZE_DOUBLE, (void *)&searchvalue};
	return ZArray_IndexOf(ZArr, startindex, &SearchArr, SIZE_DOUBLE);
}
int64_t ZFloatArray_IndexOf(ZArray *ZArr, double searchvalue) {
	return ZFloatArray_IndexOf_withIndex(ZArr, 0, searchvalue);
}


// ---------------
//  boolean Array
// ---------------
ZArray *ZBooleanArray_Construct(const void *arr, int64_t len) {
	return ZArray_Construct(arr, len, SIZE_BOOL);
}
int64_t ZBooleanArray_Length(ZArray *ZArr) {
	return ZArray_Length(ZArr, SIZE_BOOL);
}
char ZBooleanArray_Get(ZArray *ZArr, int64_t index) {
	return *((char *)ZArray_Get(ZArr, index, SIZE_BOOL));
}
//destructive
void ZBooleanArray_Set(ZArray *ZArr, int64_t index, char setelement) {
	ZArray_Set(ZArr, index, (void *)&setelement, SIZE_BOOL);
}
//destructive
void ZBooleanArray_Add(ZArray *ZArr, char addelement) {
	ZArray_Add(ZArr, (void *)&addelement, SIZE_BOOL);
}
//destructive
void ZBooleanArray_Insert(ZArray *ZArr, int64_t index, char addelement) {
	ZArray_Insert(ZArr, index, (void *)&addelement, SIZE_BOOL);
}
//destructive
void ZBooleanArray_Clear(ZArray *ZArr, int64_t index) {
	ZArray_Clear(ZArr, index, SIZE_BOOL);
}
int64_t ZBooleanArray_IndexOf_withIndex(ZArray *ZArr, int64_t startindex, char searchvalue) {
	ZArray SearchArr = {0, SIZE_BOOL, (void *)&searchvalue};
	return ZArray_IndexOf(ZArr, startindex, &SearchArr, SIZE_BOOL);
}
int64_t ZBooleanArray_IndexOf(ZArray *ZArr, char searchvalue) {
	return ZBooleanArray_IndexOf_withIndex(ZArr, 0, searchvalue);
}


// --------------
//  Object Array
// --------------
ZArray *ZObjArray_Construct(const void *arr, int64_t len) {
	return ZArray_Construct(arr, len, SIZE_POINTER);
}
int64_t ZObjArray_Length(ZArray *ZArr) {
	return ZArray_Length(ZArr, SIZE_POINTER);
}
void *ZObjArray_Get(ZArray *ZArr, int64_t index) {
	return *((void **)ZArray_Get(ZArr, index, SIZE_POINTER));
}
//destructive
void ZObjArray_Set(ZArray *ZArr, int64_t index, void *setelement) {
	ZArray_Set(ZArr, index, (void *)&setelement, SIZE_POINTER);
}
//destructive
void ZObjArray_Add(ZArray *ZArr, void *addelement) {
	ZArray_Add(ZArr, (void *)&addelement, SIZE_POINTER);
}
//destructive
void ZObjArray_Insert(ZArray *ZArr, int64_t index, void *addelement) {
	ZArray_Insert(ZArr, index, (void *)&addelement, SIZE_POINTER);
}
//destructive
void ZObjArray_Clear(ZArray *ZArr, int64_t index) {
	ZArray_Clear(ZArr, index, SIZE_POINTER);
}
int64_t ZObjArray_IndexOf_withIndex(ZArray *ZArr, int64_t startindex, void *SearchObj) {
	ZArray SearchArr = {0, SIZE_POINTER, SearchObj};
	return ZArray_IndexOf(ZArr, startindex, &SearchArr, SIZE_POINTER);
}
int64_t ZObjArray_IndexOf(ZArray *ZArr, void *SearchObj) {
	return ZObjArray_IndexOf_withIndex(ZArr, 0, SearchObj);
}


// --------
//  String
// --------
ZString *ZString_Construct(const char *arr, int64_t len) {
	return (ZString *)ZArray_Construct((const void *)arr, len, SIZE_CHAR);
}
int64_t ZString_StrLen(ZString *ZStr) {
	return ZArray_Length((ZArray *)ZStr, SIZE_CHAR);
}
ZString *ZString_Get(ZString *ZStr, int64_t index) {
	char *character = (char *)ZArray_Get((ZArray *)ZStr, index, SIZE_CHAR);
	return ZString_Construct(character, 1);
}
/* //destructive
void ZString_Set(ZString *ZStr, int64_t index, char setelement) {
	ZArray_Set((ZArray *)ZStr, index, (void *)&setelement, SIZE_CHAR);
}
//destructive
void ZString_Add(ZString *ZStr, char addelement) {
	ZArray_Add((ZArray *)ZStr, (void *)&addelement, SIZE_CHAR);
}
//destructive
void ZString_Insert(ZString *ZStr, int64_t index, char addelement) {
	ZArray_Insert((ZArray *)ZStr, index, (void *)&addelement, SIZE_CHAR);
} */
int64_t ZString_IndexOf_withIndex(ZString *ZStr, int64_t startindex, ZString *SearchStr) {
	return ZArray_IndexOf((ZArray *)ZStr, startindex, (ZArray *)SearchStr, SIZE_CHAR);
}
int64_t ZString_IndexOf(ZString *ZStr, ZString *SearchStr) {
	return ZString_IndexOf_withIndex(ZStr, 0, SearchStr);
}

#define BUFSIZE 24
ZString *ZString_int_toString(int64_t number) {
	char buf[BUFSIZE]; //maxlen "-9223372036854775808" = 20+1
	int len = snprintf(buf, BUFSIZE, "%lld", (long long)number);
	if(len != EOF) {
		return ZString_Construct(buf, (size_t)len);
	} else {
		return (ZString *)ZArray_new(0);
	}
}
ZString *ZString_float_toString(double number) {
	char buf[BUFSIZE];
	int len = snprintf(buf, BUFSIZE, "%lf", number);
	if(len != EOF) {
		return ZString_Construct(buf, (size_t)len);
	} else {
		return (ZString *)ZArray_new(0);
	}
}
ZString *ZString_boolean_toString(char boolean) {
	if(boolean & (char)1) {
		return ZString_Construct("true", 4);
	} else {
		return ZString_Construct("false", 5);
	}
}
ZString *ZString_NullString() {
	return ZString_Construct("null", 4);
}

char ZString_EqualString(ZString *ZStr1, ZString *ZStr2) {
	return ZArray_Equals((ZArray *)ZStr1, (ZArray *)ZStr2);
}
char ZString_StartsWith(ZString *ZStr1, ZString *ZStr2) {
	return ZArray_StartsWith((ZArray *)ZStr1, (ZArray *)ZStr2);
}
char ZString_EndsWith(ZString *ZStr1, ZString *ZStr2) {
	return ZArray_EndsWith((ZArray *)ZStr1, (ZArray *)ZStr2);
}
ZString *ZString_StrCat(ZString *ZStr1, ZString *ZStr2) {
	ZString *LeftZStr = (ZArray *)ZStr1;
	if(LeftZStr == NULL) {
		LeftZStr = ZString_NullString();
	}
	ZString *RightZStr = (ZArray *)ZStr2;
	if(RightZStr == NULL) {
		RightZStr = ZString_NullString();
	}	
	return (ZString *)ZArray_ConCat(LeftZStr, RightZStr);
}
ZString *ZString_SubString_withEndIndex(ZString *ZStr, int64_t startindex, int64_t endindex) {
	return (ZString *)ZArray_SubArray((ZArray *)ZStr, startindex, endindex, SIZE_CHAR);
}
ZString *ZString_SubString(ZString *ZStr, int64_t startindex) {
	return ZString_SubString_withEndIndex(ZStr, startindex, ZString_StrLen(ZStr));
}

void ZString_print(ZString *ZStr) {
	size_t len = (size_t)ZStr->len;
	char buf[len + 1];
	memcpy((void *)buf, ZStr->elements, len);
	buf[len] = '\0';
	printf("%s", buf);
}
void ZString_println(ZString *ZStr) {
	size_t len = (size_t)ZStr->len;
	char buf[len + 1];
	memcpy((void *)buf, ZStr->elements, len);
	buf[len] = '\0';
	puts(buf);
}
void ZString_printerr(ZString *ZStr) {
	size_t len = (size_t)ZStr->len;
	char buf[len + 1];
	memcpy((void *)buf, ZStr->elements, len);
	buf[len] = '\0';
	fprintf(stderr, "%s\n", buf);
}

