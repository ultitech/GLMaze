#include "camera.h"

#include <string.h>
#include <MathLib.h>
#include <MatrixLib.h>

float camera_position[] = {0.0, 0.0, 0.0};
float camera_angle[] = {0.0, 0.0, 0.0};

void camera_set_position(float pos[3])
{
	copy_v3_v3(camera_position, pos);
}

void camera_get_position(float pos[3])
{
	copy_v3_v3(pos, camera_position);
}

void camera_set_rotation(float ang[3])
{
	copy_v3_v3(camera_angle, ang);
}

void camera_get_rotation(float ang[3])
{
	copy_v3_v3(ang, camera_angle);
}

void camera_get_matrix(float mat[16])
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
