#ifndef TypesH
#define TypesH

#ifdef _WIN32
typedef unsigned __int64 uint64;
typedef unsigned long    uint32;
typedef __int64          int64;
typedef int              int32;
#else
typedef unsigned long long uint64;
typedef unsigned long uint32;
typedef long long int64;
typedef long      int32;
#endif

typedef uint64 Bitboard;

#endif
