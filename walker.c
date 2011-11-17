#include "walker.h"

#include <stdlib.h>
#include <string.h>
#include <MathLib.h>

static int cell_passage_in_direction(Cell *cell, enum Direction dir);
static enum Direction walker_rotate_direction_right(enum Direction dir);
static enum Direction walker_rotate_direction_left(enum Direction dir);
static void walker_create_new_interpolation(Walker *walker);
static float walker_pan_from_direction(enum Direction dir);
static void walker_get_global_position(Walker *walker, float pos[3]);
static void walker_straight(Walker *walker);
static void walker_right(Walker *walker);
static void walker_left(Walker *walker);
static void walker_turn(Walker *walker);
static void walker_twist(Walker *walker);

#define copy_cell(target, source) memcpy(target, source, sizeof(float)*2)

Walker* walker_create(Maze *maze, int start_cell_pos[2], enum Direction start_dir, void(*pos_callback)(float pos[3]), void(*rot_callback)(float rot[3]), void(*fin_callback)())
{
	Walker *walker = malloc(sizeof(Walker));
	walker->maze = maze;
	copy_cell(walker->cell, start_cell_pos);
	walker->direction = start_dir;
	walker->interp_step = 0.0;
	walker->interp_speed = 1.0;
	walker->set_position_callback = pos_callback;
	walker->set_rotation_callback = rot_callback;
	walker->finish_callback = fin_callback;
	
	float position[3];
	walker_get_global_position(walker, position);
	copy_v3_v3(walker->pos_interp_start, position);
	copy_v3_v3(walker->pos_interp_end, position);
	walker->set_position_callback(walker->pos_interp_start);
	float rotation[3];
	rotation[0] = walker_pan_from_direction(walker->direction);
	rotation[1] = rotation[2] = 0.0;
	copy_v3_v3(walker->rot_interp_start, rotation);
	copy_v3_v3(walker->rot_interp_end, rotation);
	walker->set_rotation_callback(rotation);
	
	walker_create_new_interpolation(walker);
	
	return walker;
}

void walker_step(Walker *walker, float time_step)
{
	walker->interp_step += time_step * walker->interp_speed;
	if(walker->interp_step >= 1.0)
	{
		copy_v3_v3(walker->pos_interp_start, walker->pos_interp_end);
		copy_v3_v3(walker->rot_interp_start, walker->rot_interp_end);
		walker->interp_step -= 1.0;
		walker_create_new_interpolation(walker);
		return;
	}
	float result[3];
	interp_v3_v3v3(result, walker->pos_interp_start, walker->pos_interp_end, walker->interp_step);
	walker->set_position_callback(result);
	interp_v3_v3v3(result, walker->rot_interp_start, walker->rot_interp_end, walker->interp_step);
	walker->set_rotation_callback(result);
}

static int cell_passage_in_direction(Cell *cell, enum Direction dir)
{
	switch(dir)
	{
		case UP:
		return cell->up;
		break;
		
		case DOWN:
		return cell->down;
		break;
		
		case LEFT:
		return cell->left;
		break;
		
		case RIGHT:
		return cell->right;
		break;
	}
}

static enum Direction walker_rotate_direction_right(enum Direction dir)
{
	switch(dir)
	{
		case UP:
		return RIGHT;
		break;
		
		case RIGHT:
		return DOWN;
		break;
		
		case DOWN:
		return LEFT;
		break;
		
		case LEFT:
		return UP;
		break;
	}
}

static enum Direction walker_rotate_direction_left(enum Direction dir)
{
	switch(dir)
	{
		case UP:
		return LEFT;
		break;
		
		case RIGHT:
		return UP;
		break;
		
		case DOWN:
		return RIGHT;
		break;
		
		case LEFT:
		return DOWN;
		break;
	}
}

static void walker_create_new_interpolation(Walker *walker)
{
	Cell *cur_cell = maze_get_cell(walker->maze, walker->cell[0], walker->cell[1]);
	
	if(cur_cell->object == OBJ_TWISTER)
	{
		walker_twist(walker);
		cur_cell->object = OBJ_NONE;
		return;
	}
	if(cur_cell->object == OBJ_FINISH)
	{
		walker->finish_callback();
		return;
	}
	
	if(cell_passage_in_direction(cur_cell, walker_rotate_direction_right(walker->direction))) walker_right(walker); //first look right
	else if(cell_passage_in_direction(cur_cell, walker->direction)) walker_straight(walker); //then forward
	else if(cell_passage_in_direction(cur_cell, walker_rotate_direction_left(walker->direction))) walker_left(walker); //then left
	else walker_turn(walker); //else turn right (180 degrees)
}

static float walker_pan_from_direction(enum Direction dir)
{
	switch(dir)
	{
		case UP:
		return 0.0;
		break;
		
		case DOWN:
		return 180.0;
		break;
		
		case LEFT:
		return 270.0;
		break;
		
		case RIGHT:
		return 90.0;
		break;
	}
}

static void walker_get_global_position(Walker *walker, float pos[3])
{
	pos[0] = (float)walker->cell[0] + 0.5;
	pos[1] = 0.0;
	pos[2] = (float)walker->cell[1] + 0.5;
	switch(walker->direction)
	{
		case UP:
		pos[2] += 0.5;
		break;
		
		case DOWN:
		pos[2] -= 0.5;
		break;
		
		case LEFT:
		pos[0] += 0.5;
		break;
		
		case RIGHT:
		pos[0] -= 0.5;
		break;
	}
}

static void walker_straight(Walker *walker)
{
	switch(walker->direction)
	{
		case UP:
		walker->cell[1]--;
		break;
		
		case DOWN:
		walker->cell[1]++;
		break;
		
		case RIGHT:
		walker->cell[0]++;
		break;
		
		case LEFT:
		walker->cell[0]--;
		break;
	}
	walker_get_global_position(walker, walker->pos_interp_end);
}

static void walker_right(Walker *walker)
{
	walker->direction = walker_rotate_direction_right(walker->direction);
	walker_straight(walker);
	
	walker->rot_interp_end[0] += 90.0;
}

static void walker_left(Walker *walker)
{
	walker->direction = walker_rotate_direction_left(walker->direction);
	walker_straight(walker);
	
	walker->rot_interp_end[0] -= 90.0;
}

static void walker_turn(Walker *walker)
{
	walker->direction = walker_rotate_direction_right(walker->direction);
	walker->direction = walker_rotate_direction_right(walker->direction);
	switch(walker->direction)
	{
		case UP:
		walker->cell[1]--;
		break;
		
		case DOWN:
		walker->cell[1]++;
		break;
		
		case RIGHT:
		walker->cell[0]++;
		break;
		
		case LEFT:
		walker->cell[0]--;
		break;
	}
	
	walker->rot_interp_end[0] += 180.0;
}

static void walker_twist(Walker *walker)
{
	walker->rot_interp_end[2] += 180.0;
}
