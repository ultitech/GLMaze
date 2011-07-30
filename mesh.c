#include "mesh.h"

#include <stdlib.h>

Mesh* mesh_create_maze(Maze *maze)
{
	Mesh *mesh = malloc(sizeof(Mesh));
	
	mesh->vertices = malloc(sizeof(GLfloat) * ((maze->width+1)*(maze->height+1)*2) * (3+2));
	int x, y, z;
	GLfloat *v = mesh->vertices;
	for(y=0; y<2; y++) for(z=0; z<(maze->height+1); z++) for(x=0; x<(maze->width+1); x++)
	{
		//position:
		*v++ = x;
		*v++ = y;
		*v++ = z;
		//texcoord:
		*v++ = x+z;
		*v++ = y;
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

Mesh* mesh_create_quad(float scale)
{
	Mesh *mesh = malloc(sizeof(Mesh));
	
	mesh->vertices = malloc(sizeof(GLfloat) * (3+2)); //position/texcoord
	int x, z;
	GLfloat *v = mesh->vertices;
	for(z=0; z<2; z++) for(x=0; x<2; x++)
	{
		//position:
		*v++ = (2*x)-1;
		*v++ = 0;
		*v++ = (2*z)-1;
		//texcoord:
		*v++ = x;
		*v++ = -z;
	}
	
	mesh->indices = malloc(sizeof(GLuint) * (2*3));
	GLuint *i = mesh->indices;
	*i++ = 0; *i++ = 1; *i++ = 2;
	*i++ = 1; *i++ = 3; *i++ = 2;
	
	return mesh;
}

void mesh_free(Mesh *mesh)
{
	free(mesh->vertices);
	free(mesh->indices);
	free(mesh);
}
