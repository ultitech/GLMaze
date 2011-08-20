#include "walker.h"

#include <stdlib.h>
#include <string.h>
#include <MathLib.h>

//Prototypes:
void walker_walk(Walker *walker);
void walker_rotate_right(Walker *walker);
void walker_rotate_left(Walker *walker);
void walker_twist(Walker *walker);

int cell_passage_in_direction(Cell *cell, enum Direction dir)
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

enum Direction walker_rotate_direction_right(enum Direction dir)
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

enum Direction walker_rotate_direction_left(enum Direction dir)
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

void walker_create_new_interpolation(Walker *walker)
{
	Cell *cur_cell = maze_get_cell(walker->maze, walker->cell[0], walker->cell[1]);
	
	if(cur_cell->object == OBJ_TWISTER)
	{
		walker_twist(walker);
		cur_cell->object = OBJ_NONE;
		return;
	}
	
	if(walker->last_operation == WALKER_WALK)
	{
		if(cell_passage_in_direction(cur_cell, walker_rotate_direction_right(walker->direction))) walker_rotate_right(walker); //first look right
		else if(cell_passage_in_direction(cur_cell, walker->direction)) walker_walk(walker); //then forward
		else if(cell_passage_in_direction(cur_cell, walker_rotate_direction_left(walker->direction))) walker_rotate_left(walker); //then left
		else walker_rotate_right(walker); //else turn right (180 degrees)
	}
	else
	{
		if(cell_passage_in_direction(cur_cell, walker->direction)) walker_walk(walker);
		else walker_rotate_right(walker);
	}
}

void walker_interpolate(Walker *walker, float time_step)
{
	walker->interp_step += time_step * walker->interp_speed;
	if(walker->interp_step >= 1.0)
	{
		walker->interp_callback(walker->interp_end);
		walker_create_new_interpolation(walker);
		return;
	}
	float result[3];
	interp_v3_v3v3(result, walker->interp_start, walker->interp_end, walker->interp_step);
	walker->interp_callback(result);
}

void walker_position_from_cell_pos(float pos[3], int cell[2])
{
	pos[0] = (float)cell[0] + 0.5;
	pos[1] = 0.0;
	pos[2] = (float)cell[1] + 0.5;
}

float walker_pan_from_direction(enum Direction dir)
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

void walker_walk(Walker *walker)
{
	walker->interp_step = 0.0;
	
	walker_position_from_cell_pos(walker->interp_start, walker->cell);
	
	int next_cell[2];
	next_cell[0] = walker->cell[0];
	next_cell[1] = walker->cell[1];
	switch(walker->direction)
	{
		case UP:
		next_cell[1]--;
		break;
		
		case DOWN:
		next_cell[1]++;
		break;
		
		case RIGHT:
		next_cell[0]++;
		break;
		
		case LEFT:
		next_cell[0]--;
		break;
	}
	walker_position_from_cell_pos(walker->interp_end, next_cell);
	
	walker->interp_callback = walker->set_position_callback;
	
	memcpy(walker->cell, next_cell, sizeof(float)*2);
	walker->last_operation = WALKER_WALK;
}

void walker_rotate_right(Walker *walker)
{
	walker->interp_step = 0.0;
	
	memset(walker->interp_start, 0x00, sizeof(float)*3);
	walker->interp_start[0] = walker->child_pan;
	walker->interp_start[2] = walker->child_roll;
	
	enum Direction next_direction;
	next_direction = walker_rotate_direction_right(walker->direction);
	copy_v3_v3(walker->interp_end, walker->interp_start);
	walker->interp_end[0] += 90.0;
	
	walker->interp_callback = walker->set_rotation_callback;
	
	walker->direction = next_direction;
	walker->child_pan += 90.0;
	walker->last_operation = WALKER_ROTATE;
}

void walker_rotate_left(Walker *walker)
{
	walker->interp_step = 0.0;
	
	memset(walker->interp_start, 0x00, sizeof(float)*3);
	walker->interp_start[0] = walker->child_pan;
	walker->interp_start[2] = walker->child_roll;
	
	enum Direction next_direction;
	next_direction = walker_rotate_direction_left(walker->direction);
	copy_v3_v3(walker->interp_end, walker->interp_start);
	walker->interp_end[0] -= 90.0;
	
	walker->interp_callback = walker->set_rotation_callback;
	
	walker->direction = next_direction;
	walker->child_pan -= 90.0;
	walker->last_operation = WALKER_ROTATE;
}

void walker_twist(Walker *walker)
{
	walker->interp_step = 0.0;
	
	memset(walker->interp_start, 0x00, sizeof(float)*3);
	walker->interp_start[0] = walker->child_pan;
	walker->interp_start[2] = walker->child_roll;
	
	copy_v3_v3(walker->interp_end, walker->interp_start);
	walker->interp_end[2] += 180.0;
	
	walker->interp_callback = walker->set_rotation_callback;
	
	walker->child_roll += 180.0;
}

Walker* walker_create(Maze *maze, int start_cell_pos[2], enum Direction start_dir, void(*pos_callback)(float pos[3]), void(*rot_callback)(float rot[3]))
{
	Walker *walker = malloc(sizeof(Walker));
	walker->maze = maze;
	memcpy(walker->cell, start_cell_pos, sizeof(float)*2);
	walker->direction = start_dir;
	walker->interp_speed = 1.0;
	walker->child_pan = walker_pan_from_direction(walker->direction);
	walker->child_roll = 0.0;
	walker->set_position_callback = pos_callback;
	walker->set_rotation_callback = rot_callback;
	
	float position[3];
	walker_position_from_cell_pos(position, walker->cell);
	walker->set_position_callback(position);
	float rotation[3];
	rotation[0] = walker_pan_from_direction(walker->direction);
	rotation[1] = rotation[2] = 0.0;
	walker->set_rotation_callback(rotation);
	
	walker_create_new_interpolation(walker);
	
	return walker;
}

Walker* walker_create_random(Maze *maze);

void walker_step(Walker *walker, float time_step)
{
	walker_interpolate(walker, time_step);
}
