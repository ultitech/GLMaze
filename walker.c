#include "walker.h"

#include "math.h"

#include <stdlib.h>
#include <string.h>

//Prototypes:
void walker_walk(Walker *walker);
void walker_rotate(Walker *walker);

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

void walker_create_new_interpolation(Walker *walker)
{
	Cell *cur_cell = maze_get_cell(walker->maze, walker->cell[0], walker->cell[1]);
	
	if(cell_passage_in_direction(cur_cell, walker->direction)) walker_walk(walker);
	else walker_rotate(walker);
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
	result[0] = walker->interp_start[0] * (1.0 - walker->interp_step) + walker->interp_end[0] * walker->interp_step;
	result[1] = walker->interp_start[1] * (1.0 - walker->interp_step) + walker->interp_end[1] * walker->interp_step;
	result[2] = walker->interp_start[2] * (1.0 - walker->interp_step) + walker->interp_end[2] * walker->interp_step;
	walker->interp_callback(result);
}

void walker_position_from_cell_pos(float pos[3], int cell[2])
{
	pos[0] = (float)cell[0] + 0.5;
	pos[1] = 0.0;
	pos[2] = (float)cell[1] + 0.5;
}

/*
void walker_rotation_from_direction(float rot[3], enum Direction dir)
{
	memset(rot, 0x00, sizeof(float)*3);
	switch(dir)
	{
		case UP:
		//rot[0] = 0.0;
		break;
		
		case DOWN:
		rot[0] = 180.0;
		break;
		
		case LEFT:
		rot[0] = 270.0;
		break;
		
		case RIGHT:
		rot[0] = 90.0;
		break;
	}
}
*/

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
}

void walker_rotate(Walker *walker)
{
	walker->interp_step = 0.0;
	
	memset(walker->interp_start, 0x00, sizeof(float)*3);
	walker->interp_start[0] = walker->child_pan;
	
	enum Direction next_direction;
	switch(walker->direction)
	{
		case UP:
		next_direction = RIGHT;
		break;
		
		case RIGHT:
		next_direction = DOWN;
		break;
		
		case DOWN:
		next_direction = LEFT;
		break;
		
		case LEFT:
		next_direction = UP;
		break;
	}
	vec_set(walker->interp_end, walker->interp_start);
	walker->interp_end[0] += 90.0;
	
	walker->interp_callback = walker->set_rotation_callback;
	
	walker->direction = next_direction;
	walker->child_pan += 90.0;
}

Walker* walker_create(Maze *maze, int start_cell_pos[2], enum Direction start_dir, void(*pos_callback)(float pos[3]), void(*rot_callback)(float rot[3]))
{
	Walker *walker = malloc(sizeof(Walker));
	walker->maze = maze;
	memcpy(walker->cell, start_cell_pos, sizeof(float)*2);
	walker->direction = start_dir;
	walker->interp_speed = 1.0;
	walker->child_pan = walker_pan_from_direction(walker->direction);
	walker->set_position_callback = pos_callback;
	walker->set_rotation_callback = rot_callback;
	
	walker_create_new_interpolation(walker);
	
	return walker;
}

Walker* walker_create_random(Maze *maze);

void walker_step(Walker *walker, float time_step)
{
	walker_interpolate(walker, time_step);
}
