#ifndef H_MESH
#define H_MESH

#include "maze.h"
#include <GL/gl.h>

typedef struct
{
	GLfloat *vertices; //data array as position/texcoord
	GLuint *indices;
	GLuint indices_count;
} Mesh;

Mesh* mesh_create_maze(Maze *maze);
Mesh* mesh_create_quad(float scale);
void mesh_free(Mesh *mesh);
void mesh_draw(Mesh *mesh);

#endif //H_MESH
