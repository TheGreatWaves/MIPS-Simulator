#pragma once
#ifndef BASE_UTILS
#define BASE_UTILS

#include "types.h"
#include <stdio.h>


//////////////////////////////////
// NOTE: Debug print

#define DEBUG

#ifndef DEBUG
#define dprint
#else
#define dprint(fmt, ...) \
        do { fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__); } while (0)
#endif

//////////////////////////////////
// NOTE: This file contains some general useful utilites.

#define stmnt(s) do { s } while(0)

/////////////////////////////////////
// NOTE(Appy): Type casting
#define     cast(t,v) ((t)(v))
#define  cast_void(v) cast(void,(v)) 
#define cast_voidp(v) cast(void*,(v)) 
#define u32t(V) (cast(uint32_t, V))

/////////////////////////////////////
// NOTE(Appy): Utils

#define MASK(n) (~((~((uint32_t)0)) << n)) // creates a mask of n 1s
#define MASK1(n, p) ((MASK(n))<<(p))
#define MASK0(n, p) (~(MASK1(n, p)))

#define TWOCOMP(x) ((~(x))+1)
#define NOR_OP(a, b) (~((a)|(b)))

// Primitive types generic printing
static void print_s32   (s32   x) { printf      ("%d\n" , (s32)(x)) ;}
static void print_u32   (u32   x) { printf      ("%u\n" , (u32)(x)) ;}
static void print_f32   (f32   x) { printf      ("%lf\n", (f32)(x)) ;}
static void print_f64   (f64   x) { printf      ("%f\n" , (f64)(x)) ;}
static void print_s64   (s64   x) { printf      ("%ld\n", (s64)(x)) ;}
static void print_u64   (u64   x) { printf      ("%lu\n", (u64)(x)) ;}
static void print_charp (char* x) { printf      ("%s\n" ,      (x)) ;}
static void print_char  (char  x) { print_charp (&x)                ;}
static void print_unknown ()      { print_charp ("Unknown type")    ;}

#ifndef DEBUG
#define gprint
#else
#define gprint(x) _Generic((x), \
  s8: print_s32,                \
  s16: print_s32,               \
  s32: print_s32,               \
  s64: print_s64,               \
  u8: print_u32,                \
  u16: print_u32,               \
  u32: print_u32,               \
  u64: print_u64,               \
  f32: print_f32,               \
  f64: print_f64,               \
  char*: print_charp,           \
  char: print_char,             \
  default: print_unknown        \
)(x)
#endif

inline u8 count_set_bits(u32 n)
{
  u32 total_count = 0;
  u32 count = 0;
  while(n)
  {
    count += 1;
    if (n&1)
    {
      total_count = count;
    }
    n >>= 1;
  }
  return count;
}

#define MAKE_SIGN_EXTEND(sz) u32 sign_extend_##sz(uint32_t n) { if ((n >> (sz - 1)) & 1) { n = n | MASK1((32 - sz), sz); } return n; }


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

#endif // BASE_UTILS
