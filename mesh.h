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
	float *vertices;
	unsigned int *indices;
} MeshData;

typedef struct
{
	unsigned int vertex_buffer, index_buffer;
} MeshVBO;

typedef struct
{
	unsigned int vertex_format;
	unsigned int vertices_count, indices_count;
	MeshData *data;
	MeshVBO *vbo;
} Mesh;

Mesh* mesh_create_maze(Maze *maze);
Mesh* mesh_create_quad(float x_scale, float z_scale);
Mesh* mesh_create_pyramid(float scale);
Mesh* mesh_create_screen_square();
int mesh_get_vertex_size(unsigned int vertex_format);
void mesh_free_data(MeshData *data);
void mesh_free_vbo(MeshVBO *data);
void mesh_free(Mesh *mesh);

#endif //H_MESH
