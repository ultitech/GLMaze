#include "drawer.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <MathLib.h>
#include <ShaderLib.h>

#include <stdlib.h>
#include <stdio.h>

float mat_projection[16], mat_modelview[16];
int screen_size[2] = {1280, 800};
enum Render3DMode render_3d_mode = RENDER_3D_OFF;

void update_matrices()
{
	float mvp[16];
	copy_m4_m4(mvp, mat_projection);
	mul_m4_m4(mvp, mat_modelview);
	
	GLuint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	if(program == 0) return;
	
	GLint location;
	location = glGetUniformLocation(program, "MVMatrix");
	if(location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, mat_modelview);
	location = glGetUniformLocation(program, "MVPMatrix");
	if(location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, mvp);
}

void drawer_init()
{
	ilInit();
	iluInit();
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(screen_size[0], screen_size[1], 32, SDL_OPENGL);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	create_perspective_m4(mat_projection, 90.0, (float)screen_size[0]/(float)screen_size[1], 0.1, 100.0);
}

void drawer_quit()
{
	SDL_Quit();
}

Program drawer_create_program(char *vertex_filename, char *fragment_filename)
{
	GLuint vertex_shader, fragment_shader;
	
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	Shader_SetSourceFile(vertex_shader, vertex_filename);
	glCompileShader(vertex_shader);
	if(!SHADER_COMPILED(vertex_shader))
	{
		printf("Failed to compile %s:\n", vertex_filename);
		SHADER_PRINTLOG(vertex_shader);
	}
	
	fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	Shader_SetSourceFile(fragment_shader, fragment_filename);
	glCompileShader(fragment_shader);
	if(!SHADER_COMPILED(fragment_shader))
	{
		printf("Failed to compile %s:\n", fragment_filename);
		SHADER_PRINTLOG(fragment_shader);
	}
	
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	if(!PROGRAM_LINKED(program))
	{
		printf("Failed to link program:\n");
		PROGRAM_PRINTLOG(program);
	}
	
	return program;
}

void drawer_use_program(Program program)
{
	glUseProgram(program);
	update_matrices();
}

void drawer_draw_mesh(Mesh *mesh)
{
	GLuint program;
	glGetIntegerv(GL_CURRENT_PROGRAM, &program);
	
	GLuint stride = 0;
	int position_offset=0, normal_offset=0, texcoord_offset=0;
	if(mesh->vertex_format & VERTEX_POSITION)
	{
		position_offset = stride;
		stride += 3;
	}
	if(mesh->vertex_format & VERTEX_NORMAL)
	{
		normal_offset = stride;
		stride += 3;
	}
	if(mesh->vertex_format & VERTEX_TEXCOORD)
	{
		texcoord_offset = stride;
		stride += 2;
	}
	stride *= sizeof(GLfloat);
	
	GLuint location;
	if(mesh->vertex_format & VERTEX_POSITION)
	{
		location = glGetAttribLocation(program, "in_position");
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, stride, mesh->vertices+position_offset);
		glEnableVertexAttribArray(location);
	}
	
	if(mesh->vertex_format & VERTEX_NORMAL)
	{
		location = glGetAttribLocation(program, "in_normal");
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, stride, mesh->vertices+normal_offset);
		glEnableVertexAttribArray(location);
	}
	
	if(mesh->vertex_format & VERTEX_TEXCOORD)
	{
		location = glGetAttribLocation(program, "in_texcoord");
		glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, stride, mesh->vertices+texcoord_offset);
		glEnableVertexAttribArray(location);
	}
	
	glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_INT, mesh->indices);
}

Texture drawer_load_texture(char *filename)
{
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilLoadImage(filename);
	iluFlipImage();
	
	int image_size[2];
	image_size[0] = ilGetInteger(IL_IMAGE_WIDTH);
	image_size[1] = ilGetInteger(IL_IMAGE_HEIGHT);
	
	GLfloat *image_data = malloc(sizeof(GLfloat) * image_size[0] * image_size[1] * 3);
	ilCopyPixels(0, 0, 0, image_size[0], image_size[1], 1, IL_RGB, IL_FLOAT, image_data);
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image_size[0], image_size[1], 0, GL_RGB, GL_FLOAT, image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	ilDeleteImages(1, &image);
	free(image_data);
	
	return texture;
}

void drawer_use_texture(Texture texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
}

void drawer_modelview_set(float matrix[16])
{
	copy_m4_m4(mat_modelview, matrix);
	update_matrices();
}

void drawer_modelview_get(float matrix[16])
{
	copy_m4_m4(matrix, mat_modelview);
}

void drawer_depth_mask(unsigned char mask)
{
	glDepthMask(mask);
}

void drawer_3d_reset()
{
	if(render_3d_mode == RENDER_3D_ANAGLYPH) glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if(render_3d_mode == RENDER_3D_SIDEBYSIDE) glViewport(0, 0, screen_size[0], screen_size[1]);
}

void drawer_3d_left()
{
	if(render_3d_mode == RENDER_3D_ANAGLYPH)
	{
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	if(render_3d_mode == RENDER_3D_SIDEBYSIDE) glViewport(0, 0, screen_size[0]/2, screen_size[1]);
}

void drawer_3d_right()
{
	if(render_3d_mode == RENDER_3D_ANAGLYPH)
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	if(render_3d_mode == RENDER_3D_SIDEBYSIDE) glViewport(screen_size[0]/2, 0, screen_size[0]/2, screen_size[1]);
}

enum Render3DMode drawer_get_3d_mode()
{
	return render_3d_mode;
}

int drawer_do_events()
{
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		if(ev.type == SDL_QUIT) return 0;
		if(ev.type == SDL_KEYDOWN)
		{
			if(ev.key.keysym.sym == SDLK_r) render_3d_mode = (render_3d_mode+1) % RENDER_3D_MODES_COUNT;
		}
	}
	return 1;
}

void drawer_begin_scene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void drawer_end_scene()
{
	SDL_GL_SwapBuffers();
}
