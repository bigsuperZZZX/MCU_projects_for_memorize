/*
 * Copyright(C) 2007, 2013 Renesas Electronics Corporation
 * RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY.
 * This program must be used solely for the purpose for which
 * it was furnished by Renesas Electronics Corporation. No part of this
 * program may be reproduced or disclosed to others, in any
 * form, without the prior written permission of Renesas Electronics
 * Corporation.
 */

/********************************************************************************/
/* RX Family C/C++ Compiler                                                     */
/********************************************************************************/

/* mathf.h standard header */
#ifndef _MATHF
#define _MATHF
#ifndef _MATH
#include <macro.h>

#include <sys/xtgmath.h>
#ifndef _YMATHF
 #include <sys/ymathf.h>
#endif /* _YMATHF */

#include <errno.h>

_C_STD_BEGIN
		/* CODES FOR is* FUNCTIONS */
#define _FP_LT	1
#define _FP_EQ	2
#define _FP_GT	4

		/* CODES FOR ilogb FUNCTIONS */
 #if _ILONG
  #define _FP_ILOGB0	(-0x7fffffff - _C2)
  #define _FP_ILOGBNAN	0x7fffffff

 #else /* _ILONG */
  #define _FP_ILOGB0	(-0x7fff - _C2)
  #define _FP_ILOGBNAN	0x7fff
 #endif /* _ILONG */

 #if _HAS_C9X
		/* TYPES */

		/* MACROS */

 #if 245 < __EDG_VERSION__ && !defined(__cplusplus)
#if defined(__RX) && defined(__DBL4)
  #define HUGE_VAL	__INFINITY__
  #define HUGE_VALF	__INFINITY__
  #define HUGE_VALL	__INFINITY__
#else /* defined(__RX) && defined(__DBL4) */
  #define HUGE_VAL	((double)__INFINITY__)
  #define HUGE_VALF	__INFINITY__
  #define HUGE_VALL	((long double)__INFINITY__)
#endif /* defined(__RX) && defined(__DBL4) */
  #define INFINITY	__INFINITY__
  #define NAN		__NAN__

 #elif defined(__EDG__)
#if defined(__RX) && defined(__DBL4)
  #define HUGE_VAL	_CSTD _FInf._Float
  #define HUGE_VALF	_CSTD _FInf._Float
  #define HUGE_VALL	_CSTD _FInf._Float
#else /* defined(__RX) && defined(__DBL4) */
  #define HUGE_VAL	_CSTD _Inf._Double
  #define HUGE_VALF	_CSTD _FInf._Float
  #define HUGE_VALL	_CSTD _LInf._Long_double
#endif /* defined(__RX) && defined(__DBL4) */
  #define INFINITY	_CSTD _FInf._Float
  #define NAN		_CSTD _FNan._Float

 #else /* !defined(__EDG__) */
  #ifndef _HUGE_ENUF
   #define _HUGE_ENUF	1e+300	/* _HUGE_ENUF*_HUGE_ENUF must overflow */
  #endif /* _HUGE_ENUF */

#if defined(__RX) && defined(__DBL4)
  #define HUGE_VAL	((float)INFINITY)
  #define HUGE_VALF	((float)INFINITY)
  #define HUGE_VALL	((float)INFINITY)
#else /* defined(__RX) && defined(__DBL4) */
  #define HUGE_VAL	((double)INFINITY)
  #define HUGE_VALF	((float)INFINITY)
  #define HUGE_VALL	((long double)INFINITY)
#endif /* defined(__RX) && defined(__DBL4) */
  #define INFINITY	((float)(_HUGE_ENUF * _HUGE_ENUF))
  #define NAN		((float)(INFINITY * 0.0F))
 #endif /* 245 < __EDG_VERSION__ */

 #else /* _IS_C9X */
		/* MACROS */
#if defined(__RX) && defined(__DBL4)
 #define HUGE_VAL	_CSTD _FHugeval._Float
#else /* defined(__RX) && defined(__DBL4) */
 #define HUGE_VAL	_CSTD _Hugeval._Double
#endif /* defined(__RX) && defined(__DBL4) */

#ifdef __RX
 #define HUGE_VALF	_CSTD _FHugeval._Float
#endif /* __RX */

 #endif /* _IS_C9X */

_C_LIB_DECL
		/* float declarations */
float acosf(float);
float asinf(float);
float atanf(float);
float atan2f(float, float);
float ceilf(float);
float expf(float);
float floorf(float);
float fmodf(float, float);
float frexpf(float, long *);
float ldexpf(float, long);
float modff(float, float *);
float powf(float, float);
float tanf(float);
float tanhf(float);

#ifdef __RX
float cosf(float);
float coshf(float);
float logf(float);
float log10f(float);
float sinf(float);
float sinhf(float);
#endif	/* __RX */

float (fabsf)(float);
float (sqrtf)(float);
#ifdef __RX
#ifdef __INTRINSIC_LIB
#define fabsf(f)            _builtin_fabsf(f)
float _builtin_fabsf(float);
#ifdef __ISA_HAS_FSQRT_INST
#ifdef __FPU
#define sqrtf(f)            _builtin_sqrtf(f)
float _builtin_sqrtf(float);
#endif	/* __FPU */
#endif	/* __ISA_HAS_FSQRT_INST */
#endif	/* __INTRINSIC_LIB */
#endif	/* __RX */

 #if _HAS_C9X
float acoshf(float);
float asinhf(float);
float atanhf(float);
float cbrtf(float);
float copysignf(float, float);
float erff(float);
float erfcf(float);
float expm1f(float);
float exp2f(float);
float fdimf(float, float);
float fmaf(float, float, float);
float fmaxf(float, float);
float fminf(float, float);
float hypotf(float, float);
long ilogbf(float);
float lgammaf(float);
_Longlong llrintf(float);
_Longlong llroundf(float);
float log1pf(float);
float logbf(float);
long lrintf(float);
long lroundf(float);
float nanf(const char *);
float nearbyintf(float);
float nextafterf(float, float);
float nexttowardf(float, long double);
float remainderf(float, float);
float remquof(float, float, long *);
float rintf(float);
float roundf(float);
float scalbnf(float, long);
float scalblnf(float, long);
float tgammaf(float);
float truncf(float);
 #endif /* _IS_C9X */

 #if _HAS_TR1_DECLARATIONS
float assoc_laguerref(unsigned, unsigned, float);
float assoc_legendref(unsigned, unsigned, float);
float betaf(float, float);
float comp_ellint_1f(float);
float comp_ellint_2f(float);
float comp_ellint_3f(float, float);
float cyl_bessel_if(float, float);
float cyl_bessel_jf(float, float);
float cyl_bessel_kf(float, float);
float cyl_neumannf(float, float);
float ellint_1f(float, float);
float ellint_2f(float, float);
float ellint_3f(float, float, float);
float expintf(float);
float hermitef(unsigned, float);
float laguerref(unsigned, float);
float legendref(unsigned, float);
float riemann_zetaf(float);
float sph_besself(unsigned, float);
float sph_legendref(unsigned, unsigned, float);
float sph_neumannf(unsigned, float);

 #if _HAS_ADDED_FUNCTIONS
float besseli0f(float);
float besseli1f(float);
float besselj0f(float);
float besselj1f(float);
float besselk0f(float);
float besselk1f(float);
float bessely0f(float);
float bessely1f(float);

double j0f(double);
double j1f(double);
double jnf(double);
double y0f(double);
double y1f(double);
double ynf(double);
 #endif /* _HAS_ADDED_FUNCTIONS */

 #endif /* _HAS_TR1_DECLARATIONS */

_END_C_LIB_DECL

 #if defined(__cplusplus) && !defined(_NO_CPP_INLINES)

		// float INLINES, FOR C++
_C_LIB_DECL
#ifndef __RX
inline float cosf(float _Left)
	{	// return cosine
	return (_FSin(_Left, 1));
	}

inline float coshf(float _Left)
	{	// return hyperbolic cosine
	return (_FCosh(_Left, 1.0F));
	}

inline float logf(float _Left)
	{	// return natural logarithm
	return (_FLog(_Left, 0));
	}

inline float log10f(float _Left)
	{	// return base-10 logarithm
	return (_FLog(_Left, 1));
	}

inline float sinf(float _Left)
	{	// return sine
	return (_FSin(_Left, 0));
	}

inline float sinhf(float _Left)
	{	// return hyperbolic sine
	return (_FSinh(_Left, 1.0F));
	}
#endif /* !__RX */

 #if _HAS_C9X
inline float log2f(float _Left)
	{	// return base-2 logarithm
	return (_FLog(_Left, -1));
	}
 #endif /* _IS_C9X */

_END_C_LIB_DECL

 #else /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */
_C_LIB_DECL
		/* float MACRO OVERRIDES, FOR C */
#ifndef __RX
float cosf(float);
float coshf(float);
float logf(float);
float log10f(float);
float sinf(float);
float sinhf(float);

 #define cosf(x)	_FSin(x, 1)
 #define coshf(x)	_FCosh(x, 1.0F)
 #define logf(x)	_FLog(x, 0)
 #define log10f(x)	_FLog(x, 1)
 #define sinf(x)	_FSin(x, 0)
 #define sinhf(x)	_FSinh(x, 1.0F)
#endif /* !__RX */

 #if _HAS_C9X
float log2f(float);

 #define log2f(x)	_FLog(x, -1)
 #endif /* _IS_C9X */

_END_C_LIB_DECL
 #endif /* defined(__cplusplus) && !defined(_NO_CPP_INLINES) */

_C_STD_END

#endif /* _MATH */
#endif /* _MATHF */

#ifndef _MATH
 #if defined(_STD_USING)

using _CSTD acosf; using _CSTD asinf;
using _CSTD atanf; using _CSTD atan2f; using _CSTD ceilf;
using _CSTD cosf; using _CSTD coshf; using _CSTD expf;
using _CSTD fabsf; using _CSTD floorf; using _CSTD fmodf;
using _CSTD frexpf; using _CSTD ldexpf; using _CSTD logf;
using _CSTD log10f; using _CSTD modff; using _CSTD powf;
using _CSTD sinf; using _CSTD sinhf; using _CSTD sqrtf;
using _CSTD tanf; using _CSTD tanhf;

 #if _HAS_C9X
using _CSTD acoshf; using _CSTD asinhf; using _CSTD atanhf;
using _CSTD cbrtf; using _CSTD erff; using _CSTD erfcf;
using _CSTD expm1f; using _CSTD exp2f;
using _CSTD hypotf; using _CSTD ilogbf; using _CSTD lgammaf;
using _CSTD log1pf; using _CSTD log2f; using _CSTD logbf;
using _CSTD llrintf; using _CSTD lrintf; using _CSTD nearbyintf;
using _CSTD rintf; using _CSTD llroundf; using _CSTD lroundf;
using _CSTD fdimf; using _CSTD fmaf; using _CSTD fmaxf; using _CSTD fminf;
using _CSTD roundf; using _CSTD truncf;
using _CSTD remainderf; using _CSTD remquof;
using _CSTD copysignf; using _CSTD nanf;
using _CSTD nextafterf; using _CSTD scalbnf; using _CSTD scalblnf;
using _CSTD nexttowardf; using _CSTD tgammaf;
 #endif /* _IS_C9X */

 #if _HAS_TR1_DECLARATIONS
using _CSTD assoc_laguerref; using _CSTD assoc_legendref; using _CSTD betaf;
using _CSTD comp_ellint_1f; using _CSTD comp_ellint_2f;
using _CSTD comp_ellint_3f; using _CSTD cyl_bessel_if;
using _CSTD cyl_bessel_jf; using _CSTD cyl_bessel_kf;
using _CSTD cyl_neumannf; using _CSTD ellint_1f; using _CSTD ellint_2f;
using _CSTD ellint_3f; using _CSTD expintf; using _CSTD hermitef;
using _CSTD laguerref; using _CSTD legendref; using _CSTD riemann_zetaf;
using _CSTD sph_besself; using _CSTD sph_legendref; using _CSTD sph_neumannf;

 #if _HAS_ADDED_FUNCTIONS
using _CSTD besseli0f; using _CSTD besseli1f;
using _CSTD besselj0f; using _CSTD besselj1f;
using _CSTD besselk0f; using _CSTD besselk1f;
using _CSTD bessely0f; using _CSTD bessely1f;
using _CSTD j0f; using _CSTD j1f; using _CSTD jnf;
using _CSTD y0f; using _CSTD y1f; using _CSTD ynf;
 #endif /* _HAS_ADDED_FUNCTIONS */

 #endif /* _HAS_TR1_DECLARATIONS */
 #endif /* defined(_STD_USING) */
#endif /* _MATH */

/*
 * Copyright (c) 1992-2007 by P.J. Plauger.  ALL RIGHTS RESERVED.
V5.03:0216 */
