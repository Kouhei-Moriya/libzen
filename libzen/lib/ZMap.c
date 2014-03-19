//#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <gc.h>

#include "ZStdLib.h"
#include "ZArray.h"


// ----------
//  HashFunc
// ----------
int64_t hash(ZString *Key, uint64_t hashsize) {
	int i;
	int max = (int)Key->len;
	char *Str = (char *)Key->elements;
	uint64_t h = 0;
	for(i = 0; i < max; ++i) {
		h = (h * 257 + (int64_t)Str[i]) % (int64_t)hashsize;
	}
	return h;
}
int64_t nexthash(int64_t hash, uint64_t hashsize) {
	return (hash * 257) % (int64_t)hashsize;
}


// -----------------
//  general purpose
// -----------------
ZMap *ZMap_new(size_t capacity, size_t elmsize) {
	ZMap *Map = (ZMap *)GC_malloc(ZMap_size);
	if(Map == NULL) {
		exit(EXIT_FAILURE);
	}
	Map->capacity = (uint64_t)capacity;
	Map->size = 0;
	Map->KeyArray = ZArray_new(capacity * SIZE_POINTER);
	Map->ValueArray = ZArray_new(capacity * elmsize);
}

ZMap *ZMap_InitMap(size_t elmsize) {
	return ZMap_new(INITCAPACITY, elmsize);
}

/* ZMap *ZMap_Construct(const char **keys, const void *values, size_t len, size_t elmsize) {
	ZMap *Map = ZMap_ConstructEmpty(elmsize);
	int64_t i;
	for(i = 0; i < len; ++i) {
		const char *key = keys + i;
		if(keys[i] != NULL) {
			ZString *Key = ZString_Construct(key, strlen(key));
			void *Value = values + (i * elmsize);
			ZMap_Put(Map, Key, Value, elmsize);
		}
	}
	return Map;
} */

//destructive
void ZMap_Remap(ZMap *Map, size_t elmsize) {
	ZMap *TempMap = ZMap_new((size_t)(Map->capacity * 2), elmsize);

	int64_t capacity = (int64_t)Map->capacity;
	ZArray *KeyArray = Map->KeyArray;
	ZArray *ValueArray = Map->ValueArray;

	int64_t i;
	for(i = 0; i < capacity; ++i) {
		ZString *Key = (ZString *)ZObjArray_Get(KeyArray, i);
		if(Key != NULL) {
			void *Value = ZArray_Get(ValueArray, i, elmsize);
			ZMap_Put(TempMap, Key, Value, elmsize);
		}
	}
	*Map = *TempMap;
}

//destructive
void ZMap_CheckLoadFactor(ZMap *Map, size_t elmsize) {
	if(((double)Map->size / (double)Map->capacity) > LOADFACTOR) {
		ZMap_Remap(Map, elmsize);
	}
}

int64_t ZMap_SearchFreeIndex(ZMap *Map, ZString *Key) {
	uint64_t capacity = Map->capacity;
	ZArray *KeyArray = Map->KeyArray;
	int64_t h = hash(Key, capacity);

	uint64_t i;
	for(i = 0; i < capacity; ++i) {
		ZString *PutKey = (ZString *)ZObjArray_Get(KeyArray, h);
		if(PutKey == NULL) {
			return h;
		}
		h = nexthash(h, capacity);
	}
	return -1;
}
int64_t ZMap_IndexOfKey(ZMap *Map, ZString *Key) {
	uint64_t capacity = Map->capacity;
	ZArray *KeyArray = Map->KeyArray;
	int64_t h = hash(Key, capacity);

	uint64_t i;
	for(i = 0; i < capacity; ++i) {
		ZString *PutKey = (ZString *)ZObjArray_Get(KeyArray, h);
		if(PutKey == NULL) {
			break;
		} else if(ZString_EqualString(Key, PutKey)) {
			return h;
		}
		h = nexthash(h, capacity);
	}
	return -1;
}
char ZMap_ContainsKey(ZMap *Map, ZString *Key) {
	if(ZMap_IndexOfKey(Map, Key) == -1) {
		return BOOL_FALSE;
	} else {
		return BOOL_TRUE;
	}
}

void *ZMap_Get(ZMap *Map, ZString *Key, size_t elmsize) {
	int64_t index = ZMap_IndexOfKey(Map, Key);
	if(index == -1) {
		return NULL;
	}
	return ZArray_Get(Map->ValueArray, index, elmsize);
}

//destructive
void ZMap_Put(ZMap *Map, ZString *Key, void *value, size_t elmsize) {
	int64_t index = ZMap_IndexOfKey(Map, Key);
	if(index == -1) {
		ZMap_CheckLoadFactor(Map, elmsize);
		index = ZMap_SearchFreeIndex(Map, Key);
		if(index == -1) {
			exit(EXIT_FAILURE);
		}
		ZObjArray_Set(Map->KeyArray, index, (void *)Key);
		Map->size += 1;
	}
	ZArray_Set(Map->ValueArray, index, value, elmsize);
}


// ---------
//  int Map
// ---------
ZMap *ZIntMap_InitMap() {
	return ZMap_InitMap(SIZE_I64);
}
int64_t ZIntMap_Get(ZMap *Map, ZString *Key) {
	return *((int64_t *)ZMap_Get(Map, Key, SIZE_I64));
}
//destructive
void ZIntMap_Put(ZMap *Map, ZString *Key, int64_t value) {
	return ZMap_Put(Map, Key, (void *)&value, SIZE_I64);
}


// -----------
//  float Map
// -----------
ZMap *ZFloatMap_InitMap() {
	return ZMap_InitMap(SIZE_DOUBLE);
}
double ZFloatMap_Get(ZMap *Map, ZString *Key) {
	return *((double *)ZMap_Get(Map, Key, SIZE_DOUBLE));
}
//destructive
void ZFloatMap_Put(ZMap *Map, ZString *Key, double value) {
	return ZMap_Put(Map, Key, (void *)&value, SIZE_DOUBLE);
}


// -------------
//  boolean Map
// -------------
ZMap *ZBooleanMap_InitMap() {
	return ZMap_InitMap(SIZE_BOOL);
}
char ZBooleanMap_Get(ZMap *Map, ZString *Key) {
	return *((char *)ZMap_Get(Map, Key, SIZE_BOOL));
}
//destructive
void ZBooleanMap_Put(ZMap *Map, ZString *Key, char value) {
	return ZMap_Put(Map, Key, (void *)&value, SIZE_BOOL);
}


// ------------
//  Object Map
// ------------
ZMap *ZObjMap_InitMap() {
	return ZMap_InitMap(SIZE_POINTER);
}
void *ZIntMap_Get(ZMap *Map, ZString *Key) {
	return *((void **)ZMap_Get(Map, Key, SIZE_I64));
}
//destructive
void ZIntMap_Put(ZMap *Map, ZString *Key, void *value) {
	return ZMap_Put(Map, Key, (void *)&value, SIZE_I64);
}

