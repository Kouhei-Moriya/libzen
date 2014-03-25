#ifndef _DEFINE_ZARRAY
#define _DEFINE_ZARRAY

typedef struct {
	uint64_t bufsize;
	uint64_t len;
	void *elements;
} ZArray;
#define ZArray_size (sizeof(ZArray))

// -----------------
//  general purpose
// -----------------
ZArray *ZArray_new(size_t bufsize);
ZArray *ZArray_Clone(ZArray *ZArr);
//destructive
void ZArray_EnsureSize(ZArray *ZArr, size_t bufsize);
ZArray *ZArray_Construct(const void *arr, int64_t len, size_t elmsize);
int64_t ZArray_Length(ZArray *ZArr, size_t elmsize);
void *ZArray_Get(ZArray *ZArr, int64_t index, size_t elmsize);
//destructive
void ZArray_Set(ZArray *ZArr, int64_t index, void *setelement, size_t elmsize);
//destructive
void ZArray_Add(ZArray *ZArr, void *addelement, size_t elmsize);
//destructive
void ZArray_Insert(ZArray *ZArr, int64_t index, void *addelement, size_t elmsize);
//destructive
void ZArray_Clear(ZArray *ZArr, int64_t index, size_t elmsize);
int64_t ZArray_IndexOf(ZArray *ZArr, int64_t startindex, ZArray *SearchArr, size_t elmsize);

// -------------------
//  method for String
// -------------------
char ZArray_Equals(ZArray *ZArr1, ZArray *ZArr2);
char ZArray_StartsWith(ZArray *ZArr1, ZArray *ZArr2);
char ZArray_EndsWith(ZArray *ZArr1, ZArray *ZArr2);
ZArray *ZArray_ConCat(ZArray *ZArr1, ZArray *ZArr2);
ZArray *ZArray_SubArray(ZArray *ZArr, int64_t startindex, int64_t endindex, size_t elmsize);

// -----------
//  int Array
// -----------
ZArray *ZIntArray_Construct(const void *arr, int64_t len);
int64_t ZIntArray_Length(ZArray *ZArr);
int64_t ZIntArray_Get(ZArray *ZArr, int64_t index);
//destructive
void ZIntArray_Set(ZArray *ZArr, int64_t index, int64_t setelement);
//destructive
void ZIntArray_Add(ZArray *ZArr, int64_t addelement);
//destructive
void ZIntArray_Insert(ZArray *ZArr, int64_t index, int64_t addelement);
//destructive
void ZIntArray_Clear(ZArray *ZArr, int64_t index);
int64_t ZIntArray_IndexOf_withIndex(ZArray *ZArr, int64_t startindex, int64_t searchvalue);
int64_t ZIntArray_IndexOf(ZArray *ZArr, int64_t searchvalue);

// -------------
//  float Array
// -------------
ZArray *ZFloatArray_Construct(const void *arr, int64_t len);
int64_t ZFloatArray_Length(ZArray *ZArr);
double ZFloatArray_Get(ZArray *ZArr, int64_t index);
//destructive
void ZFloatArray_Set(ZArray *ZArr, int64_t index, double setelement);
//destructive
void ZFloatArray_Add(ZArray *ZArr, double addelement);
//destructive
void ZFloatArray_Insert(ZArray *ZArr, int64_t index, double addelement);
//destructive
void ZFloatArray_Clear(ZArray *ZArr, int64_t index);
int64_t ZFloatArray_IndexOf_withIndex(ZArray *ZArr, int64_t startindex, double searchvalue);
int64_t ZFloatArray_IndexOf(ZArray *ZArr, double searchvalue);

// ---------------
//  boolean Array
// ---------------
ZArray *ZBooleanArray_Construct(const void *arr, int64_t len);
int64_t ZBooleanArray_Length(ZArray *ZArr);
char ZBooleanArray_Get(ZArray *ZArr, int64_t index);
//destructive
void ZBooleanArray_Set(ZArray *ZArr, int64_t index, char setelement);
//destructive
void ZBooleanArray_Add(ZArray *ZArr, char addelement);
//destructive
void ZBooleanArray_Insert(ZArray *ZArr, int64_t index, char addelement);
//destructive
void ZBooleanArray_Clear(ZArray *ZArr, int64_t index);
int64_t ZBooleanArray_IndexOf_withIndex(ZArray *ZArr, int64_t startindex, char searchvalue);
int64_t ZBooleanArray_IndexOf(ZArray *ZArr, char searchvalue);

// --------------
//  Object Array
// --------------
ZArray *ZObjArray_Construct(const void *arr, int64_t len);
int64_t ZObjArray_Length(ZArray *ZArr);
void *ZObjArray_Get(ZArray *ZArr, int64_t index);
//destructive
void ZObjArray_Set(ZArray *ZArr, int64_t index, void *setelement);
//destructive
void ZObjArray_Add(ZArray *ZArr, void *addelement);
//destructive
void ZObjArray_Insert(ZArray *ZArr, int64_t index, void *addelement);
//destructive
void ZObjArray_Clear(ZArray *ZArr, int64_t index);
int64_t ZObjArray_IndexOf_withIndex(ZArray *ZArr, int64_t startindex, void *SearchObj);
int64_t ZObjArray_IndexOf(ZArray *ZArr, void *SearchObj);

// --------
//  String
// --------
typedef ZArray ZString;
ZString *ZString_Construct(const char *arr, int64_t len);
int64_t ZString_StrLen(ZString *ZStr);
ZString *ZString_Get(ZString *ZStr, int64_t index);
int64_t ZString_IndexOf_withIndex(ZString *ZStr, int64_t startindex, ZString *SearchStr);
int64_t ZString_IndexOf(ZString *ZStr, ZString *SearchStr);
ZString *ZString_int_toString(int64_t number);
ZString *ZString_float_toString(double number);
ZString *ZString_boolean_toString(char boolean);
ZString *ZString_NullString();
char ZString_EqualString(ZString *ZStr1, ZString *ZStr2);
char ZString_StartsWith(ZString *ZStr1, ZString *ZStr2);
char ZString_EndsWith(ZString *ZStr1, ZString *ZStr2);
ZString *ZString_StrCat(ZString *ZStr1, ZString *ZStr2);
ZString *ZString_SubString_withEndIndex(ZString *ZStr, int64_t startindex, int64_t endindex);
ZString *ZString_SubString(ZString *ZStr, int64_t startindex);
void ZString_print(ZString *ZStr);
void ZString_println(ZString *ZStr);
void ZString_printerr(ZString *ZStr);

#endif //_DEFINE_ZARRAY

