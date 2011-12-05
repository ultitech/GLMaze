#ifndef H_MATHLIB
#define H_MATHLIB

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846 //pi
#endif
#ifndef M_2PI
#define M_2PI 6.28318530717958647692 //pi*2
#endif
#ifndef M_4PI
#define M_4PI 12.56637061435917295384 //pi*4
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923 //pi/2
#endif
#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962 //pi/4
#endif
#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880 //sqrt(2)
#endif
#ifndef M_E
#define M_E 2.71828182845904523536 //e
#endif

#ifndef sqrtf
#define sqrtf(x) ((float)sqrt(x))
#endif
#ifndef powf
#define powf(a, b) ((float)pow(a, b))
#endif
#ifndef sinf
#define sinf(a) ((float)sin(a))
#endif
#ifndef cosf
#define cosf(a) ((float)cos(a))
#endif
#ifndef tanf
#define tanf(a) ((float)tan(a))
#endif
#ifndef asinf
#define asinf(a) ((float)asin(a))
#endif
#ifndef acosf
#define acosf(a) ((float)acos(a))
#endif
#ifndef atanf
#define atanf(a) ((float)atan(a))
#endif
#ifndef atan2f
#define atan2f(a, b) ((float)atan2(a, b))
#endif
#ifndef floorf
#define floorf(a) ((float)floor(a))
#endif
#ifndef ceilf
#define ceilf(a) ((float)ceil(a))
#endif
#ifndef fabsf
#define fabsf(a) ((float)fabs(a))
#endif
#ifndef logf
#define logf(a) ((float)log(a))
#endif
#ifndef expf
#define expf(a) ((float)exp(a))
#endif

void zero_v2(float r[2]);
void zero_v3(float r[3]);

void copy_v2_f(float r[2], float f);
void copy_v3_f(float r[3], float f);
void copy_v2_v2(float r[2], const float a[2]);
void copy_v3_v3(float r[3], const float a[3]);

void swap_v2_v2(float a[2], float b[2]);
void swap_v3_v3(float a[3], float b[3]);

void add_v2_f(float r[2], float f);
void add_v3_f(float r[3], float f);
void add_v2_v2(float r[2], const float a[2]);
void add_v3_v3(float r[3], const float a[3]);
void add_v2_v2v2(float r[2], const float a[2], const float b[2]);
void add_v3_v3v3(float r[3], const float a[3], const float b[3]);

void sub_v2_f(float r[2], float f);
void sub_v3_f(float r[3], float f);
void sub_v2_v2(float r[2], const float a[2]);
void sub_v3_v3(float r[3], const float a[3]);
void sub_v2_v2v2(float r[2], const float a[2], const float b[2]);
void sub_v3_v3v3(float r[3], const float a[3], const float b[3]);

void mul_v2_f(float r[2], float f);
void mul_v3_f(float r[3], float f);
void mul_v2_v2f(float r[2], const float a[2], float f);
void mul_v3_v3f(float r[3], const float a[3], float f);
void mul_v2_v2(float r[2], const float a[2]);
void mul_v3_v3(float r[3], const float a[3]);
void mul_v2_v2v2(float r[2], const float a[2], const float b[2]);
void mul_v3_v3v3(float r[3], const float a[3], const float b[3]);

void negate_v2(float r[2]);
void negate_v3(float r[3]);
void negate_v2_v2(float r[2], const float a[2]);
void negate_v3_v3(float r[3], const float a[3]);

float len_v2(const float a[2]);
float len_v3(const float a[3]);
float len_v2v2(const float a[2], const float b[2]);
float len_v3v3(const float a[3], const float b[3]);

float normalize_v2(float r[2]);
float normalize_v3(float r[3]);
float normalize_v2_v2(float r[2], const float a[2]);
float normalize_v3_v3(float r[3], const float a[3]);

float dot_v2v2(const float a[2], const float b[2]);
float dot_v3v3(const float a[3], const float b[3]);

void cross_v3_v3v3(float r[3], const float a[3], const float b[3]);

void interp_v2_v2v2(float r[2], const float a[2], const float b[2], float f);
void interp_v3_v3v3(float r[3], const float a[3], const float b[3], float f);

void print_v2(const char *str, const float a[2]);
void print_v3(const char *str, const float a[3]);

#define INDEX_M3(i,j) (((j)-1)*3+((i)-1))
#define INDEX_M4(i,j) (((j)-1)*4+((i)-1))

void copy_m3_m3(float R[9], float A[9]);
void copy_m4_m4(float R[16], float A[16]);
void copy_m3_m4(float R[9], float A[16]);
void copy_m4_m3(float R[16], float A[9]);

void swap_m3m3(float A[9], float B[9]);
void swap_m4m4(float A[16], float B[16]);

void add_m3_m3m3(float R[9], float A[9], float B[9]);
void add_m4_m4m4(float R[16], float A[16], float B[16]);

void mul_m3_m3m3(float R[9], float A[9], float B[9]);
void mul_m4_m4m4(float R[16], float A[16], float B[16]);
void mul_m4_m3m4(float R[16], float A[9], float B[16]);
void mul_m4_m4m3(float R[16], float A[16], float B[9]);
void mul_m3_m3m4(float R[9], float A[9], float B[16]);
void mul_m3_m3(float R[9], float A[9]);
void mul_m4_m4(float R[16], float A[16]);

void transpose_m3(float R[9]);
void transpose_m4(float R[16]);

void translate_m4(float A[16], float x, float y, float z);
void rotate_m4(float A[16], float a, float x, float y, float z);
void scale_m4(float A[16], float x, float y, float z);

void create_zero_m3(float R[9]);
void create_zero_m4(float R[16]);
void create_identity_m3(float R[9]);
void create_identity_m4(float R[16]);
void create_translate_m4(float R[16], float x, float y, float z);
void create_rotate_m4(float R[16], float a, float x, float y, float z);
void create_scale_m4(float R[16], float x, float y, float z);
void create_ortho_m4(float R[16], float l, float r, float b, float t, float n, float f);
void create_ortho2d_m4(float R[16], float l, float r, float b, float t);
void create_frustum_m4(float R[16], float l, float r, float b, float t, float n, float f);
void create_perspective_m4(float R[16], float fov, float aspect, float near, float far);

void print_m3(const char *str, float A[9]);
void print_m4(const char *str, float A[16]);

#endif //H_MATHLIB
