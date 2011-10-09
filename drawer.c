#include "drawer.h"
#include "mesh.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_image.h>
#include <MathLib.h>
#include <ShaderLib.h>

#include <stdlib.h>
#include <stdio.h>

float mat_projection[16], mat_modelview[16];
int screen_size[2] = {1280, 800};
enum Render3DMode render_3d_mode = RENDER_3D_OFF;

GLuint pp_passes[16];
GLuint pp_passes_count = 0;
struct Rendertarget
{
	GLuint buffer;
	GLuint image;
} pp_draw_targets[2];
GLuint pp_vertex_shader;

Mesh *screen_square_mesh;

static GLuint create_shader(GLenum type, char *filename);

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

static void create_rendertarget(struct Rendertarget *target)
{
	glGenFramebuffers(1, &target->buffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->buffer);
	
	glGenTextures(1, &target->image);
	glBindTexture(GL_TEXTURE_RECTANGLE, target->image);
	GLfloat *img_buffer = malloc(sizeof(GLfloat)*screen_size[0]*screen_size[1]*3);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, screen_size[0], screen_size[1], 0, GL_RGB, GL_FLOAT, img_buffer);
	free(img_buffer);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, target->image, 0);
	
	GLuint depth_buffer;
	glGenRenderbuffers(1, &depth_buffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_size[0], screen_size[1]);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
}

void drawer_init()
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_JPG);
	
	SDL_SetVideoMode(screen_size[0], screen_size[1], 32, SDL_OPENGL);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	create_perspective_m4(mat_projection, 90.0, (float)screen_size[0]/(float)screen_size[1], 0.1, 100.0);
	
	create_rendertarget(&pp_draw_targets[0]);
	create_rendertarget(&pp_draw_targets[1]);
	
	pp_vertex_shader = create_shader(GL_VERTEX_SHADER, "Shader/pp.glslv");
	
	screen_square_mesh = mesh_create_screen_square();
}

void drawer_quit()
{
	SDL_Quit();
}

static GLuint create_shader(GLenum type, char *filename)
{
	GLuint shader = glCreateShader(type);
	Shader_SetSourceFile(shader, filename);
	glCompileShader(shader);
	if(!SHADER_COMPILED(shader))
	{
		printf("Failed to compile %s:\n", filename);
		SHADER_PRINTLOG(shader);
	}
	return shader;
}

static GLuint create_program(GLuint vertex_shader, GLuint fragment_shader)
{
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

Program drawer_create_program(char *vertex_filename, char *fragment_filename)
{
	GLuint vertex_shader, fragment_shader, program;
	
	vertex_shader = create_shader(GL_VERTEX_SHADER, vertex_filename);
	fragment_shader = create_shader(GL_FRAGMENT_SHADER, fragment_filename);
	program = create_program(vertex_shader, fragment_shader);
	
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
	
	GLsizei stride = 0;
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
	
	GLint location;
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
	SDL_Surface *img = IMG_Load(filename);
	unsigned int rmask, gmask, bmask, amask;
	
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000; gmask = 0x00ff0000; bmask = 0x0000ff00; amask = 0x000000ff;
#else
	rmask = 0x000000ff; gmask = 0x0000ff00; bmask = 0x00ff0000; amask = 0xff000000;
#endif
	
	SDL_Surface *tex = SDL_CreateRGBSurface(SDL_SWSURFACE, img->w, img->h, 32, rmask, gmask, bmask, amask);
	SDL_BlitSurface(img, NULL, tex, NULL);
	
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex->w, tex->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	SDL_FreeSurface(img);
	SDL_FreeSurface(tex);
	
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

void calc_gauss_values(GLint location)
{
	const float sigma = 4.0;
	float values[11][2];
	int i;
	for(i=0; i<11; i++)
	{
		float x = i-5.0;
		values[i][0] = x;
		values[i][1] = (1.0/sqrtf(2.0*M_PI*sigma*sigma))*powf(M_E,-((x*x)/(2.0*sigma*sigma)));
		printf("[%f,%f] ", values[i][0], values[i][1]);
	}
	printf("\n");
	glUniform2fv(location, 11, values);
}

void drawer_postprocess_pass_add(char *filename)
{
	GLuint fragment_shader = create_shader(GL_FRAGMENT_SHADER, filename);
	GLuint program = create_program(pp_vertex_shader, fragment_shader);
	pp_passes[pp_passes_count++] = program;
	
	glUseProgram(program);
	
	GLint location;
	location = glGetUniformLocation(program, "gaussValues");
	if(location != -1) calc_gauss_values(location);
	location = glGetUniformLocation(program, "screen_size");
	if(location != -1) glUniform2iv(location, 1, screen_size);
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
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pp_draw_targets[0].buffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void drawer_do_postprocess()
{
	struct Rendertarget read = pp_draw_targets[0], draw = pp_draw_targets[1];
	struct Rendertarget window = {0, 0};
	int pass;
	for(pass=0; pass<pp_passes_count; pass++)
	{
		if(pass != 0) //do not swap on first pass
		{
			struct Rendertarget temp;
			temp = draw;
			draw = read;
			read = temp;
		}
		if(pass == pp_passes_count-1) draw = window;
		glUseProgram(pp_passes[pass]);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw.buffer);
		glBindTexture(GL_TEXTURE_RECTANGLE, read.image);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawer_draw_mesh(screen_square_mesh);
	}
}

void drawer_end_scene()
{
	SDL_GL_SwapBuffers();
}
