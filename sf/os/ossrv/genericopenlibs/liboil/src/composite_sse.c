/*
 * Copyright (c) 2005
 *	Eric Anholt.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
//Portions Copyright (c)  2008-2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <liboilclasses.h>
#include <liboilfunction.h>
#include <emmintrin.h>
#include <liboilcolorspace.h>

#define SSE_FUNCTION __attribute__((force_align_arg_pointer))

#define COMPOSITE_ADD(d,s) oil_clamp_255((d) + (s))

SSE_FUNCTION static void
composite_add_argb_sse (uint32_t *dest, const uint32_t *src, int n)
{
  /* Initial operations to align the destination pointer */
  for (; ((long)dest & 15) && (n > 0); n--) {
    uint32_t d = *dest, s = *src++;

    *dest++ = oil_argb(
	COMPOSITE_ADD(oil_argb_A(d), oil_argb_A(s)),
	COMPOSITE_ADD(oil_argb_R(d), oil_argb_R(s)),
	COMPOSITE_ADD(oil_argb_G(d), oil_argb_G(s)),
	COMPOSITE_ADD(oil_argb_B(d), oil_argb_B(s)));
  }
  for (; n >= 4; n -= 4) {
    __m128i s;
    s = _mm_loadu_si128((__m128i *)src);
    *(__m128i *)dest = _mm_adds_epu8(s, *(__m128i *)dest);
    src += 4;
    dest += 4;
  }
  for (; n > 0; n--) {
    uint32_t d = *dest, s = *src++;

    *dest++ = oil_argb(
	COMPOSITE_ADD(oil_argb_A(d), oil_argb_A(s)),
	COMPOSITE_ADD(oil_argb_R(d), oil_argb_R(s)),
	COMPOSITE_ADD(oil_argb_G(d), oil_argb_G(s)),
	COMPOSITE_ADD(oil_argb_B(d), oil_argb_B(s)));
  }
}
OIL_DEFINE_IMPL_FULL (composite_add_argb_sse, composite_add_argb,
    OIL_IMPL_FLAG_SSE2);

SSE_FUNCTION static void
composite_add_argb_const_src_sse (uint32_t *dest, const uint32_t *src_1, int n)
{
  __m128i s;
  uint32_t val = *src_1;

  /* Initial operations to align the destination pointer */
  for (; ((long)dest & 15) && (n > 0); n--) {
    uint32_t d = *dest;

    *dest++ = oil_argb(
	COMPOSITE_ADD(oil_argb_A(d), oil_argb_A(val)),
	COMPOSITE_ADD(oil_argb_R(d), oil_argb_R(val)),
	COMPOSITE_ADD(oil_argb_G(d), oil_argb_G(val)),
	COMPOSITE_ADD(oil_argb_B(d), oil_argb_B(val)));
  }
  s = _mm_set1_epi32(val);
  for (; n >= 4; n -= 4) {
    __m128i xmm0;
    xmm0 = _mm_adds_epu8(s, *(__m128i *)dest);
    _mm_store_si128((__m128i *)dest, xmm0);
    dest += 4;
  }
  for (; n > 0; n--) {
    uint32_t d = *dest;

    *dest++ = oil_argb(
	COMPOSITE_ADD(oil_argb_A(d), oil_argb_A(val)),
	COMPOSITE_ADD(oil_argb_R(d), oil_argb_R(val)),
	COMPOSITE_ADD(oil_argb_G(d), oil_argb_G(val)),
	COMPOSITE_ADD(oil_argb_B(d), oil_argb_B(val)));
  }
}
OIL_DEFINE_IMPL_FULL (composite_add_argb_const_src_sse,
    composite_add_argb_const_src, OIL_IMPL_FLAG_SSE2);

SSE_FUNCTION static void
composite_add_u8_sse (uint8_t *dest, const uint8_t *src, int n)
{
  /* Initial operations to align the destination pointer */
  for (; ((long)dest & 15) && (n > 0); n--) {
    int x = (int)*dest + *src++;
    if (x > 255) 
      x = 255;
    *dest++ = x;
  }
  for (; n >= 16; n -= 16) {
    __m128i d, s;
    s = _mm_loadu_si128((__m128i *)src);
    d = _mm_adds_epu8(s, *(__m128i *)dest);
    _mm_store_si128((__m128i *)dest, d);
    src += 16;
    dest += 16;
  }
  for (; n > 0; n--) {
    int x = (int)*dest + *src++;
    if (x > 255) 
      x = 255;
    *dest++ = x;
  }
}
OIL_DEFINE_IMPL_FULL (composite_add_u8_sse, composite_add_u8,
    OIL_IMPL_FLAG_SSE2);

SSE_FUNCTION static void
composite_add_u8_const_src_sse (uint8_t *dest, const uint8_t *src_1, int n)
{
  __m128i s;
  int val = *src_1;

  /* Initial operations to align the destination pointer */
  for (; ((long)dest & 15) && (n > 0); n--) {
    int x = *dest + val;
    if (x > 255) 
      x = 255;
    *dest++ = x;
  }
  s = _mm_set1_epi8(val);
  for (; n >= 16; n -= 16) {
    __m128i d;
    d = _mm_adds_epu8(*(__m128i *)dest, s);
    _mm_store_si128((__m128i *)dest, d);
    dest += 16;
  }
  for (; n > 0; n--) {
    int x = *dest + val;
    if (x > 255) 
      x = 255;
    *dest++ = x;
  }
}
OIL_DEFINE_IMPL_FULL (composite_add_u8_const_src_sse,
    composite_add_u8_const_src, OIL_IMPL_FLAG_SSE2);


#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_composite_add_argb_sse, composite_add_argb() {
		return &_oil_function_impl_composite_add_argb_sse, composite_add_argb;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_composite_add_argb_const_src_sse() {
		return &_oil_function_impl_composite_add_argb_const_src_sse;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_composite_add_u8_sse, composite_add_u8() {
		return &_oil_function_impl_composite_add_u8_sse, composite_add_u8;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_composite_add_u8_const_src_sse() {
		return &_oil_function_impl_composite_add_u8_const_src_sse;
}
#endif

