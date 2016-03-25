/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#include "noise.h"

#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

static float interpolate(float x, float a, float b)
{
	x = cos(x*M_PI) * -0.5 + 0.5;
	return a + x * (b-a);
}

void noise_generate_texture2d_channel(int gridsize, int width, int height, int stride, float *data)
{
	if(stride == 0) stride = 1;

	int i, j;

	float **noise_grid;
	noise_grid = (float **)malloc(gridsize * sizeof(float *));
	for (i = 0; i<gridsize; i++)
		noise_grid[i] = (float *)malloc(gridsize * sizeof(float));

	for(i=0; i<gridsize; i++) for(j=0; j<gridsize; j++) noise_grid[i][j] = rand()/(float)RAND_MAX;

	for(j=0; j<height; j++) for(i=0; i<width; i++)
	{
		int x1 = (int)floor(i/(width/gridsize)) % gridsize;
		int x2 = (x1+1) % gridsize;
		int y1 = (int)floor(j/(height/gridsize)) % gridsize;
		int y2 = (y1+1) % gridsize;
		float xfac = (i%(width/gridsize))/((float)(width/gridsize));
		float yfac = (j%(height/gridsize))/((float)(height/gridsize));
		float v1 = interpolate(xfac, noise_grid[x1][y1], noise_grid[x2][y1]);
		float v2 = interpolate(xfac, noise_grid[x1][y2], noise_grid[x2][y2]);
		float value = interpolate(yfac, v1, v2);
		data[(i+j*width)*stride] = value;
	}
	free(noise_grid);
}
