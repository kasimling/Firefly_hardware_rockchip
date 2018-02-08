/*
 * CONFIDENTIAL
 * Copyright(c) 2014 Yamaha Corporation
 */
#ifndef __YAS_MATH_H__
#define __YAS_MATH_H__
#include "yas_float.h"
#include "yas.h"

#define USE_MATH_FIXED

extern const uint8_t sym_idx_tbl[9][9];
extern const uint8_t sym_diag_idx_tbl[9];

#define sym_idx(row, column) ((int)(sym_idx_tbl[(int)row][(int)column]))
#define sym_diag_idx(row_column) ((int)(sym_diag_idx_tbl[(int)row_column]))

#ifdef USE_FIXED
#ifdef USE_MATH_FIXED
#if defined(__CC_ARM)
static inline sfixed yas_math_sqrt_fixed_q31(sfixed x)
{
	sfixed y;

	arm_sqrt_q31(x, &y);

	return y;
}
#define yas_math_sin_fixed_q31(x) arm_sin_q31((x))
#define yas_math_cos_fixed_q31(x) arm_cos_q31((x))
#define yas_math_sin_cos_fixed_q31(x, s, c) arm_sin_cos_q31((x), (s), (c))
#else
#ifdef __ARM_LIB_SRC__
sfixed yas_math_sqrt_fixed_q31(sfixed in);
sfixed yas_math_sin_fixed_q31(sfixed x);
sfixed yas_math_cos_fixed_q31(sfixed x);
void yas_math_sin_cos_fixed_q31(sfixed theta, sfixed *pSinVal, sfixed *pCosVal);
#else
#define yas_math_sqrt_fixed_q31(x) \
	SFIXED_FSET(msfloat_sqrt(SFIXED_FGET((x), FIXED_Q31)), FIXED_Q31)
#define yas_math_sin_fixed_q31(x) \
	SFIXED_FSET(msfloat_sin(SFIXED_FGET((x), FIXED_Q31) * (2.0 * M_PI)), \
			FIXED_Q31)
#define yas_math_cos_fixed_q31(x) \
	SFIXED_FSET(msfloat_cos(SFIXED_FGET((x), FIXED_Q31) * (2.0 * M_PI)), \
			FIXED_Q31)
#define yas_math_sin_cos_fixed_q31(x, s, c) \
	(*(s) = SFIXED_FSET(msfloat_sin(SFIXED_FGET((x), FIXED_Q31) \
					* (1.0 * M_PI)), FIXED_Q31), \
	 *(c) = SFIXED_FSET(msfloat_cos(SFIXED_FGET((x), FIXED_Q31) \
			 * (1.0 * M_PI)), FIXED_Q31))
#endif
#endif
sfixed yas_math_atan2_fixed_q31(sfixed y, sfixed x);
sfixed yas_math_asin_fixed_q31(sfixed x);
sfixed yas_math_acos_fixed_q31(sfixed x);
#define yas_math_sqrt_fixed(x, frac, x_frac) yas_math_sqrt_fixed_q31((x))
#define yas_math_atan2_fixed(y, x, frac, xy_frac) \
	yas_math_atan2_fixed_q31((y), (x))
#define yas_math_asin_fixed(x, frac, x_frac) yas_math_asin_fixed_q31((x))
#define yas_math_acos_fixed(x, frac, x_frac) yas_math_acos_fixed_q31((x))
#define yas_math_sin_fixed(x, frac, x_frac) yas_math_sin_fixed_q31((x))
#define yas_math_cos_fixed(x, frac, x_frac) yas_math_cos_fixed_q31((x))
#define yas_math_sin_cos_fixed(x, s, c, frac, x_frac) \
	yas_math_sin_cos_fixed_q31((x), (s), (c))
#else
#define yas_math_sqrt_fixed(x, frac, x_frac) \
	SFIXED_FSET(msfloat_sqrt(SFIXED_FGET((x), (x_frac))), (frac))
#define yas_math_atan2_fixed(y, x, frac, xy_frac) \
	SFIXED_FSET(msfloat_atan2(SFIXED_FGET(y, (xy_frac)), SFIXED_FGET((x), \
					(xy_frac))) / (2.0 * M_PI), (frac))
#define yas_math_asin_fixed(x, frac, x_frac) \
	SFIXED_FSET(msfloat_asin(SFIXED_FGET((x), (x_frac))) / (2.0 * M_PI), \
			(frac))
#define yas_math_acos_fixed(x, frac, x_frac) \
	SFIXED_FSET(msfloat_acos(SFIXED_FGET((x), (x_frac))) / (2.0 * M_PI), \
			(frac))
#define yas_math_sin_fixed(x, frac, x_frac) \
	SFIXED_FSET(msfloat_sin(SFIXED_FGET((x), (x_frac)) * (2.0 * M_PI)), \
			(frac))
#define yas_math_cos_fixed(x, frac, x_frac) \
	SFIXED_FSET(msfloat_cos(SFIXED_FGET((x), (x_frac)) * (2.0 * M_PI)), \
			(frac))
#endif
#else
#define yas_math_sqrt_fixed(x, frac, x_frac)		msfloat_sqrt((x))
#define yas_math_atan2_fixed(y, x, frac, xy_frac)	msfloat_atan2(y, (x))
#define yas_math_asin_fixed(x, frac, x_frac)		msfloat_asin((x))
#define yas_math_acos_fixed(x, frac, x_frac)		msfloat_acos((x))
#define yas_math_sin_fixed(x, frac, x_frac)		msfloat_sin((x))
#define yas_math_cos_fixed(x, frac, x_frac)		msfloat_cos((x))
#define yas_math_sin_cos_fixed(x, s, c, frac, x_frac)	\
	(*(s) = msfloat_sin((x)), *(c) = msfloat_cos((x)))
#endif

#if 0
#define YAS_MATHLIB_ARITHMETIC_SHIFT /* Default for other than gcc. */
#ifdef __GNUC__
# define YAS_MATHLIB_ARITHMETIC_SHIFT
#endif
#endif

# define SHIFT_L(a, b) ((a) << (b))
# define SHIFT_R_S(a, b) (((a) >= 0) ? ((a) >> (b)) : -((SFIXED_ABS(a)) >> (b)))

#define SHIFT_R(a, b) (((a) >= 0) ? ((a) >> (b)) : -((-a) >> (b)))

#if 0
#ifdef YAS_MATHLIB_ARITHMETIC_SHIFT
# define SHIFT_R(a, b) ((a) >> (b))
#else
# define SHIFT_R(a, b) SHIFT_R_S(a, b)
#endif
#endif

struct vector_3d {
	int32_t x[3];
};

struct vector_3d16 {
	int16_t x[3];
};

struct sym3_matrix {
	int32_t v[6];
};


#define YAS_MATH_SQRT_FIXED32(x) yas_math_sqrt_fixed((x), FIXED_BIT, FIXED_BIT)
#define YAS_MATH_ATAN2_FIXED32(y, x) \
	yas_math_atan2_fixed((y), (x), FIXED_BIT, FIXED_BIT)
#define YAS_MATH_ASIN_FIXED32(x) \
	yas_math_asin_fixed((x), FIXED_BIT, FIXED_BIT)
#define YAS_MATH_ACOS_FIXED32(x) \
	yas_math_acos_fixed((x), FIXED_BIT, FIXED_BIT)
#define YAS_MATH_SIN_FIXED32(x) \
	yas_math_sin_fixed((x), FIXED_BIT, FIXED_BIT)
#define YAS_MATH_COS_FIXED32(x) \
	yas_math_cos_fixed((x), FIXED_BIT, FIXED_BIT)
#define YAS_MATH_SIN_COS_FIXED32(x, s, c) \
	yas_math_sin_cos_fixed((x), (s), (c), FIXED_BIT, FIXED_BIT)

int32_t yas_sqrt32(int32_t v);
int yas_sin_cos32(int32_t z, int32_t *s, int32_t *c);

#if YAS_MAG_CALIB_ENABLE && !YAS_MAG_CALIB_MINI_ENABLE \
	&& !YAS_MAG_CALIB_FLOAT_ENABLE
int64_t yas_sqrt64(int64_t x);
int64_t yas_sqrt64_approx(int64_t x);

/*!
  @brief
  Calculates eigenvalues.
  @param[in, out] x
  A matrix. The result is overwitten and eigenvalues appears
  on the diagonal components.
  "mat_{ij}" before calling this function is expected to be 15bit or less.
  The matrix is a symmetric 3x3 matrix.
  @retval 0
  Success.
  @retval "Negative numbers"
  Failure.
*/
int yas_jacobi32(int32_t *mat);

/*!
  @brief
  Solves an linear equation "L * x = b" for x.
  @param[in] L
  A pointer to a matrix. The matrix is a lower triangular matrix.
  @param[in,out] b
  A pointer to a vector. The result is overwritten to this vector.
  This is in int64_t-type.
  @param[in] size
  The size of the vector.
  @retval "Positive number"
  Success but input matrix and vector are not suitable.
  @retval 0
  Success.
  @retval "Negative number"
  Failure.
*/
int yas_solve_Lx64(const int32_t *L, int64_t *b, int_fast8_t size);

/*!
  @brief
  Solves an linear equation "U * x = b" for x.
  @param[in] U
  A pointer to a matrix. The matrix is a upper triangular matrix.
  @param[in,out] b
  A pointer to a vector. The result is overwritten to this vector.
  This is in int64_t-type.
  @param[in] size
  The size of the vector.
  @retval "Positive number"
  Success but input matrix and vector are not suitable.
  @retval 0
  Success.
  @retval "Negative number"
  Failure.
*/
int yas_solve_Ux64(const int32_t *U, int64_t *b, int_fast8_t size);

/*!
  @brief
  Solves an linear equation "L * U * x = b" for x.
  @param[in] U
  A pointer to a matrix. The matrix is refferred as both "L" and "U".
  @param[in,out] b
  A pointer to a vector. The result is overwritten to this vector.
  This is in int64_t-type.
  @param[in] size
  The size of the vector.
  @retval "Positive number"
  Success but input matrix and vector are not suitable.
  @retval 0
  Success.
  @retval "Negative number"
  Failure.
*/
int yas_solve_LUx64(const int32_t *U, int64_t *b, int_fast8_t size);

/*!
  @brief
  Solves an linear equation "L * x = b" for x.
  @param[in] L
  A pointer to a matrix. The matrix is a upper triangular matrix.
  @param[in,out] b
  A pointer to a vector. The result is overwritten to this vector.
  @param[in] size
  The size of the vector.
  @retval "Positive number"
  Success but input matrix and vector are not suitable.
  @retval 0
  Success.
  @retval "Negative number"
  Failure.
*/
int yas_solve_Lx32(const int32_t *L, int32_t *b, int_fast8_t size);

/*!
  @brief
  Solves an linear equation "U * x = b" for x.
  @param[in] U
  A pointer to a matrix. The matrix is a upper triangular matrix.
  @param[in,out] b
  A pointer to a vector. The result is overwritten to this vector.
  @param[in] size
  The size of the vector.
  @retval "Positive number"
  Success but input matrix and vector are not suitable.
  @retval 0
  Success.
  @retval "Negative number"
  Failure.
*/
int yas_solve_Ux32(const int32_t *U, int32_t *b, int_fast8_t size);

/*!
  @brief
  Solves an linear equation "L * U * x = b" for x.
  @param[in] U
  A pointer to a matrix. The matrix is refferred as both "L" and "U".
  @param[in,out] b
  A pointer to a vector. The result is overwritten to this vector.
  @param[in] size
  The size of the vector.
  @retval "Positive number"
  Success but input matrix and vector are not suitable.
  @retval 0
  Success.
  @retval "Negative number"
  Failure.
*/
int yas_solve_LUx32(const int32_t *U, int32_t *b, int_fast8_t size);

/*!
  @brief
  Calculates cholesky decomposition.
  @param[in,out] mat
  A pointer to a matrix. Result is overwritten to the matrix.
  The matrix is symmetric.
  @param[in] size
  The number of row or column of the matrix.
  Currently the number is intended to be 2, 3, 7, or 9.
  @retval "Positive number"
  Success but input matrix is not suitable.
  @retval 0
  Success.
  @retval "Negative number"
  Failure.
*/
int yas_cholesky_decomposition32(int32_t *mat, int_fast8_t size);
#endif

int yas_get_matrix(struct yas_vector *acc, struct yas_vector *mag, sfixed *m);
int yas_get_quaternion(sfixed *matrix, struct yas_quaternion *q);
#if YAS_ORIENTATION_ENABLE
int yas_get_orientation(sfixed *matrix, struct yas_vector *orientation);
#endif

#endif /* __YAS_MATH_H__ */
