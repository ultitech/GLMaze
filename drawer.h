#ifndef H_DRAWER
#define H_DRAWER

#include "mesh.h"

enum Render3DMode
{
	RENDER_3D_OFF,
	RENDER_3D_ANAGLYPH,
	RENDER_3D_SIDEBYSIDE,
	RENDER_3D_MODES_COUNT
};

typedef unsigned int Texture;
typedef unsigned int Program;

void drawer_init();
void drawer_quit();
void drawer_modelview_set(float matrix[16]);
void drawer_modelview_get(float matrix[16]);
Program drawer_create_program(char *vertex_filename, char *fragment_filename);
void drawer_use_program(Program program);
Texture drawer_load_texture(char *filename);
void drawer_use_texture(Texture texture);
void drawer_depth_mask(unsigned char mask);
void drawer_draw_mesh(Mesh *mesh);
void drawer_postprocess_pass_add(char *filename, int toggle_key);
void drawer_do_postprocess();
int drawer_do_events();
void drawer_begin_scene();
void drawer_end_scene();
void drawer_3d_reset();
void drawer_3d_left();
void drawer_3d_right();
enum Render3DMode drawer_get_3d_mode();

#endif //H_DRAWER
