#ifndef _DEFINE_ZMAP
#define _DEFINE_ZMAP

// ----------
//  HashFunc
// ----------
int64_t hash(ZString *Key, uint64_t hashsize);
int64_t nexthash(int64_t hash, uint64_t hashsize);

// -----------------
//  general purpose
// -----------------
typedef struct {
	uint64_t capacity;
	uint64_t size;
	ZArray *KeyArray;
	ZArray *ValueArray;
} ZMap;
#define ZMap_size (sizeof(ZMap))

ZMap *ZMap_new(size_t capacity, size_t elmsize);
ZMap *ZMap_InitMap(size_t elmsize);
//destructive
void ZMap_Remap(ZMap *Map, size_t elmsize);
//destructive
void ZMap_CheckLoadFactor(ZMap *Map, size_t elmsize);
int64_t ZMap_SearchFreeIndex(ZMap *Map, ZString *Key);
int64_t ZMap_IndexOfKey(ZMap *Map, ZString *Key);
char ZMap_ContainsKey(ZMap *Map, ZString *Key);
void *ZMap_Get(ZMap *Map, ZString *Key, size_t elmsize);
//destructive
void ZMap_Put(ZMap *Map, ZString *Key, void *value, size_t elmsize);

// ---------
//  int Map
// ---------
ZMap *ZIntMap_InitMap();
int64_t ZIntMap_Get(ZMap *Map, ZString *Key);
//destructive
void ZIntMap_Put(ZMap *Map, ZString *Key, int64_t value);

// -----------
//  float Map
// -----------
ZMap *ZFloatMap_InitMap();
double ZFloatMap_Get(ZMap *Map, ZString *Key);
//destructive
void ZFloatMap_Put(ZMap *Map, ZString *Key, double value);

// -------------
//  boolean Map
// -------------
ZMap *ZBooleanMap_InitMap();
char ZBooleanMap_Get(ZMap *Map, ZString *Key);
//destructive
void ZBooleanMap_Put(ZMap *Map, ZString *Key, char value);

// ------------
//  Object Map
// ------------
ZMap *ZObjMap_InitMap();
void *ZIntMap_Get(ZMap *Map, ZString *Key);
//destructive
void ZIntMap_Put(ZMap *Map, ZString *Key, void *value);

#endif //_DEFINE_ZMAP

