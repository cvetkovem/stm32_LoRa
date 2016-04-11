/*
 * Main archive of that lib is decrease program size
 *
 */
#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>


typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef signed short int16_t;
typedef unsigned short uint16_t;

typedef signed long int32_t;
typedef unsigned long uint32_t;

typedef uint8_t bool;

typedef struct
{
  long quot; /* quotient */
  long rem; /* remainder */
} ldiv_t;

#define true	1
#define false	0

/*
 *  Common functions
 */
int32_t abs_32(int32_t i);
int16_t abs_16(int16_t s);
int8_t 	abs_8(int8_t c);

int32_t max_32(int32_t a, int32_t b);
int16_t max_16(int16_t a, int16_t b);
int8_t	max_8(int8_t a, int8_t b);

int32_t min_32(int32_t a, int32_t b);
int16_t min_16(int16_t a, int16_t b);
int8_t	min_8(int8_t a, int8_t b);

#undef memcpy
void * memcpy(void * dst, const void * src, size_t len);

#undef memcmp
int memcmp(const void * s1, const void * s2, size_t n);
int memcmp_r(const void * s1, const void * s2, size_t n);

#undef strncmp
int strncmp(const char *s1, const char *s2, size_t n);

#undef memset
void * memset(void * s, int c, size_t n);

#undef strlen
size_t strlen (const char *str);

#undef vsprintf
int vsprintf( char *out, const char *format, va_list args);

#undef snprint
int snprint(char *out, const char *format, ...);

#undef ldiv
#define div(numer,denom) ldiv((numer),(denom))
ldiv_t ldiv(long int numer, long int denom);

#undef strncpy
char *strncpy(char *dst0, const char *src0, size_t count);

#undef strcpy
char* strcpy (char* dst, const char* src);

#undef strcmp
int strcmp(const char *s1, const char *s2);

#endif
