#include "scene.h"
#include "mesh.h"
#include "camera.h"
#include "walker.h"
#include "drawer.h"

#include <stdlib.h>
#include <MathLib.h>

float t = 0.0;
float t_endgame;
Maze *maze;
Mesh *maze_mesh, *plane, *pyramid;
Walker *walker;
Texture wall_texture, ceiling_texture, floor_texture;
Program textured_program, twister_program, floor_reflect_program;
Rendertarget reflection_target;

#define WALL_GROW_TIME 2.0

enum
{
	GAME_STARTING,
	GAME_RUNNING,
	GAME_ENDING
} game_state;

enum RenderPass
{
	PASS_FINAL,
	PASS_REFLECTION
};

static void camera_update_pos(float pos[3]);
static void finish();
static void clean_up();
static void new_game();
static void draw_scene();
static void draw_models(enum RenderPass pass);
static void draw_ceiling(enum RenderPass pass);
static void draw_floor(enum RenderPass pass);
static void draw_walls(enum RenderPass pass);
static void draw_twisters(enum RenderPass pass);

void scene_init()
{
	wall_texture = drawer_load_texture("wall.jpg");
	ceiling_texture = drawer_load_texture("ceiling.jpg");
	floor_texture = drawer_load_texture("floor.jpg");
	
	textured_program = drawer_create_program("textured.glslv", "textured.glslf");
	floor_reflect_program = drawer_create_program("textured.glslv", "floor_reflect.glslf");
	twister_program = drawer_create_program("twister.glslv", "twister.glslf");
	drawer_postprocess_pass_add("pp_radialblur.glslf", 'b');
	drawer_postprocess_pass_add("pp_nightvision.glslf", 'n');
	
	reflection_target = drawer_create_rendertarget();
	
	pyramid = mesh_create_pyramid(0.2);
	
	new_game();
}

void scene_quit()
{
}

void scene_update(float time)
{
	t += time;
	
	if(t > WALL_GROW_TIME && game_state == GAME_STARTING) game_state = GAME_RUNNING;
	if((t-t_endgame) > WALL_GROW_TIME && game_state == GAME_ENDING) new_game();
	
	if(game_state == GAME_RUNNING) walker_step(walker, time);
}

void scene_draw()
{
	if(drawer_get_3d_mode() == RENDER_3D_OFF) draw_scene();
	else
	{
		float camera_rot[3], temp[3];
		camera_get_rotation(camera_rot);
		
		drawer_3d_left();
		copy_v3_v3(temp, camera_rot);
		temp[0] -= 2.5;
		camera_set_rotation(temp);
		draw_scene();
		
		drawer_3d_right();
		copy_v3_v3(temp, camera_rot);
		temp[0] += 2.5;
		camera_set_rotation(temp);
		draw_scene();
		
		camera_set_rotation(camera_rot);
		drawer_3d_reset();
	}
	
	drawer_do_postprocess();
}

static void camera_update_pos(float pos[3])
{
	pos[1] = 0.5;
	camera_set_position(pos);
}

static void finish()
{
	t_endgame = t;
	game_state = GAME_ENDING;
}

static void clean_up()
{
	if(maze) maze_free(maze);
	if(maze_mesh) mesh_free(maze_mesh);
	if(walker) free(walker);
	if(plane) mesh_free(plane);
}

static void new_game()
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

static void draw_scene()
{	
	draw_models(PASS_REFLECTION);
	draw_models(PASS_FINAL);
}

static void draw_models(enum RenderPass pass)
{
	if(pass == PASS_REFLECTION) drawer_use_rendertarget(reflection_target);
	else drawer_use_rendertarget(DRAWER_PP_RENDERTARGET);
	
	float mv[16];
	camera_get_matrix(mv);
	if(pass == PASS_REFLECTION) scale_m4(mv, 1.0, -1.0, 1.0);
	drawer_modelview_set(mv);
	
	if(pass != PASS_REFLECTION)
	{
		draw_floor(pass);
		drawer_modelview_set(mv);
	}
	
	draw_ceiling(pass);
	drawer_modelview_set(mv);
	
	draw_walls(pass);
	drawer_modelview_set(mv);
	
	draw_twisters(pass);
	drawer_modelview_set(mv);
}

static void draw_ceiling(enum RenderPass pass)
{
	float temp[16];
	drawer_modelview_get(temp);
	translate_m4(temp, 0.0, 1.0, 0.0);
	drawer_modelview_set(temp);
	
	drawer_use_program(textured_program);
	drawer_use_texture(ceiling_texture, 0, "Diffuse");
	drawer_draw_mesh(plane);
}

static void draw_floor(enum RenderPass pass)
{
	if(pass == PASS_FINAL)
	{
		drawer_use_program(floor_reflect_program);
		drawer_use_rendertarget_texture(reflection_target, 1, "Reflection");
	}
	else drawer_use_program(textured_program);
	drawer_use_texture(floor_texture, 0, "Diffuse");
	drawer_draw_mesh(plane);
}

static void draw_walls(enum RenderPass pass)
{
	float temp[16];
	drawer_modelview_get(temp);
	if(game_state == GAME_STARTING) scale_m4(temp, 1.0, t/WALL_GROW_TIME, 1.0);
	else if(game_state == GAME_ENDING) scale_m4(temp, 1.0, 1.0-((t-t_endgame)/WALL_GROW_TIME), 1.0);
	drawer_modelview_set(temp);
	
	drawer_use_program(textured_program);
	drawer_use_texture(wall_texture, 0, "Diffuse");
	drawer_draw_mesh(maze_mesh);
}

static void draw_twisters(enum RenderPass pass)
{
	float mv[16], temp[16];
	drawer_modelview_get(mv);
	
	drawer_use_program(twister_program);
	drawer_depth_mask(0);
	int i;
	for(i=0; i<maze->height*maze->width; i++)
	{
		Cell *cell = &maze->cells[i];
		if(cell->object == OBJ_TWISTER)
		{
			copy_m4_m4(temp, mv);
			translate_m4(temp, cell->x+0.5, 0.5, cell->y+0.5);
			rotate_m4(temp, t*50.0, 0.0, 1.0, 0.0);
			rotate_m4(temp, t*35, 1.0, 0.0, 0.0);
			drawer_modelview_set(temp);
			drawer_draw_mesh(pyramid);
		}
	}
	drawer_depth_mask(1);
}
