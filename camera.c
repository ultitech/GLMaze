#include "camera.h"

#include <string.h>
#include <MatrixLib.h>

float camera_position[] = {0.0, 0.0, 0.0};
float camera_angle[] = {0.0, 0.0, 0.0};

void camera_set_position(float *pos)
{
	memcpy(camera_position, pos, sizeof(float)*3);
}

void camera_get_position(float *pos)
{
	memcpy(pos, camera_position, sizeof(float)*3);
}

void camera_set_rotation(float *ang)
{
	memcpy(camera_angle, ang, sizeof(float)*3);
}

void camera_get_rotation(float *ang)
{
	memcpy(ang, camera_angle, sizeof(float)*3);
}

void camera_get_matrix(float *mat)
{
	Matrix m, t;
	mat_create_identity(&m);
	
	mat_create_rotate(&t, camera_angle[2], 0.0, 0.0, 1.0);
	mat_multiply(&m, &t);
	
	mat_create_rotate(&t, camera_angle[1], 1.0, 0.0, 0.0);
	mat_multiply(&m, &t);
	
	mat_create_rotate(&t, camera_angle[0], 0.0, 1.0, 0.0);
	mat_multiply(&m, &t);
	
	mat_create_translate(&t, -camera_position[0], -camera_position[1], -camera_position[2]);
	mat_multiply(&m, &t);
	
	memcpy(mat, mat_elements_pointer(&m), sizeof(float)*16);
}
