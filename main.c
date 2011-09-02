#include "maze.h"
#include "mesh.h"
#include "camera.h"
#include "walker.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int screen_size[2] = {1280, 800};

float t = 0.0;
float t_endgame;
Maze *maze;
Mesh *maze_mesh, *plane, *pyramid;
Walker *walker;
GLuint wall_texture;
GLuint ceiling_texture;
GLuint floor_texture;

enum
{
	GAME_STARTING,
	GAME_RUNNING,
	GAME_ENDING
} game_state;

enum
{
	RENDER_NORMAL,
	RENDER_ANAGLYPH,
	RENDER_SIDEBYSIDE
} render_mode = RENDER_NORMAL;

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

void initGL()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	glMatrixMode(GL_PROJECTION);
	gluPerspective(90.0, (float)screen_size[0]/(float)screen_size[1], 0.1, 1000.0);
	glMatrixMode(GL_MODELVIEW);
	
	glEnable(GL_LIGHT0);
	GLfloat light_pos[] = {0.0, 1.0, 0.0, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	GLfloat light_ambient[] = {1.0, 1.0, 1.0, 1.0};
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	GLfloat mat_ambient[] = {0.3, 0.3, 0.3, 1.0};
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
}

void draw_scene()
{	
	float m[16];
	camera_get_matrix(m);
	glLoadMatrixf(m);
	
	glBindTexture(GL_TEXTURE_2D, floor_texture);
	mesh_draw(plane);
	glBindTexture(GL_TEXTURE_2D, ceiling_texture);
	glPushMatrix();
	glTranslatef(0.0, 1.0, 0.0);
	mesh_draw(plane);
	glPopMatrix();
	
	glBindTexture(GL_TEXTURE_2D, wall_texture);
	glPushMatrix();
	if(game_state == GAME_STARTING) glScalef(1.0, t/100.0, 1.0);
	if(game_state == GAME_ENDING) glScalef(1.0, 1.0-((t-t_endgame)/100.0), 1.0);
	mesh_draw(maze_mesh);
	glPopMatrix();
	
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	int i;
	for(i=0; i<maze->height*maze->width; i++)
	{
		Cell *cell = &maze->cells[i];
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
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	if(render_mode == RENDER_ANAGLYPH) glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
	if(render_mode == RENDER_SIDEBYSIDE) glViewport(0, 0, screen_size[0]/2, screen_size[1]);
	
	draw_scene();
	
	if(render_mode == RENDER_NORMAL) return;
	
	float vec1[3], vec2[3];
	camera_get_rotation(vec1);
	copy_v3_v3(vec2, vec1);
	vec1[0] += 5.0;
	camera_set_rotation(vec1);
	
	if(render_mode == RENDER_ANAGLYPH)
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	if(render_mode == RENDER_SIDEBYSIDE) glViewport(screen_size[0]/2, 0, screen_size[0]/2, screen_size[1]);
	
	draw_scene();
	
	camera_set_rotation(vec2);
	
	if(render_mode == RENDER_ANAGLYPH) glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if(render_mode == RENDER_SIDEBYSIDE) glViewport(0, 0, screen_size[0], screen_size[1]);
}

int main()
{
	srand(time(NULL));
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(screen_size[0], screen_size[1], 32, SDL_OPENGL);
	
	initGL();
	
	texture_init();
	wall_texture = texture_create("wall.jpg");
	ceiling_texture = texture_create("ceiling.jpg");
	floor_texture = texture_create("floor.jpg");
	
	pyramid = mesh_create_pyramid(0.2);
	
	new_game();
	
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
		
		render();
		
		SDL_GL_SwapBuffers();
		SDL_Delay(20);
	}
	
	SDL_Quit();
	return 0;
}
