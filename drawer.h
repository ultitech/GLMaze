#ifndef H_DRAWER
#define H_DRAWER

#include "mesh.h"

#include <GL/gl.h>

typedef GLuint Texture;
typedef GLuint Program;

void drawer_init();
void drawer_quit();
Program drawer_create_program(char *vertex_filename, char *fragment_filename);
void drawer_use_program(Program program);
void drawer_draw_mesh(Mesh *mesh);
Texture drawer_load_texture(char *filename);
void drawer_use_texture(Texture texture);
void drawer_modelview_set(float matrix[16]);
void drawer_modelview_get(float matrix[16]);
int drawer_do_events();
void drawer_begin_scene();
void drawer_end_scene();

#endif //H_DRAWER
