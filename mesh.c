#include "mesh.h"

#include <stdlib.h>
#include <MathLib.h>

Mesh* mesh_create_maze(Maze *maze)
{
	Mesh *mesh = malloc(sizeof(Mesh));
	
	mesh->vertex_format = GL_T2F_V3F;
	mesh->vertices = malloc(sizeof(GLfloat) * ((maze->width+1)*(maze->height+1)*2) * (3+2));
	int x, y, z;
	GLfloat *v = mesh->vertices;
	for(y=0; y<2; y++) for(z=0; z<(maze->height+1); z++) for(x=0; x<(maze->width+1); x++)
	{
		//texcoord:
		*v++ = x+z;
		*v++ = y;
		//position:
		*v++ = x;
		*v++ = y;
		*v++ = z;
	}
	
	mesh->indices_count = (maze->width*(maze->height+1)+(maze->width+1)*maze->height) * (2*3);
	mesh->indices_count -= ((maze->width*maze->height) - 1) * (2*3); //passages
	mesh->indices = malloc(sizeof(GLuint) * mesh->indices_count);
	GLuint *i = mesh->indices;
	//horizontal walls
	for(y=0; y<maze->height+1; y++) for(x=0; x<maze->width; x++)
	{
		if(y <= maze->height)
		{
			Cell *cell = maze_get_cell(maze, x, y);
			if(cell->up) continue;
		}
		
		GLuint origin = y*(maze->width+1)+x;
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
		
		GLuint origin = y*(maze->width+1)+x;
		*i++ = origin;
		*i++ = origin + (maze->width+1);
		*i++ = origin + (maze->height+1) * (maze->width+1);
		
		*i++ = origin + (maze->width+1);
		*i++ = origin + (maze->height+1) * (maze->width+1) + (maze->width+1);
		*i++ = origin + (maze->height+1) * (maze->width+1);
	}
	
	return mesh;
}

Mesh* mesh_create_quad(float x_scale, float z_scale)
{
	Mesh *mesh = malloc(sizeof(Mesh));
	
	mesh->vertex_format = GL_T2F_V3F;
	mesh->vertices = malloc(sizeof(GLfloat) * (3+2) * 4); //position/texcoord
	int x, z;
	GLfloat *v = mesh->vertices;
	for(z=0; z<2; z++) for(x=0; x<2; x++)
	{
		//texcoord:
		*v++ = x * x_scale;
		*v++ = z * z_scale;
		//position:
		*v++ = x * x_scale;
		*v++ = 0;
		*v++ = z * z_scale;
	}
	
	mesh->indices_count = 2*3;
	mesh->indices = malloc(sizeof(GLuint) * mesh->indices_count);
	GLuint *i = mesh->indices;
	*i++ = 0; *i++ = 1; *i++ = 2;
	*i++ = 1; *i++ = 3; *i++ = 2;
	
	return mesh;
}

Mesh* mesh_create_pyramid(float scale)
{
	Mesh *mesh = malloc(sizeof(Mesh));
	
	mesh->vertex_format = GL_N3F_V3F;
	mesh->vertices = malloc(sizeof(GLfloat) * (3+3) * 4);
	GLfloat *v = mesh->vertices;
	const GLfloat n = 0.577349;
	
	#define V(a,b,c) *v++ = a n; *v++ = b n; *v++ = c n; *v++ = a scale; *v++ = b scale; *v++ = c scale;
	//left down front
	V(-,-,+)
	//right down back
	V(+,-,-)
	//left top back
	V(-,+,-)
	//right top front
	V(+,+,+)
	#undef V
	
	mesh->indices_count = 4*3;
	mesh->indices = malloc(sizeof(GLuint) * mesh->indices_count);
	GLuint *i = mesh->indices;
	*i++ = 0; *i++ = 1; *i++ = 3;
	*i++ = 0; *i++ = 3; *i++ = 2;
	*i++ = 1; *i++ = 2; *i++ = 3;
	*i++ = 0; *i++ = 1; *i++ = 2;
	
	return mesh;
}

void mesh_free(Mesh *mesh)
{
	free(mesh->vertices);
	free(mesh->indices);
	free(mesh);
}

void mesh_draw(Mesh *mesh)
{
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
	
	glInterleavedArrays(mesh->vertex_format, 0, mesh->vertices);
	
	glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_INT, mesh->indices);
	
	glPopClientAttrib();
}
