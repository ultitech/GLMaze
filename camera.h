#ifndef H_CAMERA
#define H_CAMERA

void camera_set_position(float pos[3]);
void camera_get_position(float pos[3]);
void camera_set_rotation(float ang[3]);
void camera_get_rotation(float ang[3]);
void camera_get_matrix(float mat[16]);

#endif //H_CAMERA
