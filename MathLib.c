#include "MathLib.h"

#include <stdio.h>
#include <string.h>

void zero_v2(float r[2])
{
	r[0] = r[1] = 0.0f;
}
void zero_v3(float r[3])
{
	r[0] = r[1] = r[2] = 0.0f;
}

void copy_v2_f(float r[2], float f)
{
	r[0] = r[1] = f;
}
void copy_v3_f(float r[3], float f)
{
	r[0] = r[1] = r[2] = f;
}
void copy_v2_v2(float r[2], const float a[2])
{
	r[0] = a[0];
	r[1] = a[1];
}
void copy_v3_v3(float r[3], const float a[3])
{
	r[0] = a[0];
	r[1] = a[1];
	r[2] = a[2];
}

void swap_v2_v2(float a[2], float b[2])
{
	float temp[2];
	copy_v2_v2(temp, b);
	copy_v2_v2(b, a);
	copy_v2_v2(a, temp);
}
void swap_v3_v3(float a[3], float b[3])
{
	float temp[3];
	copy_v3_v3(temp, b);
	copy_v3_v3(b, a);
	copy_v3_v3(a, temp);
}

void add_v2_f(float r[2], float f)
{
	r[0] += f;
	r[1] += f;
}
void add_v3_f(float r[3], float f)
{
	r[0] += f;
	r[1] += f;
	r[2] += f;
}
void add_v2_v2(float r[2], const float a[2])
{
	r[0] += a[0];
	r[1] += a[1];
}
void add_v3_v3(float r[3], const float a[3])
{
	r[0] += a[0];
	r[1] += a[1];
	r[2] += a[2];
}
void add_v2_v2v2(float r[2], const float a[2], const float b[2])
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
}
void add_v3_v3v3(float r[3], const float a[3], const float b[3])
{
	r[0] = a[0] + b[0];
	r[1] = a[1] + b[1];
	r[2] = a[2] + b[2];
}

void sub_v2_f(float r[2], float f)
{
	r[0] -= f;
	r[1] -= f;
}
void sub_v3_f(float r[3], float f)
{
	r[0] -= f;
	r[1] -= f;
	r[2] -= f;
}
void sub_v2_v2(float r[2], const float a[2])
{
	r[0] -= a[0];
	r[1] -= a[1];
}
void sub_v3_v3(float r[3], const float a[3])
{
	r[0] -= a[0];
	r[1] -= a[1];
	r[2] -= a[2];
}
void sub_v2_v2v2(float r[2], const float a[2], const float b[2])
{
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
}
void sub_v3_v3v3(float r[3], const float a[3], const float b[3])
{
	r[0] = a[0] - b[0];
	r[1] = a[1] - b[1];
	r[2] = a[2] - b[2];
}

void mul_v2_f(float r[2], float f)
{
	r[0] *= f;
	r[1] *= f;
}
void mul_v3_f(float r[3], float f)
{
	r[0] *= f;
	r[1] *= f;
	r[2] *= f;
}
void mul_v2_v2f(float r[2], const float a[2], float f)
{
	r[0] = a[0] * f;
	r[1] = a[1] * f;
}
void mul_v3_v3f(float r[3], const float a[3], float f)
{
	r[0] = a[0] * f;
	r[1] = a[1] * f;
	r[2] = a[2] * f;
}
void mul_v2_v2(float r[2], const float a[2])
{
	r[0] *= a[0];
	r[1] *= a[1];
}
void mul_v3_v3(float r[3], const float a[3])
{
	r[0] *= a[0];
	r[1] *= a[1];
	r[2] *= a[2];
}
void mul_v2_v2v2(float r[2], const float a[2], const float b[2])
{
	r[0] = a[0] * b[0];
	r[1] = a[1] * b[1];
}
void mul_v3_v3v3(float r[3], const float a[3], const float b[3])
{
	r[0] = a[0] * b[0];
	r[1] = a[1] * b[1];
	r[2] = a[2] * b[2];
}

void negate_v2(float r[2])
{
	mul_v2_f(r, -1.0f);
}
void negate_v3(float r[3])
{
	mul_v3_f(r, -1.0f);
}
void negate_v2_v2(float r[2], const float a[2])
{
	mul_v2_v2f(r, a, -1.0f);
}
void negate_v3_v3(float r[3], const float a[3])
{
	mul_v3_v3f(r, a, -1.0f);
}

float len_v2(const float a[2])
{
	return sqrtf(a[0]*a[0] + a[1]*a[1]);
}
float len_v3(const float a[3])
{
	return sqrtf(a[0]*a[0] + a[1]*a[1] + a[2]*a[2]);
}
float len_v2v2(const float a[2], const float b[2])
{
	float temp[2];
	sub_v2_v2v2(temp, b, a);
	return len_v2(temp);
}
float len_v3v3(const float a[3], const float b[3])
{
	float temp[3];
	sub_v3_v3v3(temp, b, a);
	return len_v3(temp);
}

float normalize_v2(float r[2])
{
	normalize_v2_v2(r, r);
}
float normalize_v3(float r[3])
{
	return normalize_v3_v3(r, r);
}
float normalize_v2_v2(float r[2], const float a[2])
{
	float d = dot_v2v2(a, a);
	
	if(d > 0.0f)
	{
		d = sqrtf(d);
		mul_v2_v2f(r, a, 1.0f/d);
	}
	else
	{
		d = 0.0f;
		zero_v2(r);
	}
	
	return d;
}
float normalize_v3_v3(float r[3], const float a[3])
{
	float d = dot_v3v3(a, a);
	
	if(d > 0.0f)
	{
		d = sqrtf(d);
		mul_v3_v3f(r, a, 1.0f/d);
	}
	else
	{
		d = 0.0f;
		zero_v3(r);
	}
	
	return d;
}

float dot_v2v2(const float a[2], const float b[2])
{
	return a[0]*b[0] + a[1]*b[1];
}
float dot_v3v3(const float a[3], const float b[3])
{
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

void cross_v3_v3v3(float r[3], const float a[3], const float b[3])
{
	r[0] = a[1]*b[2] - a[2]*b[1];
	r[1] = a[2]*b[0] - a[0]*b[2];
	r[2] = a[0]*b[1] - a[1]*b[0];
}

void interp_v2_v2v2(float r[2], const float a[2], const float b[2], float f)
{
	r[0] = (1.0f-f)*a[0] + f*b[0];
	r[1] = (1.0f-f)*a[1] + f*b[1];
}
void interp_v3_v3v3(float r[3], const float a[3], const float b[3], float f)
{
	r[0] = (1.0f-f)*a[0] + f*b[0];
	r[1] = (1.0f-f)*a[1] + f*b[1];
	r[2] = (1.0f-f)*a[2] + f*b[2];
}

void print_v2(const char *str, const float a[2])
{
	printf("%s: %.3f %.3f\n", str, a[0], a[1]);
}
void print_v3(const char *str, const float a[3])
{
	printf("%s: %.3f %.3f %.3f\n", str, a[0], a[1], a[2]);
}

void copy_m3_m3(float R[9], float A[9])
{
	memcpy(R, A, 3*3*sizeof(float));
}
void copy_m4_m4(float R[16], float A[16])
{
	memcpy(R, A, 4*4*sizeof(float));
}
void copy_m3_m4(float R[9], float A[16])
{
	int i, j;
	for(i=1; i<=3; i++) for(j=1; j<=3; j++) R[INDEX_M3(i,j)] = A[INDEX_M4(i,j)];
}
void copy_m4_m3(float R[16], float A[9])
{
	int i, j;
	for(i=1; i<=4; i++) for(j=1; j<=4; j++)
	{
		if(i==4 && j==4) R[INDEX_M4(i,j)] = 1.0;
		else if(i==4 || j==4) R[INDEX_M4(i,j)] = 0.0;
		else R[INDEX_M4(i,j)] = A[INDEX_M3(i,j)];
	}
}

void swap_m3m3(float A[9], float B[9])
{
	float t;
	int i, j;
	for(i=1; i<=3; i++) for(j=1; i<=3; j++)
	{
		t = A[INDEX_M3(i,j)];
		A[INDEX_M3(i,j)] = B[INDEX_M3(i,j)];
		B[INDEX_M3(i,j)] = t;
	}
}
void swap_m4m4(float A[16], float B[16])
{
	float t;
	int i, j;
	for(i=1; i<=4; i++) for(j=1; i<=4; j++)
	{
		t = A[INDEX_M4(i,j)];
		A[INDEX_M4(i,j)] = B[INDEX_M4(i,j)];
		B[INDEX_M4(i,j)] = t;
	}
}

void add_m3_m3m3(float R[9], float A[9], float B[9])
{
	int i, j;
	for(i=1; i<=3; i++) for(j=1; j<=3; j++) R[INDEX_M3(i,j)] = A[INDEX_M3(i,j)] + B[INDEX_M3(i,j)];
}
void add_m4_m4m4(float R[16], float A[16], float B[16])
{
	int i, j;
	for(i=1; i<=4; i++) for(j=1; j<=4; j++) R[INDEX_M4(i,j)] = A[INDEX_M4(i,j)] + B[INDEX_M4(i,j)];
}

void mul_m3_m3m3(float R[9], float A[9], float B[9])
{
	int i, j, n;
	float sum, temp[9];
	for(i=1; i<=3; i++) for(j=1; j<=3; j++)
	{
		sum = 0.0;
		for(n=1; n<=3; n++) sum += A[INDEX_M3(i,n)] * B[INDEX_M3(n,j)];
		temp[INDEX_M3(i,j)] = sum;
	}
	copy_m3_m3(R, temp);
}
void mul_m4_m4m4(float R[16], float A[16], float B[16])
{
	int i, j, n;
	float sum, temp[16];
	for(i=1; i<=4; i++) for(j=1; j<=4; j++)
	{
		sum = 0.0;
		for(n=1; n<=4; n++) sum += A[INDEX_M4(i,n)] * B[INDEX_M4(n,j)];
		temp[INDEX_M4(i,j)] = sum;
	}
	copy_m4_m4(R, temp);
}
void mul_m4_m3m4(float R[16], float A[9], float B[16]);
void mul_m4_m4m3(float R[16], float A[16], float B[9]);
void mul_m3_m3m4(float R[9], float A[9], float B[16]);
void mul_m3_m3(float R[9], float A[9])
{
	mul_m3_m3m3(R, R, A);
}
void mul_m4_m4(float R[16], float A[16])
{
	mul_m4_m4m4(R, R, A);
}

void transpose_m3(float R[9])
{
	int i, j;
	float t;
	for(i=1; i<=3; i++) for(j=i+1; j<=3; j++)
	{
		t = R[INDEX_M3(i,j)];
		R[INDEX_M3(i,j)] = R[INDEX_M3(j,i)];
		R[INDEX_M3(j,i)] = t;
	}
}
void transpose_m4(float R[16])
{
	int i, j;
	float t;
	for(i=1; i<=4; i++) for(j=i+1; j<=4; j++)
	{
		t = R[INDEX_M4(i,j)];
		R[INDEX_M4(i,j)] = R[INDEX_M4(j,i)];
		R[INDEX_M4(j,i)] = t;
	}
}

void translate_m4(float A[16], float x, float y, float z)
{
	float M[16];
	create_translate_m4(M, x, y, z);
	mul_m4_m4m4(A, A, M);
}
void rotate_m4(float A[16], float a, float x, float y, float z)
{
	float M[16];
	create_rotate_m4(M, a, x, y, z);
	mul_m4_m4m4(A, A, M);
}
void scale_m4(float A[16], float x, float y, float z)
{
	float M[16];
	create_scale_m4(M, x, y, z);
	mul_m4_m4m4(A, A, M);
}

void create_zero_m3(float R[9])
{
	memset(R, 0, 9*sizeof(float));
}
void create_zero_m4(float R[16])
{
	memset(R, 0, 16*sizeof(float));
}
void create_identity_m3(float R[9])
{
	int i, j;
	for(i=1; i<=3; i++) for(j=1; j<=3; j++)
	{
		if(i==j) R[INDEX_M3(i,j)] = 1.0;
		else R[INDEX_M3(i,j)] = 0.0;
	}
}
void create_identity_m4(float R[16])
{
	int i, j;
	for(i=1; i<=4; i++) for(j=1; j<=4; j++)
	{
		if(i==j) R[INDEX_M4(i,j)] = 1.0;
		else R[INDEX_M4(i,j)] = 0.0;
	}
}
void create_translate_m4(float R[16], float x, float y, float z)
{
	float M[16] = {	1.0, 0.0, 0.0, x,
			0.0, 1.0, 0.0, y,
			0.0, 0.0, 1.0, z,
			0.0, 0.0, 0.0, 1.0};
	transpose_m4(M);
	copy_m4_m4(R, M);
}
void create_rotate_m4(float R[16], float a, float x, float y, float z)
{
	float f = 1.0/sqrtf(x*x+y*y+z*z);
	x *= f; y *= f; z *= f;
	a *= M_PI/180.0;
	float c = cosf(a);
	float s = sinf(a);
	float M[16] = {	x*x*(1-c)+c,	x*y*(1-c)-z*s,	x*z*(1-c)+y*s,	0.0,
			y*x*(1-c)+z*s,	y*y*(1-c)+c,	y*z*(1-c)-x*s,	0.0,
			z*x*(1-c)-y*s,	z*y*(1-c)+x*s,	z*z*(1-c)+c,	0.0,
			0.0,		0.0,		0.0,		1.0};
	transpose_m4(M);
	copy_m4_m4(R, M);
}
void create_scale_m4(float R[16], float x, float y, float z)
{
	float M[16] = {	x, 0.0, 0.0, 0.0,
			0.0, y, 0.0, 0.0,
			0.0, 0.0, z, 0.0,
			0.0, 0.0, 0.0, 1.0};
	transpose_m4(M);
	copy_m4_m4(R, M);
}
void create_ortho_m4(float R[16], float l, float r, float b, float t, float n, float f)
{
	float M[16] = {	2.0/(r-l), 0.0, 0.0, -((r+l)/(r-l)),
			0.0, 2.0/(t-b), 0.0, -((t+b)/(t-b)),
			0.0, 0.0, -2.0/(f-n), -((f+n)/(f-n)),
			0.0, 0.0, 0.0, 1.0};
	transpose_m4(M);
	copy_m4_m4(R, M);
}
void create_ortho2d_m4(float R[16], float l, float r, float b, float t)
{
	create_ortho_m4(R, l, r, b, t, -1.0, 1.0);
}
void create_frustum_m4(float R[16], float l, float r, float b, float t, float n, float f)
{
	float M[16] = {	(2.0*n)/(r-f), 0.0, (r+l)/(r-l), 0.0,
			0.0, (2.0*n)/(t-b), (t+b)/(t-b), 0.0,
			0.0, 0.0, -(f+n)/(f-n), (-2.0*f*n)/(f-n),
			0.0, 0.0, -1.0, 0.0};
	transpose_m4(M);
	copy_m4_m4(R, M);
}
void create_perspective_m4(float R[16], float fov, float aspect, float near, float far)
{
	float f = 1.0/tan((fov*(M_PI/180.0))/2.0);
	float M[16] = {	f/aspect, 0.0, 0.0, 0.0,
			0.0, f, 0.0, 0.0,
			0.0, 0.0, (near+far)/(near-far), (2.0*near*far)/(near-far),
			0.0, 0.0, -1.0, 0.0};
	transpose_m4(M);
	copy_m4_m4(R, M);
}

void print_m3(const char *str, float A[9])
{
	int i, j;
	printf("%s:\n", str);
	for(i=1; i<=3; i++)
	{
		for(j=1; j<=3; j++) printf("%f ", A[INDEX_M3(i,j)]);
		printf("\n");
	}
}
void print_m4(const char *str, float A[16])
{
	int i, j;
	printf("%s:\n", str);
	for(i=1; i<=4; i++)
	{
		for(j=1; j<=4; j++) printf("%f ", A[INDEX_M4(i,j)]);
		printf("\n");
	}
}
