#include "scene.h"
#include "mesh.h"
#include "camera.h"
#include "walker.h"
#include "drawer.h"
#include "key.h"
#include "config.h"
#include "MathLib.h"

#include <stdlib.h>

static float global_time;
static float time_endgame;
static Maze *maze;
static Mesh *maze_mesh, *plane, *pyramid;
static Walker *walker;
static Texture wall_texture, ceiling_texture, floor_texture;
static Program textured_program, twister_program, floor_reflect_program;
static Rendertarget reflection_target;

static char reflection_enabled;
static char postprocess_enabled;

#define WALL_GROW_TIME 2.0

enum GameState
{
	GAME_STARTING,
	GAME_RUNNING,
	GAME_ENDING
};
static enum GameState game_state;

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
	reflection_enabled = config_get_value_integer("reflection", 1);
	postprocess_enabled = config_get_value_integer("postprocess", 1);
	if(config_get_value_integer("sidebyside", 0) && !postprocess_enabled) drawer_set_3d_mode(DRAWER_3D_SIDEBYSIDE);
	
	wall_texture = drawer_load_texture("wall.jpg");
	ceiling_texture = drawer_load_texture("ceiling.jpg");
	floor_texture = drawer_load_texture("floor.jpg");
	
	textured_program = drawer_create_program("textured.glslv", "textured.glslf");
	twister_program = drawer_create_program("twister.glslv", "twister.glslf");
	
	if(postprocess_enabled)
	{
		drawer_postprocess_pass_add("pp_radialblur.glslf", KEY_b);
		drawer_postprocess_pass_add("pp_nightvision.glslf", KEY_n);
	}
	
	if(reflection_enabled)
	{
		floor_reflect_program = drawer_create_program("textured.glslv", "floor_reflect.glslf");
		reflection_target = drawer_create_rendertarget();
	}
	
	pyramid = mesh_create_pyramid(0.2);
	
	new_game();
}

void scene_quit()
{
}

void scene_update(float time_passed)
{
	global_time += time_passed;
	
	if(global_time > WALL_GROW_TIME && game_state == GAME_STARTING) game_state = GAME_RUNNING;
	if((global_time-time_endgame) > WALL_GROW_TIME && game_state == GAME_ENDING) new_game();
	
	if(game_state == GAME_RUNNING) walker_step(walker, time_passed);
}

void scene_draw()
{
	drawer_use_rendertarget(postprocess_enabled ? DRAWER_PP_RENDERTARGET : DRAWER_WINDOW_RENDERTARGET, 1);
	if(drawer_get_3d_mode() == DRAWER_3D_OFF) draw_scene();
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
}

static void camera_update_pos(float pos[3])
{
	pos[1] = 0.5;
	camera_set_position(pos);
}

static void finish()
{
	time_endgame = global_time;
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
	time_endgame = global_time = 0.0;
}

static void draw_scene()
{	
	if(reflection_enabled)
	{
		drawer_use_rendertarget(reflection_target, 1);
		draw_models(PASS_REFLECTION);
	}
	drawer_use_rendertarget(postprocess_enabled ? DRAWER_PP_RENDERTARGET : DRAWER_WINDOW_RENDERTARGET, drawer_get_3d_mode()==DRAWER_3D_OFF ? 1 : 0);
	draw_models(PASS_FINAL);
	if(postprocess_enabled) drawer_do_postprocess();
}

static void draw_models(enum RenderPass pass)
{
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
	if(pass == PASS_FINAL && reflection_enabled)
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
	if(game_state == GAME_STARTING) scale_m4(temp, 1.0, global_time/WALL_GROW_TIME, 1.0);
	else if(game_state == GAME_ENDING) scale_m4(temp, 1.0, 1.0-((global_time-time_endgame)/WALL_GROW_TIME), 1.0);
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
			rotate_m4(temp, global_time*50.0, 0.0, 1.0, 0.0);
			rotate_m4(temp, global_time*35, 1.0, 0.0, 0.0);
			drawer_modelview_set(temp);
			drawer_draw_mesh(pyramid);
		}
	}
	drawer_depth_mask(1);
}
