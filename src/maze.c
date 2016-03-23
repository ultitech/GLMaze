/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#include "maze.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int maze_cell_isfree(Maze *maze, unsigned int x, unsigned int y)
{
	Cell *cell = maze_get_cell(maze, x, y);
	return !cell->up && !cell->down && !cell->left && !cell->right;
}

void maze_generate_cell(Maze *maze, Cell *cell)
{
	enum Direction paths[4];
	int directions;

	while(1)
	{
		directions = 0;
		if(cell->y != 0) if(maze_cell_isfree(maze, cell->x, cell->y-1)) paths[directions++] = UP;
		if(cell->y != maze->height-1) if(maze_cell_isfree(maze, cell->x, cell->y+1)) paths[directions++] = DOWN;
		if(cell->x != 0) if(maze_cell_isfree(maze, cell->x-1, cell->y)) paths[directions++] = LEFT;
		if(cell->x != maze->width-1) if(maze_cell_isfree(maze, cell->x+1, cell->y)) paths[directions++] = RIGHT;

		if(!directions) break;

		int index = rand() % directions;
		Cell *new_cell = NULL;
		switch(paths[index])
		{
			case UP:
			cell->up = 1;
			new_cell = maze_get_cell(maze, cell->x, cell->y-1);
			new_cell->down = 1;
			break;

			case DOWN:
			cell->down = 1;
			new_cell = maze_get_cell(maze, cell->x, cell->y+1);
			new_cell->up = 1;
			break;

			case LEFT:
			cell->left = 1;
			new_cell = maze_get_cell(maze, cell->x-1, cell->y);
			new_cell->right = 1;
			break;

			case RIGHT:
			cell->right = 1;
			new_cell = maze_get_cell(maze, cell->x+1, cell->y);
			new_cell->left = 1;
			break;
		}
		maze_generate_cell(maze, new_cell);
	}
}

Maze* maze_generate(unsigned int width, unsigned int height)
{
	Maze *maze = malloc(sizeof(Maze));
	maze->width = width;
	maze->height = height;

	maze->cells = calloc(width*height, sizeof(Cell));
	//Coordinate System:
	// + x->
	// y
	// |
	// \/
	//Write Positions
	unsigned int x, y;
	for(x=0; x<width; x++) for(y=0; y<height; y++)
	{
		Cell *cell = maze_get_cell(maze, x, y);
		cell->x = x;
		cell->y = y;
	}

	Cell *cell = maze_get_cell(maze, rand() % width, rand() % height);
	maze_generate_cell(maze, cell);

	maze_get_cell(maze, 0, 0)->object = OBJ_START;
	maze_get_cell(maze, maze->width-1, maze->height-1)->object = OBJ_FINISH;

	//create twisters
	int i;
	for(i=0; i<10; i++)
	{
		Cell *cell;
		do cell = maze_get_cell(maze, rand()%maze->width, rand()%maze->height);
		while(cell->object != OBJ_NONE);
		cell->object = OBJ_TWISTER;
	}

	return maze;
}

Cell* maze_get_cell(Maze *maze, unsigned int x, unsigned int y)
{
	return &maze->cells[y*maze->width+x];
}

void maze_free(Maze *maze)
{
	free(maze->cells);
	free(maze);
}

void maze_print(Maze *maze)
{
	char *buf;
	buf = (char *)malloc((maze->width + 1) * 2 * sizeof(char));

	unsigned int x, y;

	strcpy(buf, "+");
	for(x=0; x<maze->width; x++)
	{
		strcat(buf, "-+");
	}
	puts(buf);

	for(y=0; y<maze->height; y++)
	{
		strcpy(buf, "|");
		for(x=0; x<maze->width; x++)
		{
			Cell *cell = maze_get_cell(maze, x, y);
			if(cell->object == OBJ_TWISTER) strcat(buf, "T");
			else if(cell->object == OBJ_START) strcat(buf, "S");
			else if(cell->object == OBJ_FINISH) strcat(buf, "F");
			else strcat(buf, " ");
			if(cell->right) strcat(buf, " ");
			else strcat(buf, "|");
		}
		puts(buf);

		strcpy(buf, "+");
		for(x=0; x<maze->width; x++)
		{
			Cell *cell = maze_get_cell(maze, x, y);
			if(cell->down) strcat(buf, " ");
			else strcat(buf, "-");
			strcat(buf, "+");
		}
		puts(buf);
	}
	free(buf);
}
