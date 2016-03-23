/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#include "mesh.h"
#include "drawer.h"
#include "MathLib.h"

#include <stdlib.h>
#include <string.h>

static char generate_vbo = 0;

#define MAZE_Y0_OFFSET -0.2

static Mesh* allocate_mesh();
static void allocate_data(Mesh *mesh);

Mesh* mesh_create_maze(Maze *maze)
{
	Mesh *mesh = allocate_mesh();
	
	mesh->vertex_format = VERTEX_POSITION | VERTEX_TEXCOORD;
	mesh->vertices_count = ((maze->width+1)*(maze->height+1)*2);
	mesh->indices_count = ((maze->width*(maze->height+1)+(maze->width+1)*maze->height)-((maze->width*maze->height) - 1)) * (2*3);
	
	allocate_data(mesh);
	
	unsigned int x, y, z;
	float *v = mesh->data->vertices;
	for(y=0; y<2; y++) for(z=0; z<(maze->height+1); z++) for(x=0; x<(maze->width+1); x++)
	{
		//position:
		*v++ = x;
		*v++ = y==0.0 ? MAZE_Y0_OFFSET : y;
		*v++ = z;
		//texcoord:
		*v++ = x+z;
		*v++ = y==0.0 ? MAZE_Y0_OFFSET : y;
	}
	
	unsigned int *i = mesh->data->indices;
	//horizontal walls
	for(y=0; y<maze->height+1; y++) for(x=0; x<maze->width; x++)
	{
		if(y < maze->height)
		{
			Cell *cell = maze_get_cell(maze, x, y);
			if(cell->up) continue;
		}
		
		unsigned int origin = y*(maze->width+1)+x;
		*i++ = origin;
		*i++ = origin + 1;
		*i++ = origin + (maze->height+1) * (maze->width+1);
		
		*i++ = origin + 1;
		*i++ = origin + (maze->height+1) * (maze->width+1) + 1;
		*i++ = origin + (maze->height+1) * (maze->width+1);
	}
	//vertical walls
	for(y=0; y<maze->height; y++) for(x=0; x<maze->width+1; x++)
	{
		if(x < maze->width)
		{
			Cell *cell = maze_get_cell(maze, x, y);
			if(cell->left) continue;
		}
		
		unsigned int origin = y*(maze->width+1)+x;
		*i++ = origin;
		*i++ = origin + (maze->width+1);
		*i++ = origin + (maze->height+1) * (maze->width+1);
		
		*i++ = origin + (maze->width+1);
		*i++ = origin + (maze->height+1) * (maze->width+1) + (maze->width+1);
		*i++ = origin + (maze->height+1) * (maze->width+1);
	}
	
	if(generate_vbo) drawer_create_mesh_vbo(mesh);
	
	return mesh;
}

Mesh* mesh_create_quad(float x_scale, float z_scale)
{
	Mesh *mesh = allocate_mesh();
	
	mesh->vertex_format = VERTEX_POSITION | VERTEX_TEXCOORD;
	mesh->vertices_count = 4;
	mesh->indices_count = 2*3;
	
	allocate_data(mesh);
	
	int x, z;
	float *v = mesh->data->vertices;
	for(x=0; x<2; x++) for(z=0; z<2; z++)
	{
		//position:
		*v++ = x * x_scale;
		*v++ = 0;
		*v++ = z * z_scale;
		//texcoord:
		*v++ = x * x_scale;
		*v++ = z * z_scale;
	}
	
	unsigned int *i = mesh->data->indices;
	*i++ = 0; *i++ = 1; *i++ = 2;
	*i++ = 1; *i++ = 3; *i++ = 2;
	
	if(generate_vbo) drawer_create_mesh_vbo(mesh);
	
	return mesh;
}

Mesh* mesh_create_pyramid(float scale)
{
	Mesh *mesh = allocate_mesh();
	
	mesh->vertex_format = VERTEX_POSITION;
	mesh->vertices_count = 4;
	mesh->indices_count = 4*3;
	
	allocate_data(mesh);
	
	float *v = mesh->data->vertices;
	#define V(a,b,c) *v++ = a scale; *v++ = b scale; *v++ = c scale;
	//left down front
	V(-,-,+)
	//right down back
	V(+,-,-)
	//left top back
	V(-,+,-)
	//right top front
	V(+,+,+)
	#undef V
	
	unsigned int *i = mesh->data->indices;
	*i++ = 0; *i++ = 1; *i++ = 3;
	*i++ = 0; *i++ = 3; *i++ = 2;
	*i++ = 1; *i++ = 2; *i++ = 3;
	*i++ = 0; *i++ = 2; *i++ = 1;
	
	if(generate_vbo) drawer_create_mesh_vbo(mesh);
	
	return mesh;
}

Mesh* mesh_create_screen_square()
{
	Mesh *mesh = allocate_mesh();
	
	mesh->vertex_format = VERTEX_POSITION;
	mesh->vertices_count = 4;
	mesh->indices_count = 2*3;
	
	allocate_data(mesh);
	
	float *v = mesh->data->vertices;
	*v++ = -1.0; *v++ = -1.0; *v++ = 0.0;
	*v++ = 1.0; *v++ = -1.0; *v++ = 0.0;
	*v++ = 1.0; *v++ = 1.0; *v++ = 0.0;
	*v++ = -1.0; *v++ = 1.0; *v++ = 0.0;
	
	unsigned int *i = mesh->data->indices;
	*i++ = 0; *i++ = 1; *i++ = 2;
	*i++ = 0; *i++ = 2; *i++ = 3;
	
	if(generate_vbo) drawer_create_mesh_vbo(mesh);
	
	return mesh;
}

int mesh_get_vertex_size(unsigned int vertex_format)
{
	int size = 0;
	if(vertex_format & VERTEX_POSITION) size += 3;
	if(vertex_format & VERTEX_NORMAL) size += 3;
	if(vertex_format & VERTEX_TEXCOORD) size += 2;
	return size;
}

void mesh_generate_vbos(char value)
{
	generate_vbo = value;
}

void mesh_free_data(MeshData *data)
{
	free(data->vertices);
	free(data->indices);
	free(data);
}

void mesh_free_vbo(MeshVBO *vbo)
{
	drawer_free_mesh_vbo(vbo);
}

void mesh_free(Mesh *mesh)
{
	mesh_free_data(mesh->data);
	mesh_free_vbo(mesh->vbo);
	free(mesh);
}

static Mesh* allocate_mesh()
{
	Mesh *mesh = malloc(sizeof(Mesh));
	memset(mesh, 0x00, sizeof(Mesh));
	return mesh;
}

static void allocate_data(Mesh *mesh)
{
	MeshData *data = malloc(sizeof(MeshData));
	
	data->vertices = malloc(sizeof(float) * mesh->vertices_count * mesh_get_vertex_size(mesh->vertex_format));
	data->indices = malloc(sizeof(unsigned int) * mesh->indices_count);
	
	mesh->data = data;
}
