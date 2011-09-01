#ifndef H_WALKER
#define H_WALKER

#include "maze.h"

typedef struct
{
	Maze *maze;
	
	float pos_interp_start[3], pos_interp_end[3];
	float rot_interp_start[3], rot_interp_end[3];
	float interp_step, interp_speed;
	
	void(*set_position_callback)(float pos[3]);
	void(*set_rotation_callback)(float rot[3]);
	
	void(*finish_callback)();
	
	int cell[2];
	enum Direction direction;
	
	void *user_data;
} Walker;

Walker* walker_create(Maze *maze, int start_cell[2], enum Direction start_dir, void(*pos_callback)(float pos[3]), void(*rot_callback)(float rot[3]), void(*fin_callback)());
void walker_step(Walker *walker, float time_step);

#endif //H_MAZE
