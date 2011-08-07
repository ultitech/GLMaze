#ifndef H_WALKER
#define H_WALKER

#include "maze.h"

enum WalkerOperation
{
	WALKER_ROTATE_RIGHT,
	WALKER_ROTATE_LEFT,
	WALKER_WALK
};

typedef struct
{
	Maze *maze;
	
	float interp_start[3];
	float interp_end[3];
	float interp_step, interp_speed;
	void(*interp_callback)(float val[3]);
	
	float child_pan;
	
	void(*set_position_callback)(float pos[3]);
	void(*set_rotation_callback)(float rot[3]);
	
	int cell[2];
	enum Direction direction;
	
	enum WalkerOperation last_operation;
	
	void *user_data;
} Walker;

Walker* walker_create(Maze *maze, int start_cell[2], enum Direction start_dir, void(*pos_callback)(float pos[3]), void(*rot_callback)(float rot[3]));
Walker* walker_create_random(Maze *maze);
void walker_step(Walker *walker, float time_step);

#endif //H_MAZE
