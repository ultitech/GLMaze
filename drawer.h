#ifndef H_DRAWER
#define H_DRAWER

#include "mesh.h"
#include "key.h"

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
void drawer_use_rendertarget(Rendertarget target);
void drawer_use_rendertarget_texture(Rendertarget target, unsigned int texture_unit, char *uniform_name);
void drawer_depth_mask(unsigned char mask);
void drawer_draw_mesh(Mesh *mesh);
void drawer_postprocess_pass_add(char *filename, enum Key toggle_key);
void drawer_do_postprocess();
void drawer_begin_scene(float time_passed);
void drawer_end_scene();
void drawer_create_mesh_vbo(Mesh *mesh);
void drawer_free_mesh_vbo(MeshVBO *vbo);

#endif //H_DRAWER
