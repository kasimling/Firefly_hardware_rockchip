/*
 * CONFIDENTIAL
 * Copyright(c) 2014 Yamaha Corporation
 */
#ifndef __YAS_FLOAT_H__
#define __YAS_FLOAT_H__

#include <stdint.h>
#include <math.h>

#ifdef _WIN32
#define inline __inline
#endif

#if defined(__CC_ARM)
#include "arm_math.h"
#include "arm_common_tables.h"
#define inline __INLINE
#endif

#undef __ARM_LIB_SRC__

#ifdef MSCALIB_TYPES_USE_DOUBLE
# define msfloat_sqrt sqrt
# define msfloat_pow pow
# define msfloat_acos acos
# define msfloat_asin asin
# define msfloat_cos cos
# define msfloat_sin sin
# define msfloat_fabs fabs
# define msfloat_atan2 atan2
#else
# define msfloat_sqrt sqrtf
# define msfloat_pow powf
# define msfloat_acos acosf
# define msfloat_asin asinf
# define msfloat_cos cosf
# define msfloat_sin sinf
# define msfloat_fabs fabsf
# define msfloat_atan2 atan2f
#endif

typedef int32_t sfixed;

#ifdef MSCALIB_TYPES_USE_DOUBLE
typedef double msfloat;
#else
typedef float msfloat;
#endif

struct Vector3D {
	sfixed x[3];
};

struct Vector3D_Int {
	int32_t x[3];
};

struct Sym3Matrix {
	sfixed v[6];
};

#define Vector_Clear(p, size) \
	{ int _l; for (_l = 0; _l < (size); _l++) (p)[_l] = FIXED_Q31_0; }

#define Vector_Clear_Int(p, size) \
	{ int _l; for (_l = 0; _l < (size); _l++) (p)[_l] = 0; }

#define Sym3Matrix_Clear(p) \
	{ int _l; for (_l = 0; _l < 6; _l++) (p)->v[_l] = FIXED_Q31_0; }

#if !defined(__CC_ARM)
#define USE_ASSERT
#endif

#if 0
#ifdef USE_ASSERT
#include <assert.h>
#define ASSERT(x) assert(x)
#else
#define ASSERT(x) ((void)0)
#endif
#else
#define ASSERT(x) ((void)0)
#endif

#define USE_FIXED

#define FIXED_BIT		31
#define FIXED_Q31		31

#define SFIXED_ISET32_MACRO(val) (sfixed)((val) * 1LL << FIXED_BIT)
#define SFIXED_FSET32_MACRO(val) \
	(sfixed)((msfloat)(val) * (msfloat)(1LL << FIXED_BIT) + \
			(((val) >= 0.0) ?  0.5 : -0.5))

#define SFIXED_SET32(val) SFIXED_SET((val), FIXED_BIT)
#define SFIXED_ISET32(val) SFIXED_ISET((val), FIXED_BIT)
#define SFIXED_GET32(a) SFIXED_GET((a), FIXED_BIT)
#define SFIXED_IGET32(a) SFIXED_IGET((a), FIXED_BIT)
#define SFIXED_FSET32(a) ((sfixed)((a) * (1ULL<<FIXED_BIT) + \
			(((a) >= 0.0) ? 0.5 : -0.5)))
#define SFIXED_FGET32(a) ((msfloat)(a) / (1ULL<<FIXED_BIT))
#define SFIXED_ADD32(a, b) SFIXED_CLIP_Q31((int64_t)(a) + (b))
#define SFIXED_ADD32_3(a, b, c) SFIXED_CLIP_Q31((int64_t)(a) + (b) + (c))
#define SFIXED_ADD32_4(a, b, c, d) \
	SFIXED_CLIP_Q31((int64_t)(a) + (b) + (c) + (d))
#define SFIXED_HADD32(a, b) SFIXED_HADD((a), (b))
#define SFIXED_SUB32(a, b) SFIXED_CLIP_Q31((int64_t)(a) - (b))
#define SFIXED_HSUB32(a, b) SFIXED_HSUB((a), (b))
#define SFIXED_MUL32(a, b) \
	SFIXED_CLIP_Q31(((int64_t)(a) * (int64_t)(b)) >> FIXED_BIT)
#define SFIXED_DIV32(a, b) (sfixed)(((int64_t)(a) << FIXED_BIT) / (int64_t)(b))
#if defined(__CC_ARM)
#define SFIXED_RECIP_Q31(src, dst) \
	arm_recip_q31((src), (dst), ((q31_t *)armRecipTableQ31))
#else
#define SFIXED_RECIP_Q31(src, dst) SFIXED_RECIP_Q31_((src), (dst))
#endif
#if defined(__CC_ARM)
#define SFIXED_CLIP_Q31(a) clip_q63_to_q31((a))
#else
#define SFIXED_CLIP_Q31(a) SFIXED_CLIP_Q31_((a))
#endif
#if defined(__CC_ARM)
#define SFIXED_CLIP_Q31_NOCHECK(a) clip_q63_to_q31((a))
#else
#define SFIXED_CLIP_Q31_NOCHECK(a) SFIXED_CLIP_Q31_NOCHECK_((a))
#endif
#define SFIXED_IGET_MUL_Q31(a, b) SFIXED_MUL((a), (b), 0, 31, 0)
#define SFIXED_ISET_DIV_Q31(a, b) SFIXED_DIV((a), (b), 31, 0, 0)
#define SFIXED_ISET_MUL_Q31(a, b, bit) SFIXED_MUL((a), (b), 31, 0, 62 - (bit))

#ifdef USE_FIXED
#define FIXED_EP		0x40000

#define FIXED_Q31_1			0x7FFFFFFF
#define FIXED_Q31_1_DIV_2	0x40000000
#define FIXED_Q31_1_DIV_4	0x20000000
#define FIXED_Q31_0			0
#define FIXED_Q31_MINUS_1	0x80000000

static inline sfixed SFIXED_CLIP_Q31_(int64_t val)
{
	ASSERT(val <= 0x7FFFFFFFLL + FIXED_EP
			&& val >= -0x80000000LL - FIXED_EP);

	if (val > 0x7FFFFFFFLL)
		return 0x7FFFFFFF;
	if (val < -0x80000000LL)
		return 0x80000000;

	return (sfixed)val;
}

static inline sfixed SFIXED_CLIP_Q31_NOCHECK_(int64_t val)
{
	if (val > 0x7FFFFFFFLL)
		return 0x7FFFFFFF;
	if (val < -0x80000000LL)
		return 0x80000000;
	return (sfixed)val;
}

static inline sfixed SFIXED_MUL(sfixed a, sfixed b, int frac, int a_frac,
		int b_frac)
{
	int64_t tmp;
	int shift;

	tmp = (int64_t)a * b;
	shift = frac - (a_frac + b_frac);
	if (shift > 0)
		tmp = tmp << shift;
	else
		tmp = tmp >> -shift;

	return SFIXED_CLIP_Q31(tmp);
}

static inline sfixed SFIXED_DIV(sfixed a, sfixed b, int frac, int a_frac,
		int b_frac)
{
	int64_t tmp;
	int shift;

	tmp = ((int64_t)a << 32) / b;
	shift = frac - (a_frac - b_frac + 32);
	if (shift > 0)
		tmp <<= shift;
	else
		tmp >>= -shift;

	return SFIXED_CLIP_Q31(tmp);
}
#ifdef __ARM_LIB_SRC__

static inline uint32_t __CLZ(sfixed data)
{
	uint32_t count = 0;
	uint32_t mask = 0x80000000;
	while ((data & mask) == 0) {
		count += 1u;
		mask = mask >> 1u;
	}
	return count;
}


extern const sfixed armRecipTableQ31[64];
#define INDEX_MASK			0x0000003F

static inline int32_t SFIXED_RECIP_Q31_(sfixed in, sfixed *dst)
{
	const sfixed *pRecipTable = armRecipTableQ31;
	uint32_t out, tempVal;
	uint32_t index, i;
	uint32_t signBits;

	if (in > 0)
		signBits = __CLZ(in) - 1;
	else
		signBits = __CLZ(-in) - 1;

	/* Convert input sample to 1.31 format */
	in = in << signBits;

	/* calculation of index for initial approximated Val */
	index = (uint32_t) (in >> 24u);
	index = (index & INDEX_MASK);

	/* 1.31 with exp 1 */
	out = pRecipTable[index];

	/* calculation of reciprocal value */
	/* running approximation for two iterations */
	for (i = 0u; i < 2u; i++) {
		tempVal = (sfixed) (((int64_t) in * out) >> 31u);
		tempVal = 0x7FFFFFFF - tempVal;
		/*      1.31 with exp 1 */
		/*out = (sfixed) (((int64_t) out * tempVal) >> 30u);*/
		out = (sfixed)SFIXED_CLIP_Q31(((int64_t) out * tempVal) >> 30u);
	}

	/* write output */
	*dst = out;

	/* return num of signbits of out = 1/in value */
	return signBits + 1u;

}

#else

static inline int32_t SFIXED_RECIP_Q31_(sfixed src, sfixed *dst)
{
	int64_t tmp;
	int shift, msb;

	if (src == 0) {
		*dst = 0x7fffffff;
		return 31;
	}

	shift = 16;
	msb = 0;

	do {
		if (src >= (int64_t)(1ULL << (msb + shift)))
			msb += shift;
		shift /= 2;
	} while (shift >= 1);

	tmp = (int64_t)(1ULL<<62) / src;

	tmp = tmp >> (31 - msb);

	*dst = SFIXED_CLIP_Q31(tmp);

	return 31 - msb;
}

#endif

#define SFIXED_SET(val, frac)	((sfixed) val)
#define SFIXED_ISET(val, frac)	(SFIXED_CLIP_Q31((int64_t)(val) << (frac)))
#define SFIXED_FSET(val, frac)	(SFIXED_CLIP_Q31((int64_t)((val) * \
			 (1ULL<<(frac)) + (((val) >= 0.0) ? 0.5 : -0.5))))

#define SFIXED_GET(a, frac)	((int32_t)a)
#define SFIXED_IGET(a, frac)	((int32_t)((a) >> (frac)))
#define SFIXED_FGET(a, frac)	((msfloat)((msfloat)(a) / (1ULL << (frac))))

#define SFIXED_ADD(a, b)	SFIXED_CLIP_Q31((int64_t)(a) + (b))
#define SFIXED_HADD(a, b)	(((a) >> 1) + ((b) >> 1))
#define SFIXED_SUB(a, b)	SFIXED_CLIP_Q31((int64_t)(a) - (b))
#define SFIXED_HSUB(a, b)	(((a) >> 1) - ((b) >> 1))

#define SFIXED_IMUL(a, b)	((a) * (b))
#define SFIXED_IMUL_Q(a, b)	SFIXED_CLIP_Q31((int64_t)(a) * (b))
#define SFIXED_IDIV(a, b)	((a) / (b))
#define SFIXED_NEG(a) ((a) == (sfixed) 0x80000000 ? (sfixed) 0x7FFFFFFF : -(a))
#define SFIXED_ABS(a) ((a) == (sfixed) 0x80000000 ? (sfixed) 0x7FFFFFFF : \
		(a) < 0 ? -(a) : (a))

#define SFIXED_LSFT(a, b)	((a) << (b))
#define SFIXED_LSFT_CLIP(a, b)	SFIXED_CLIP_Q31((a) << (b))
#define SFIXED_RSFT(a, b)	((a) >> (b))

#else
#define sfixed msfloat

#define SFIXED_SET(val, frac)	((msfloat)(val))
#define SFIXED_ISET(val, frac)	((msfloat)(val))
#define SFIXED_FSET(val, frac)	((msfloat)(val))

#define SFIXED_GET(val, frac)	((msfloat)(val))
#define SFIXED_IGET(val, frac)	((int64_t)(val))
#define SFIXED_FGET(val, frac)	((msfloat)(val))

#define SFIXED_ADD(a, b)	((a) + (b))
#define SFIXED_SUB(a, b)	((a) - (b))
#define SFIXED_MUL(a, b, frac, a_frac, b_frac) ((a) * (b))
#define SFIXED_DIV(a, b, frac, a_frac, b_frac) ((a) / (b))
#define SFIXED_IMUL(a, b)	((a) * (b))
#define SFIXED_IDIV(a, b)	((a) / (b))
#define SFIXED_NEG(a)		(-(a))
#define SFIXED_ABS(a)		(msfloat_fabs(a))
#endif

#endif /* __YAS_FLOAT_H__ */
