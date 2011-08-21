#include "maze.h"
#include "mesh.h"
#include "camera.h"
#include "walker.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

float t = 0.0;
float t_endgame;
Maze *maze;
Mesh *maze_mesh;
Walker *walker;
Mesh *plane;

enum
{
	GAME_STARTING,
	GAME_RUNNING,
	GAME_ENDING
} game_state;

void camera_update_pos(float pos[3])
{
	pos[1] = 0.5;
	camera_set_position(pos);
}

void finish()
{
	t_endgame = t;
	game_state = GAME_ENDING;
}

void clean_up()
{
	if(maze) maze_free(maze);
	if(maze_mesh) mesh_free(maze_mesh);
	if(walker) free(walker);
	if(plane) mesh_free(plane);
}

void new_game()
{
	clean_up();
	maze = maze_generate(10, 10);
	maze_print(maze);
	maze_mesh = mesh_create_maze(maze);
	plane = mesh_create_quad((float)maze->width, (float)maze->height);
	int start[2] = {0, 0};
	walker = walker_create(maze, start, DOWN, camera_update_pos, camera_set_rotation, finish);
	game_state = GAME_STARTING;
	t_endgame = t = 0.0;
}

int main()
{
	srand(time(NULL));
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(500, 500, 32, SDL_OPENGL);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	gluPerspective(60.0, 1.0, 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	glPointSize(10.0);
	
	glEnable(GL_LIGHT0);
	GLfloat light_pos[] = {0.0, 1.0, 0.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	GLfloat light_ambient[] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	GLfloat mat_ambient[] = {0.3, 0.3, 0.3, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	
	texture_init();
	GLuint wall_texture = texture_create("wall.jpg");
	GLuint ceiling_texture = texture_create("ceiling.jpg");
	GLuint floor_texture = texture_create("floor.jpg");
	
	new_game();
	
	Mesh *pyramid = mesh_create_pyramid(0.2);
	
	char quit = 0;
	while(!quit)
	{
		SDL_Event ev;
		while(SDL_PollEvent(&ev))
		{
			if(ev.type == SDL_QUIT) quit = 1;
		}
		
		t++;
		
		if(t > 100.0 && game_state == GAME_STARTING) game_state = GAME_RUNNING;
		if((t-t_endgame) > 100.0 && game_state == GAME_ENDING) new_game();
		
		if(game_state == GAME_RUNNING) walker_step(walker, 0.02);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		float m[16];
		camera_get_matrix(m);
		glLoadMatrixf(m);
		
		glPushMatrix();
		glBindTexture(GL_TEXTURE_2D, floor_texture);
		mesh_draw(plane);
		glTranslatef(0.0, 1.0, 0.0);
		glBindTexture(GL_TEXTURE_2D, ceiling_texture);
		mesh_draw(plane);
		glPopMatrix();
		
		glColor3f(1.0, 1.0, 1.0);
		glBindTexture(GL_TEXTURE_2D, wall_texture);
		glPushMatrix();
		if(game_state == GAME_STARTING) glScalef(1.0, t/100.0, 1.0);
		if(game_state == GAME_ENDING) glScalef(1.0, 1.0-((t-t_endgame)/100.0), 1.0);
		mesh_draw(maze_mesh);
		glPopMatrix();
		
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_LIGHTING);
		int x, y;
		for(y=0; y<maze->height; y++) for(x=0; x<maze->width; x++)
		{
			Cell *cell = maze_get_cell(maze, x, y);
			if(cell->object == OBJ_TWISTER)
			{
				glPushMatrix();
				glTranslatef(cell->x+0.5, 0.5, cell->y+0.5);
				glRotatef(t, 0.0, 1.0, 0.0);
				glRotatef(t*0.7, 1.0, 0.0, 0.0);
				mesh_draw(pyramid);
				glPopMatrix();
			}
		}
		glPopAttrib();
		
		GLuint error = glGetError();
		if(error) printf("OpenGL Error: %s\n", gluErrorString(error));
		
		SDL_GL_SwapBuffers();
		SDL_Delay(20);
	}
	
	SDL_Quit();
	return 0;
}
