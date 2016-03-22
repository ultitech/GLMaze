/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef H_DRAWER
#define H_DRAWER

#include "mesh.h"
#include <SDL_keycode.h>

enum Drawer3DMode
{
	DRAWER_3D_OFF,
	DRAWER_3D_SIDEBYSIDE,
	DRAWER_3D_MODES_COUNT
};

#define DRAWER_WINDOW_RENDERTARGET 0
#define DRAWER_PP_RENDERTARGET 0xFFFFFFFF

typedef unsigned int Texture;
typedef unsigned int Program;
typedef unsigned int Rendertarget;

void drawer_init();
void drawer_quit();
void drawer_modelview_set(float matrix[16]);
void drawer_modelview_get(float matrix[16]);
Program drawer_create_program(char *vertex_filename, char *fragment_filename);
void drawer_use_program(Program program);
Texture drawer_load_texture(char *filename);
void drawer_use_texture(Texture texture, unsigned int texture_unit, char *uniform_name);
Rendertarget drawer_create_rendertarget();
void drawer_use_rendertarget(Rendertarget target, char clear);
void drawer_use_rendertarget_texture(Rendertarget target, unsigned int texture_unit, char *uniform_name);
void drawer_depth_mask(unsigned char mask);
void drawer_draw_mesh(Mesh *mesh);
void drawer_postprocess_pass_add(char *filename, SDL_Keycode toggle_key);
void drawer_do_postprocess();
void drawer_begin_scene(float time_passed);
void drawer_end_scene();
void drawer_3d_reset();
void drawer_3d_left();
void drawer_3d_right();
void drawer_set_3d_mode(enum Drawer3DMode mode);
enum Drawer3DMode drawer_get_3d_mode();
void drawer_create_mesh_vbo(Mesh *mesh);
void drawer_free_mesh_vbo(MeshVBO *vbo);
void drawer_screenshot();
void drawer_print_glinfo();
void drawer_write_glinfo();

#endif //H_DRAWER
