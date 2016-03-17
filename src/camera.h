/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef H_CAMERA
#define H_CAMERA

void camera_set_position(float pos[3]);
void camera_get_position(float pos[3]);
void camera_set_rotation(float ang[3]);
void camera_get_rotation(float ang[3]);
void camera_get_matrix(float mat[16]);

#endif //H_CAMERA
