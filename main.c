#include "maze.h"
#include "mesh.h"
#include "camera.h"
#include "walker.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void mat_print(float *mat)
{
	int i, k;
	for(i=0; i<4; i++)
	{
		for(k=0; k<4; k++)
		{
			printf("%f ", mat[k*4+i]);
		}
		printf("\n");
	}
	printf("\n");
}

void camera_update_pos(float pos[3])
{
	pos[1] = 0.3;
	camera_set_position(pos);
}

int main()
{
	srand(time(NULL));
	Maze *maze = maze_generate(10, 10);
	maze_print(maze);
	Mesh *mesh = mesh_create_maze(maze);
	int start[2] = {5, 5};
	Walker *walker = walker_create(maze, start, UP, camera_update_pos, camera_set_rotation);
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(500, 500, 32, SDL_OPENGL);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(60.0, 1.0, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	
	/*
	float cam_pos[] = {5.5, 0.5, 5.5};
	float cam_rot[] = {30.0, 0.0, 0.0};
	camera_set_position(cam_pos);
	camera_set_rotation(cam_rot);
	*/
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(GLfloat)*5, mesh->vertices);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	char quit = 0;
	while(!quit)
	{
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			if(ev.type == SDL_QUIT) quit = 1;
		}
		
		walker_step(walker, 0.02);
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		/*
		float rot[3];
		camera_get_rotation(rot);
		rot[0]++;
		rot[1] = sin(rot[0]/20.0)*10.0;
		rot[2] = sin(rot[0]);
		camera_set_rotation(rot);
		*/
		float m[16];
		camera_get_matrix(m);
		glLoadMatrixf(m);
		
		glColor3f(1.0, 1.0, 1.0);
		glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_INT, mesh->indices);
		
		glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 1.0);
		glEnd();
		
		GLuint error = glGetError();
		if(error) printf("OpenGL Error: %s\n", gluErrorString(error));
		
		SDL_GL_SwapBuffers();
		SDL_Delay(20);
	}
	
	SDL_Quit();
	return 0;
}
