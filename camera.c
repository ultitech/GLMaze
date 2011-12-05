#include "camera.h"
#include "MathLib.h"

#include <string.h>

static float camera_position[] = {0.0, 0.0, 0.0};
static float camera_angle[] = {0.0, 0.0, 0.0};

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
	float m[16];
	create_identity_m4(m);
	rotate_m4(m, camera_angle[2], 0.0, 0.0, 1.0);
	rotate_m4(m, camera_angle[1], 1.0, 0.0, 0.0);
	rotate_m4(m, camera_angle[0], 0.0, 1.0, 0.0);
	translate_m4(m, -camera_position[0], -camera_position[1], -camera_position[2]);
	copy_m4_m4(mat, m);
}
