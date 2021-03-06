/*
 * LIBOIL - Library of Optimized Inner Loops
 * Copyright (c) 2003,2004 David A. Schleef <ds@schleef.org>
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
//Portions Copyright (c)  2008-2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved. 

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <liboil/liboilfunction.h>
#include "liboil/simdpack/simdpack.h"

#ifdef __SYMBIAN32__
#ifdef __WINSCW__
#pragma warn_emptydecl off
#endif//__WINSCW__
#endif//__SYMBIAN32__

#define SCALARMULT_DEFINE_REF(type)		\
static void scalarmult_ ## type ## _ref(	\
    oil_type_ ## type *dest, int dstr,		\
    oil_type_ ## type *src, int sstr,		\
    oil_type_ ## type *val, int n)			\
{						\
  int i;					\
  for(i=0;i<n;i++){				\
    OIL_GET(dest,dstr*i,oil_type_ ## type) = OIL_GET(src,sstr*i,oil_type_ ## type) * *val; \
  }						\
}						\
OIL_DEFINE_CLASS(scalarmult_ ## type,           \
    "oil_type_" #type " *d, int dstr, "          \
    "oil_type_" #type " *s1, int sstr, "           \
    "oil_type_" #type " *s2_1, int n");            \
OIL_DEFINE_IMPL_REF (scalarmult_ ## type ## _ref, scalarmult_ ## type);


/**
 * oil_scalarmult_s8:
 * @d:
 * @dstr:
 * @s1:
 * @sstr:
 * @s2_1:
 * @n:
 *
 * Multiplies the constant value @s2_1 to each value in @s1 and places the
 * result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
SCALARMULT_DEFINE_REF (s8);
/**
 * oil_scalarmult_u8:
 * @d:
 * @dstr:
 * @s1:
 * @sstr:
 * @s2_1:
 * @n:
 *
 * Multiplies the constant value @s2_1 to each value in @s1 and places the
 * result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
SCALARMULT_DEFINE_REF (u8);
/**
 * oil_scalarmult_s16:
 * @d:
 * @dstr:
 * @s1:
 * @sstr:
 * @s2_1:
 * @n:
 *
 * Multiplies the constant value @s2_1 to each value in @s1 and places the
 * result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
SCALARMULT_DEFINE_REF (s16);
/**
 * oil_scalarmult_u16:
 * @d:
 * @dstr:
 * @s1:
 * @sstr:
 * @s2_1:
 * @n:
 *
 * Multiplies the constant value @s2_1 to each value in @s1 and places the
 * result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
SCALARMULT_DEFINE_REF (u16);
/**
 * oil_scalarmult_s32:
 * @d:
 * @dstr:
 * @s1:
 * @sstr:
 * @s2_1:
 * @n:
 *
 * Multiplies the constant value @s2_1 to each value in @s1 and places the
 * result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
SCALARMULT_DEFINE_REF (s32);
/**
 * oil_scalarmult_u32:
 * @d:
 * @dstr:
 * @s1:
 * @sstr:
 * @s2_1:
 * @n:
 *
 * Multiplies the constant value @s2_1 to each value in @s1 and places the
 * result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
SCALARMULT_DEFINE_REF (u32);
/**
 * oil_scalarmult_f32:
 * @d:
 * @dstr:
 * @s1:
 * @sstr:
 * @s2_1:
 * @n:
 *
 * Multiplies the constant value @s2_1 to each value in @s1 and places the
 * result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
SCALARMULT_DEFINE_REF (f32);
/**
 * oil_scalarmult_f64:
 * @d:
 * @dstr:
 * @s1:
 * @sstr:
 * @s2_1:
 * @n:
 *
 * Multiplies the constant value @s2_1 to each value in @s1 and places the
 * result in @d.
 *
 * FIXME: This function is difficult to optimize and will likely be
 * replaced.
 */
SCALARMULT_DEFINE_REF (f64);

#ifdef	__SYMBIAN32__
 
#endif
OilFunctionClass*  __oil_function_class_scalarmult_f32()
{
return &_oil_function_class_scalarmult_f32;
}


#ifdef	__SYMBIAN32__
 
#endif
OilFunctionClass*  __oil_function_class_scalarmult_f64()
{
return &_oil_function_class_scalarmult_f64;
}

#ifdef	__SYMBIAN32__
 
#endif
OilFunctionClass*  __oil_function_class_scalarmult_s16()
{
return &_oil_function_class_scalarmult_s16;
}

#ifdef	__SYMBIAN32__
 
#endif
OilFunctionClass*  __oil_function_class_scalarmult_s32()
{
return &_oil_function_class_scalarmult_s32;
}

#ifdef	__SYMBIAN32__
 
#endif
OilFunctionClass*  __oil_function_class_scalarmult_s8()
{
return &_oil_function_class_scalarmult_s8;
}

#ifdef	__SYMBIAN32__
 
#endif
OilFunctionClass*  __oil_function_class_scalarmult_u16()
{
return &_oil_function_class_scalarmult_u16;
}

#ifdef	__SYMBIAN32__
 
#endif
OilFunctionClass*  __oil_function_class_scalarmult_u32()
{
return &_oil_function_class_scalarmult_u32;
}

#ifdef	__SYMBIAN32__
 
#endif
OilFunctionClass*  __oil_function_class_scalarmult_u8()
{
return &_oil_function_class_scalarmult_u8;
}

/*
OilFunctionClass*  __oil_function_class_scalarmult_f32_ns()
{
return &_oil_function_class_scalarmult_f32_ns;
}

OilFunctionClass*  __oil_function_class_scalarmult_f64_ns()
{
return &_oil_function_class_scalarmult_f64_ns;
}
*/



#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scalarmult_s8_ref() {
		return &_oil_function_impl_scalarmult_s8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scalarmult_u8_ref() {
		return &_oil_function_impl_scalarmult_u8_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scalarmult_s16_ref() {
		return &_oil_function_impl_scalarmult_s16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scalarmult_u16_ref() {
		return &_oil_function_impl_scalarmult_u16_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scalarmult_s32_ref() {
		return &_oil_function_impl_scalarmult_s32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scalarmult_u32_ref() {
		return &_oil_function_impl_scalarmult_u32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scalarmult_f32_ref() {
		return &_oil_function_impl_scalarmult_f32_ref;
}
#endif

#ifdef	__SYMBIAN32__
 
OilFunctionImpl* __oil_function_impl_scalarmult_f64_ref() {
		return &_oil_function_impl_scalarmult_f64_ref;
}
#endif

