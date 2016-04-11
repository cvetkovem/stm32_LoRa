/*
 * Main archive of that lib is decrease program size
 *
 */
#include <stdlib.h>

/*
 *  Common functions
 */

inline int32_t abs_32(int32_t i)
{
	return ((i ^ (i >> 31)) - (i >> 31));
}

inline int16_t abs_16(int16_t s)
{
	return ((s ^ (s >> 15)) - (s >> 15));
}

inline int8_t abs_8(int8_t c)
{
	return ((c ^ (c >> 7)) - (c >> 7));
}

inline int32_t max_32(int32_t a, int32_t b)
{
	return  a - ((a - b) & ((a - b) >> 31));
}

inline int32_t min_32(int32_t a, int32_t b)
{
	return  b + ((a - b) & ((a - b) >> 31));
}

inline int16_t max_16(int16_t a, int16_t b)
{
	return  a - ((a - b) & ((a - b) >> 15));
}

inline int16_t min_16(int16_t a, int16_t b)
{
	return  b + ((a - b) & ((a - b) >> 15));
}

inline int8_t max_8(int8_t a, int8_t b)
{
	return  a - ((a - b) & ((a - b) >> 7));
}

inline int8_t min_8(int8_t a, int8_t b)
{
	return b + ((a - b) & ((a - b) >> 7));
}

void * memcpy(void * dst, const void * src, size_t len)
{
	const unsigned char *csrc = (const unsigned char*) src;

	unsigned char *tmp = (unsigned char *) dst;

	while (len--)	*tmp++ = *csrc++;

	return dst;
}

int memcmp(const void * s1, const void * s2, size_t n)
{
	const unsigned char *cs1 = (const unsigned char*) s1;
	const unsigned char *cs2 = (const unsigned char*) s2;

	signed char ret = 0;

	while (ret == 0 && n--)  ret = *cs1++ - *cs2++;

	return ret;
}

int memcmp_r(const void * s1, const void * s2, size_t n)
{
	const unsigned char *cs1 = (const unsigned char*) s1;
	const unsigned char *cs2 = (const unsigned char*) s2;

	signed char ret = 0;

	while (ret == 0 && n--)  ret = cs1[n] - cs2[n];

	return ret;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
	register int ret = 0;

	while ((ret == 0) && (*(uint8_t *)s2 != 0) && (n--)) ret = *(uint8_t *)s1++ - *(uint8_t *)s2++;

	return ret;
}

void * memset(void * s, int c, size_t n)
{
	unsigned char * s1 = (unsigned char *)s;

	while (n--) *s1++ = c;

	return s;
}

/*
 * New print functions
 */
#define PAD_RIGHT 1
#define PAD_ZERO 2
#define PRINT_BUF_LEN 12
/* Print string */
static int prints(char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0)
	{
		register int len = 0;
		register const char *ptr;

		for (ptr = string; *ptr; ++ptr) ++len;

		if (len >= width) width = 0;
		else width -= len;

		if (pad & PAD_ZERO) padchar = '0';
	}

	if (!(pad & PAD_RIGHT))
	{
		for ( ; width > 0; --width)
		{
			**out = padchar;
			++(*out);
			++pc;
		}
	}

	for ( ; *string ; ++string)
	{
		**out = *string;
		++(*out);
		++pc;
	}

	for ( ; width > 0; --width)
	{
		**out = padchar;
		++(*out);
		++pc;
	}

	return pc;
}

/* print integer value */
static int printi(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
	char print_buf[12];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = (unsigned int)i;

	if (i == 0)
	{
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints (out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0)
	{
		neg = 1;
		u = (unsigned int)-i;
	}

	s = print_buf + PRINT_BUF_LEN-1;
	*s = '\0';

	while (u)
	{
		t = (unsigned int)u % b;

		if( t >= 10 )
			t += letbase - '0' - 10;

		*--s = (char)(t + '0');
		u /= b;
	}

	if (neg)
	{
		if( width && (pad & PAD_ZERO) )
		{
			**out = '-';
			++(*out);
			++pc;
			--width;
		}
		else
		{
			*--s = '-';
		}
	}

	return pc + prints(out, s, width, pad);
}

int vsprintf(char *out, const char *format, va_list args )
{
	register int width, pad;
	register int pc = 0;
	char scr[2];

	for (; *format != 0; ++format)
	{
		if (*format == '%')
		{
			++format;
			width = pad = 0;

			if (*format == '\0') break;

			if (*format == '%') goto out_f;

			if (*format == '-')
			{
				++format;
				pad = PAD_RIGHT;
			}

			while (*format == '0')
			{
				++format;
				pad |= PAD_ZERO;
			}

			for ( ; *format >= '0' && *format <= '9'; ++format)
			{
				width *= 10;
				width += *format - '0';
			}

			if( *format == 's' )
			{
				register char *s = (char *)va_arg( args, int );
				pc += prints (&out, s?s:"(null)", width, pad);
				continue;
			}

			if( *format == 'd' )
			{
				pc += printi (&out, va_arg( args, int ), 10, 1, width, pad, 'a');

				continue;
			}

			if( *format == 'x' )
			{
				pc += printi (&out, va_arg( args, int ), 16, 0, width, pad, 'a');
				continue;
			}

			if( *format == 'X' )
			{
				pc += printi (&out, va_arg( args, int ), 16, 0, width, pad, 'A');
				continue;
			}

			if( *format == 'u' )
			{
				pc += printi (&out, va_arg( args, int ), 10, 0, width, pad, 'a');
				continue;
			}

			if( *format == 'c' )
			{
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += prints (&out, scr, width, pad);
				continue;
			}
		}
		else
		{
out_f:
			*out = *format;
			++(out);
			++pc;
		}
	}

	if (out) *out = '\0';

	va_end( args );
	return pc;
}

/* only for 32 bit MCU */
size_t strlen (const char *str)
{
	const char *char_ptr;
	const uint32_t *longword_ptr;
	uint32_t longword, himagic, lomagic;

	/* Handle the first few characters by reading one character at a time.
	   Do this until CHAR_PTR is aligned on a longword boundary.  */
	for (char_ptr = str; ((uint32_t)char_ptr & ((uint32_t)3)) != 0; ++char_ptr)
	{
		if (*char_ptr == '\0')
			return char_ptr - str;
	}

	/* All these elucidatory comments refer to 4-byte longwords,
	   but the theory applies equally well to 8-byte longwords.  */

	longword_ptr = (uint32_t *) char_ptr;

	/* Bits 31, 24, 16, and 8 of this number are zero.  Call these bits
	   the "holes."  Note that there is a hole just to the left of
	   each byte, with an extra at the end:

	   bits:  01111110 11111110 11111110 11111111
	   bytes: AAAAAAAA BBBBBBBB CCCCCCCC DDDDDDDD

	   The 1-bits make sure that carries propagate to the next 0-bit.
	   The 0-bits provide holes for carries to fall into.
  */
	himagic = 0x80808080L;
	lomagic = 0x01010101L;

	/* Instead of the traditional loop which tests each character,
	   we will test a longword at a time.  The tricky part is testing
	   if *any of the four* bytes in the longword in question are zero.
  */
	for (;;)
	{
		/* We tentatively exit the loop if adding MAGIC_BITS to
		LONGWORD fails to change any of the hole bits of LONGWORD.

		1) Is this safe?  Will it catch all the zero bytes?
		Suppose there is a byte with all zeros.  Any carry bits
		propagating from its left will fall into the hole at its
		least significant bit and stop.  Since there will be no
		carry from its most significant bit, the LSB of the
		byte to the left will be unchanged, and the zero will be
		detected.

		2) Is this worthwhile?  Will it ignore everything except
		zero bytes?  Suppose every byte of LONGWORD has a bit set
		somewhere.  There will be a carry into bit 8.  If bit 8
		is set, this will carry into bit 16.  If bit 8 is clear,
		one of bits 9-15 must be set, so there will be a carry
		into bit 16.  Similarly, there will be a carry into bit
		24.  If one of bits 24-30 is set, there will be a carry
		into bit 31, so all of the hole bits will be changed.

		The one misfire occurs when bits 24-30 are clear and bit
		31 is set; in this case, the hole at bit 31 is not
		changed.  If we had access to the processor carry flag,
		we could close this loophole by putting the fourth hole
		at bit 32!

		So it ignores everything except 128's, when they're aligned
		properly.  */

		longword = *longword_ptr++;

		if (((longword - lomagic) & himagic) != 0)
		{
			/* Which of the bytes was the zero?  If none of them were, it was
			   a misfire; continue the search.  */

			const char *cp = (const char *) (longword_ptr - 1);

			if (cp[0] == 0)
				return cp - str;

			if (cp[1] == 0)
				return cp - str + 1;

			if (cp[2] == 0)
				return cp - str + 2;

			if (cp[3] == 0)
				return cp - str + 3;

			if (sizeof (longword) > 4)
			{
				if (cp[4] == 0)
					return cp - str + 4;

				if (cp[5] == 0)
					return cp - str + 5;

				if (cp[6] == 0)
					return cp - str + 6;

				if (cp[7] == 0)
					return cp - str + 7;
			}
		}
	}
}

int snprint(char *out, const char *format, ...)
{
	va_list args;
    va_start(args, format);
	return vsprintf(out, format, args);
}

ldiv_t ldiv(long int numer, long int denom)
{
  return (ldiv_t){.quot = numer/denom, .rem = numer%denom};
}

#define UNALIGNED(X, Y) (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))
#define TOO_SMALL(LEN) ((LEN) < sizeof (long))
#define DETECTNULL(X) (((X) - 0x01010101) & ~(X) & 0x80808080)
char *strncpy(char *dst0, const char *src0, size_t count)
{
#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)
  char *dscan;
  const char *sscan;
  dscan = dst0;
  sscan = src0;
  while (count > 0)
    {
      --count;
      if ((*dscan++ = *sscan++) == '\0')
	break;
    }
  while (count-- > 0)
    *dscan++ = '\0';
  return dst0;
#else
  char *dst = dst0;
  const char *src = src0;
  long *aligned_dst;
  const long *aligned_src;
  /* If SRC and DEST is aligned and count large enough, then copy words.  */
  if (!UNALIGNED (src, dst) && !TOO_SMALL (count))
    {
      aligned_dst = (long*)dst;
      aligned_src = (long*)src;
      /* SRC and DEST are both "long int" aligned, try to do "long int"
	 sized copies.  */
      while (count >= sizeof (long int) && !DETECTNULL(*aligned_src))
	{
	  count -= sizeof (long int);
	  *aligned_dst++ = *aligned_src++;
	}
      dst = (char*)aligned_dst;
      src = (char*)aligned_src;
    }
  while (count > 0)
    {
      --count;
      if ((*dst++ = *src++) == '\0')
	break;
    }
  while (count-- > 0)
    *dst++ = '\0';
  return dst0;
#endif /* not PREFER_SIZE_OVER_SPEED */
}

char *strcpy(char *dst0, const char *src0)
{
#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)
  char *s = dst0;

  while (*dst0++ = *src0++);

  return s;
#else
  char *dst = dst0;
  const char *src = src0;
  long *aligned_dst;
  const long *aligned_src;

  /* If SRC or DEST is unaligned, then copy bytes.  */
  if (!UNALIGNED (src, dst))
    {
      aligned_dst = (long*)dst;
      aligned_src = (long*)src;

      /* SRC and DEST are both "long int" aligned, try to do "long int"
         sized copies.  */
      while (!DETECTNULL(*aligned_src))
        {
          *aligned_dst++ = *aligned_src++;
        }

      dst = (char*)aligned_dst;
      src = (char*)aligned_src;
    }

  while ((*dst++ = *src++))
    ;
  return dst0;
#endif /* not PREFER_SIZE_OVER_SPEED */
}

int strcmp(const char *s1, const char *s2)
{
#if defined(PREFER_SIZE_OVER_SPEED) || defined(__OPTIMIZE_SIZE__)
  while (*s1 != '\0' && *s1 == *s2)
    {
      s1++;
      s2++;
    }

  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
#else
  unsigned long *a1;
  unsigned long *a2;

  /* If s1 or s2 are unaligned, then compare bytes. */
  if (!UNALIGNED (s1, s2))
    {
      /* If s1 and s2 are word-aligned, compare them a word at a time. */
      a1 = (unsigned long*)s1;
      a2 = (unsigned long*)s2;
      while (*a1 == *a2)
        {
          /* To get here, *a1 == *a2, thus if we find a null in *a1,
	     then the strings must be equal, so return zero.  */
          if (DETECTNULL (*a1))
	    return 0;

          a1++;
          a2++;
        }

      /* A difference was detected in last few bytes of s1, so search bytewise */
      s1 = (char*)a1;
      s2 = (char*)a2;
    }

  while (*s1 != '\0' && *s1 == *s2)
    {
      s1++;
      s2++;
    }
  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
#endif /* not PREFER_SIZE_OVER_SPEED */
}





