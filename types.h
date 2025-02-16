#ifndef TypesH
#define TypesH

#ifdef _WIN32
typedef unsigned __int64 uint64;
#else
typedef unsigned long long uint64;
typedef unsigned long uint32;
#endif

typedef uint64 Bitboard;

#endif
