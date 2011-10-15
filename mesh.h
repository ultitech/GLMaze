#ifndef H_MESH
#define H_MESH

#include "maze.h"

enum VertexFormatBits
{
	VERTEX_POSITION = (1<<0),
	VERTEX_NORMAL = (1<<1),
	VERTEX_TEXCOORD = (1<<2)
};

typedef struct
{
	unsigned int vertex_format;
	float *vertices;
	unsigned int vertices_count;
	unsigned int *indices;
	unsigned int indices_count;
} Mesh;

Mesh* mesh_create_maze(Maze *maze);
Mesh* mesh_create_quad(float x_scale, float z_scale);
Mesh* mesh_create_pyramid(float scale);
Mesh* mesh_create_screen_square();
int mesh_get_vertex_size(Mesh *mesh);
void mesh_save(Mesh *mesh, char *filename);
void mesh_free(Mesh *mesh);

#endif //H_MESH
