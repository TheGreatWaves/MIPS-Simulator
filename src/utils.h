#pragma once
#ifndef BASE_UTILS
#define BASE_UTILS

#include "types.h"
#include <stdio.h>

//////////////////////////////////
// NOTE: Useful utilites

#define stmnt(s) do { s } while(0)

// Typing casting
#define     cast(t,v) ((t)(v))
#define  cast_void(v) cast(void,(v)) 
#define cast_voidp(v) cast(void*,(v)) 

// Primitive types generic printing
void print_s32   (s32   x) { printf      ("%d\n" , (s32)(x)) ;}
void print_u32   (u32   x) { printf      ("%u\n" , (u32)(x)) ;}
void print_f32   (f32   x) { printf      ("%lf\n", (f32)(x)) ;}
void print_f64   (f64   x) { printf      ("%f\n" , (f64)(x)) ;}
void print_s64   (s64   x) { printf      ("%ld\n", (s64)(x)) ;}
void print_u64   (u64   x) { printf      ("%lu\n", (u64)(x)) ;}
void print_charp (char* x) { printf      ("%s\n" ,      (x)) ;}
void print_char  (char  x) { print_charp (&x)                ;}
void print_unknown ()      { print_charp ("Unknown type")    ;}

#define gprint(x) _Generic((x), \
  s8: print_s32,\
  s16: print_s32,\
  s32: print_s32,\
  s64: print_s64,\
  u8: print_u32, \
  u16: print_u32, \
  u32: print_u32, \
  u64: print_u64, \
  f32: print_f32, \
  f64: print_f64, \
  char*: print_charp, \
  char: print_char, \
  default: print_unknown \
)(x)

#endif // BASE_UTILS
