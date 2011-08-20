#ifndef H_MESH
#define H_MESH

#include "maze.h"
#include <GL/gl.h>

typedef struct
{
	GLenum vertex_format;
	GLfloat *vertices;
	GLuint *indices;
	GLuint indices_count;
} Mesh;

Mesh* mesh_create_maze(Maze *maze);
Mesh* mesh_create_quad(float x_scale, float z_scale);
Mesh* mesh_create_pyramid(float scale);
void mesh_free(Mesh *mesh);
void mesh_draw(Mesh *mesh);

#endif //H_MESH
