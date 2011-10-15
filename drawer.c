#include "drawer.h"
#include "mesh.h"

#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <IL/il.h>
#include <MathLib.h>
#include <ShaderLib.h>

#include <stdlib.h>
#include <stdio.h>

float mat_projection[16], mat_modelview[16];
int screen_size[2] = {1280, 800};
enum Render3DMode render_3d_mode = RENDER_3D_OFF;
GLuint current_program;

struct PostProcessPass
{
	GLuint shader;
	GLuint program;
	int key;
	unsigned enabled:1;
} pp_passes[16];
GLuint pp_passes_count = 0;
struct Rendertarget
{
	GLuint buffer;
	GLuint image;
} pp_draw_targets[2];
GLuint pp_vertex_shader, pp_fragment_shader, pp_program;

MeshVBO *screen_square_mesh;

static void update_matrices();
static void create_rendertarget(struct Rendertarget *target);
static GLuint create_shader(GLenum type, char *filename);
static GLuint create_program(GLuint vertex_shader, GLuint fragment_shader);
static void calc_gauss_values(GLint location);
static void screenshot();

void drawer_init()
{
	ilInit();
	ilEnable(IL_FILE_OVERWRITE);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_SetVideoMode(screen_size[0], screen_size[1], 32, SDL_OPENGL);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	create_perspective_m4(mat_projection, 90.0, (float)screen_size[0]/(float)screen_size[1], 0.1, 100.0);
	
	create_rendertarget(&pp_draw_targets[0]);
	create_rendertarget(&pp_draw_targets[1]);
	
	pp_vertex_shader = create_shader(GL_VERTEX_SHADER, "Shader/pp.glslv");
	pp_fragment_shader = create_shader(GL_FRAGMENT_SHADER, "Shader/pp.glslf");
	pp_program = create_program(pp_vertex_shader, pp_fragment_shader);
	
	Mesh *m = mesh_create_screen_square();
	screen_square_mesh = drawer_create_mesh_vbo(m);
	mesh_free(m);
}

void drawer_quit()
{
	SDL_Quit();
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
	current_program = program;
	update_matrices();
}

Texture drawer_load_texture(char *filename)
{
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilLoadImage(filename);
	
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

void drawer_depth_mask(unsigned char mask)
{
	glDepthMask(mask);
}

void drawer_postprocess_pass_add(char *filename, int toggle_key)
{
	struct PostProcessPass *pass = &pp_passes[pp_passes_count++];
	pass->key = toggle_key;
	pass->enabled = 1;
	pass->shader = create_shader(GL_FRAGMENT_SHADER, filename);
	pass->program = create_program(pp_vertex_shader, pass->shader);
	
	glUseProgram(pass->program);
	
	GLint location;
	location = glGetUniformLocation(pass->program, "gaussValues");
	if(location != -1) calc_gauss_values(location);
	location = glGetUniformLocation(pass->program, "screen_size");
	if(location != -1) glUniform2iv(location, 1, screen_size);
}

void drawer_do_postprocess()
{
	struct Rendertarget read = pp_draw_targets[0], draw = pp_draw_targets[1];
	struct Rendertarget window = {0, 0};
	GLuint enabled_passes[pp_passes_count];
	int enabled_passes_count = 0;
	int pass;
	
	for(pass=0; pass<pp_passes_count; pass++)
	{
		struct PostProcessPass *p = &pp_passes[pass];
		if(p->enabled) enabled_passes[enabled_passes_count++] = p->program;
	}
	
	if(enabled_passes_count == 0)
	{
		enabled_passes[0] = pp_program;
		enabled_passes_count = 1;
	}
	
	for(pass=0; pass<enabled_passes_count; pass++)
	{
		if(pass != 0) //do not swap on first pass
		{
			struct Rendertarget temp;
			temp = draw;
			draw = read;
			read = temp;
		}
		if(pass == enabled_passes_count-1) draw = window;
		
		glUseProgram(enabled_passes[pass]);
		
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw.buffer);
		glBindTexture(GL_TEXTURE_RECTANGLE, read.image);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawer_draw_mesh_vbo(screen_square_mesh);
	}
}

int drawer_do_events()
{
	SDL_Event ev;
	while(SDL_PollEvent(&ev))
	{
		if(ev.type == SDL_QUIT) return 0;
		if(ev.type == SDL_KEYDOWN)
		{
			SDLKey key = ev.key.keysym.sym;
			if(key == SDLK_r) render_3d_mode = (render_3d_mode+1) % RENDER_3D_MODES_COUNT;
			else if(key == SDLK_F12) screenshot();
			else
			{
				int i;
				for(i=0; i<pp_passes_count; i++)
				{
					struct PostProcessPass *p = &pp_passes[i];
					if(p->key == key) p->enabled ^= 1;
				}
			}
		}
	}
	return 1;
}

void drawer_begin_scene()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, pp_draw_targets[0].buffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void drawer_end_scene()
{
	SDL_GL_SwapBuffers();
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

MeshVBO* drawer_create_mesh_vbo(Mesh *mesh)
{
	MeshVBO *vbo = malloc(sizeof(MeshVBO));
	vbo->vertex_format = mesh->vertex_format;
	vbo->vertices_count = mesh->vertices_count;
	vbo->indices_count = mesh->indices_count;
	
	glGenBuffers(1, &vbo->vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh_get_vertex_size(vbo->vertex_format) * vbo->vertices_count, mesh->vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &vbo->index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vbo->indices_count, mesh->indices, GL_STATIC_DRAW);
	
	return vbo;
}

void drawer_draw_mesh_vbo(MeshVBO *vbo)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo->vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->index_buffer);
	
	GLsizei stride = 0;
	int position_offset=0, normal_offset=0, texcoord_offset=0;
	if(vbo->vertex_format & VERTEX_POSITION)
	{
		position_offset = stride;
		stride += 3;
	}
	if(vbo->vertex_format & VERTEX_NORMAL)
	{
		normal_offset = stride;
		stride += 3;
	}
	if(vbo->vertex_format & VERTEX_TEXCOORD)
	{
		texcoord_offset = stride;
		stride += 2;
	}
	stride *= sizeof(GLfloat);
	
	GLint location;
	if(vbo->vertex_format & VERTEX_POSITION)
	{
		location = glGetAttribLocation(current_program, "in_position");
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, stride, ((GLfloat*)NULL)+position_offset);
		glEnableVertexAttribArray(location);
	}
	
	if(vbo->vertex_format & VERTEX_NORMAL)
	{
		location = glGetAttribLocation(current_program, "in_normal");
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, stride, ((GLfloat*)NULL)+normal_offset);
		glEnableVertexAttribArray(location);
	}
	
	if(vbo->vertex_format & VERTEX_TEXCOORD)
	{
		location = glGetAttribLocation(current_program, "in_texcoord");
		glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, stride, ((GLfloat*)NULL)+texcoord_offset);
		glEnableVertexAttribArray(location);
	}
	
	glDrawElements(GL_TRIANGLES, vbo->indices_count, GL_UNSIGNED_INT, ((GLuint*)NULL));
}

void drawer_free_mesh_vbo(MeshVBO *vbo)
{
	glDeleteBuffers(1, vbo->vertex_buffer);
	glDeleteBuffers(1, vbo->index_buffer);
	free(vbo);
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

static void calc_gauss_values(GLint location)
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

static void update_matrices()
{
	float mvp[16];
	copy_m4_m4(mvp, mat_projection);
	mul_m4_m4(mvp, mat_modelview);
	
	if(current_program == 0) return;
	
	GLint location;
	location = glGetUniformLocation(current_program, "MVMatrix");
	if(location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, mat_modelview);
	location = glGetUniformLocation(current_program, "MVPMatrix");
	if(location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, mvp);
}

static void screenshot()
{
	const unsigned int w = screen_size[0], h = screen_size[1];
	GLfloat *data = malloc(sizeof(GLfloat) * w * h * 3);
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	GLuint read;
	glGetIntegerv(GL_READ_BUFFER, &read);
	glReadBuffer(GL_FRONT);
	
	glReadPixels(0, 0, w, h, GL_RGB, GL_FLOAT, data);
	
	glReadBuffer(read);
	
	char filename[32];
	int index;
	for(index=0;;index++)
	{
		sprintf(filename, "Screenshot%d.jpg", index);
		FILE *f;
		if((f = fopen(filename, "r")) == NULL) break;
		else fclose(f);
	}
	
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilTexImage(w, h, 1, 3, IL_RGB, IL_FLOAT, data);
	
	ilDisable(IL_ORIGIN_SET);
	ilSaveImage(filename);
	ilEnable(IL_ORIGIN_SET);
	
	ilDeleteImages(1, &image);
	free(data);
}
