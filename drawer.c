#include "drawer.h"
#include "mesh.h"
#include "file.h"
#include "window.h"
#include "noise.h"

#include <GL/glew.h>
#include <IL/il.h>
#include <MathLib.h>

#include <stdlib.h>
#include <stdio.h>

float mat_projection[16], mat_modelview[16];
enum Render3DMode render_3d_mode = RENDER_3D_OFF;
GLuint current_program;
char vbo_bound = 0;
float global_time = 0.0;

struct PostProcessPass
{
	GLuint shader;
	GLuint program;
	int key;
	unsigned enabled:1;
} pp_passes[16];
GLuint pp_passes_count = 0;
GLuint pp_draw_targets[2];
GLuint pp_vertex_shader, pp_fragment_shader, pp_program;

Mesh *screen_square_mesh;
Texture noise_texture;
#define NOISE_TEXTURE_LAYER 7

static void update_uniforms();
static GLuint create_shader(GLenum type, char *filename);
static GLuint create_program(GLuint vertex_shader, GLuint fragment_shader);
static int uniform_exists(char *name, GLint *location);
static GLuint create_texture(GLsizei width, GLsizei height, GLenum format, GLfloat *data);
static GLuint generate_noise_texture();
static void calc_gauss_values(GLint location);
static void screenshot();
static void print_glinfo();
static void write_glinfo();
static void handle_keypress(char key);

void drawer_init()
{
	window_add_keypress_handler(handle_keypress);
	
	ilInit();
	ilEnable(IL_FILE_OVERWRITE);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	
	glewInit();
	
	print_glinfo();
	
	if(GLEW_ARB_vertex_buffer_object) mesh_generate_vbos(1);
	else mesh_generate_vbos(0);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	int size[2];
	window_get_size(size);
	create_perspective_m4(mat_projection, 90.0, (float)size[0]/(float)size[1], 0.1, 100.0);
	
	pp_draw_targets[0] = drawer_create_rendertarget();
	pp_draw_targets[1] = drawer_create_rendertarget();
	
	glActiveTexture(GL_TEXTURE0+NOISE_TEXTURE_LAYER);
	noise_texture = generate_noise_texture();
	glActiveTexture(GL_TEXTURE0);
	
	pp_vertex_shader = create_shader(GL_VERTEX_SHADER, "pp.glslv");
	pp_fragment_shader = create_shader(GL_FRAGMENT_SHADER, "pp.glslf");
	pp_program = create_program(pp_vertex_shader, pp_fragment_shader);
	
	screen_square_mesh = mesh_create_screen_square();
}

void drawer_quit()
{
}

void drawer_modelview_set(float matrix[16])
{
	copy_m4_m4(mat_modelview, matrix);
	update_uniforms();
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
	update_uniforms();
}

Texture drawer_load_texture(char *filename)
{
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	ilLoadImage(file_prefix(filename, PREFIX_TEXTURE));
	
	int image_size[2];
	image_size[0] = ilGetInteger(IL_IMAGE_WIDTH);
	image_size[1] = ilGetInteger(IL_IMAGE_HEIGHT);
	
	GLfloat *image_data = malloc(sizeof(GLfloat) * image_size[0] * image_size[1] * 3);
	ilCopyPixels(0, 0, 0, image_size[0], image_size[1], 1, IL_RGB, IL_FLOAT, image_data);
	
	GLuint texture = create_texture(image_size[0], image_size[1], GL_RGB, image_data);
	
	ilDeleteImages(1, &image);
	free(image_data);
	
	return texture;
}

void drawer_use_texture(Texture texture, unsigned int texture_unit, char *uniform_name)
{
	glActiveTexture(GL_TEXTURE0+texture_unit);
	glBindTexture(GL_TEXTURE_2D, texture);
	GLint location;
	if(uniform_exists(uniform_name, &location)) glUniform1i(location, texture_unit);
}

void drawer_use_rendertarget_texture(Rendertarget target, unsigned int texture_unit, char *uniform_name)
{
	GLuint texture;
	GLint location;
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, target);
	glGetFramebufferAttachmentParameteriv(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &texture);
	
	glActiveTexture(GL_TEXTURE0+texture_unit);
	glBindTexture(GL_TEXTURE_RECTANGLE, texture);
	if(uniform_exists(uniform_name, &location)) glUniform1i(location, texture_unit);
}

Rendertarget drawer_create_rendertarget()
{
	GLuint target, image, depth;
	
	glGenFramebuffers(1, &target);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target);
	
	int screen_size[2];
	window_get_size(screen_size);
	
	glGenTextures(1, &image);
	glBindTexture(GL_TEXTURE_RECTANGLE, image);
	GLfloat *img_buffer = malloc(sizeof(GLfloat)*screen_size[0]*screen_size[1]*3);
	glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, screen_size[0], screen_size[1], 0, GL_RGB, GL_FLOAT, img_buffer);
	free(img_buffer);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, image, 0);
	
	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_size[0], screen_size[1]);
	glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
	
	return target;
}

void drawer_use_rendertarget(Rendertarget target)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target);
}

void drawer_depth_mask(unsigned char mask)
{
	glDepthMask(mask);
}

void drawer_draw_mesh(Mesh *mesh)
{
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
	
	GLfloat *position_pointer, *normal_pointer, *texcoord_pointer;
	GLuint *element_pointer;
	
	if(mesh->vbo)
	{
		MeshVBO *vbo = mesh->vbo;
		glBindBuffer(GL_ARRAY_BUFFER, vbo->vertex_buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->index_buffer);
		vbo_bound = 1;
		position_pointer = ((GLfloat*)NULL)+position_offset;
		normal_pointer = ((GLfloat*)NULL)+normal_offset;
		texcoord_pointer = ((GLfloat*)NULL)+texcoord_offset;
		element_pointer = ((GLuint*)NULL);
	}
	else
	{
		if(vbo_bound)
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			vbo_bound = 0;
		}
		MeshData *data = mesh->data;
		position_pointer = data->vertices+position_offset;
		normal_pointer = data->vertices+normal_offset;
		texcoord_pointer = data->vertices+texcoord_offset;
		element_pointer = data->indices;
	}
	
	GLint location;
	if(mesh->vertex_format & VERTEX_POSITION)
	{
		location = glGetAttribLocation(current_program, "in_position");
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, stride, position_pointer);
		glEnableVertexAttribArray(location);
	}
	
	if(mesh->vertex_format & VERTEX_NORMAL)
	{
		location = glGetAttribLocation(current_program, "in_normal");
		glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, stride, normal_pointer);
		glEnableVertexAttribArray(location);
	}
	
	if(mesh->vertex_format & VERTEX_TEXCOORD)
	{
		location = glGetAttribLocation(current_program, "in_texcoord");
		glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, stride, texcoord_pointer);
		glEnableVertexAttribArray(location);
	}
	
	glDrawElements(GL_TRIANGLES, mesh->indices_count, GL_UNSIGNED_INT, element_pointer);
}

void drawer_postprocess_pass_add(char *filename, int toggle_key)
{
	struct PostProcessPass *pass = &pp_passes[pp_passes_count++];
	pass->key = toggle_key;
	pass->enabled = 1;
	pass->shader = create_shader(GL_FRAGMENT_SHADER, filename);
	pass->program = create_program(pp_vertex_shader, pass->shader);
	
	drawer_use_program(pass->program);
}

void drawer_do_postprocess()
{
	GLuint read = pp_draw_targets[0], draw = pp_draw_targets[1], window = 0;
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
	
	glActiveTexture(GL_TEXTURE0);
	
	for(pass=0; pass<enabled_passes_count; pass++)
	{
		if(pass != 0) //do not swap on first pass
		{
			Rendertarget temp;
			temp = draw;
			draw = read;
			read = temp;
		}
		if(pass == enabled_passes_count-1) draw = window;
		
		drawer_use_program(enabled_passes[pass]);
		
		drawer_use_rendertarget(draw);
		drawer_use_rendertarget_texture(read, 0, "Image");
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawer_draw_mesh(screen_square_mesh);
	}
}

void drawer_begin_scene(float time)
{
	global_time += time;
	drawer_use_rendertarget(pp_draw_targets[0]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void drawer_end_scene()
{
	window_swap_buffers();
}

void drawer_3d_reset()
{
	if(render_3d_mode == RENDER_3D_ANAGLYPH) glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	if(render_3d_mode == RENDER_3D_SIDEBYSIDE)
	{
		int size[2];
		window_get_size(size);
		glViewport(0, 0, size[0], size[1]);
	}
}

void drawer_3d_left()
{
	if(render_3d_mode == RENDER_3D_ANAGLYPH)
	{
		glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_FALSE);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	if(render_3d_mode == RENDER_3D_SIDEBYSIDE)
	{
		int size[2];
		window_get_size(size);
		glViewport(0, 0, size[0]/2, size[1]);
	}
}

void drawer_3d_right()
{
	if(render_3d_mode == RENDER_3D_ANAGLYPH)
	{
		glColorMask(GL_FALSE, GL_FALSE, GL_TRUE, GL_FALSE);
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	if(render_3d_mode == RENDER_3D_SIDEBYSIDE)
	{
		int size[2];
		window_get_size(size);
		glViewport(size[0]/2, 0, size[0]/2, size[1]);
	}
}

enum Render3DMode drawer_get_3d_mode()
{
	return render_3d_mode;
}

void drawer_create_mesh_vbo(Mesh *mesh)
{
	MeshVBO *vbo = malloc(sizeof(MeshVBO));
	
	glGenBuffers(1, &vbo->vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vbo->vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * mesh_get_vertex_size(mesh->vertex_format) * mesh->vertices_count, mesh->data->vertices, GL_STATIC_DRAW);
	
	glGenBuffers(1, &vbo->index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh->indices_count, mesh->data->indices, GL_STATIC_DRAW);
	
	mesh->vbo = vbo;
	vbo_bound = 1;
}

void drawer_free_mesh_vbo(MeshVBO *vbo)
{
	glDeleteBuffers(1, &vbo->vertex_buffer);
	glDeleteBuffers(1, &vbo->index_buffer);
	free(vbo);
}

static GLuint create_shader(GLenum type, char *filename)
{
	GLuint shader = glCreateShader(type);
	GLchar *shader_source = file_text(file_prefix(filename, PREFIX_SHADER));
	glShaderSource(shader, 1, (const GLchar**)&shader_source, NULL);
	free(shader_source);
	
	glCompileShader(shader);
	
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(compiled == GL_FALSE)
	{
		printf("Failed to compile %s:\n", filename);
		GLint log_length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
		GLchar *log = malloc(log_length+1);
		glGetShaderInfoLog(shader, log_length, NULL, log);
		printf("%s\n", log);
		free(log);
	}
	return shader;
}

static GLuint create_program(GLuint vertex_shader, GLuint fragment_shader)
{
	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
	glLinkProgram(program);
	
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if(linked == GL_FALSE)
	{
		printf("Failed to link program:\n");
		GLint log_length;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
		GLchar *log = malloc(log_length+1);
		glGetProgramInfoLog(program, log_length, NULL, log);
		printf("%s\n", log);
		free(log);
	}
	return program;
}

static int uniform_exists(char *name, GLint *location)
{
	*location = glGetUniformLocation(current_program, name);
	return *location == -1 ? 0 : 1;
}

static GLuint create_texture(GLsizei width, GLsizei height, GLenum format, GLfloat *data)
{
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_FLOAT, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	return texture;
}

static GLuint generate_noise_texture()
{
	const int size = 256;
	float *texture_data = malloc(sizeof(float) * size * size * 4);
	noise_generate_texture2d_channel(4, size, size, 4, texture_data);
	noise_generate_texture2d_channel(8, size, size, 4, texture_data+1);
	noise_generate_texture2d_channel(16, size, size, 4, texture_data+2);
	noise_generate_texture2d_channel(32, size, size, 4, texture_data+3);
	
	GLuint texture = create_texture(size, size, GL_RGBA, texture_data);
	
	free(texture_data);
	
	return texture;
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
	}
	glUniform2fv(location, 11, (const GLfloat*)values);
}

static void update_uniforms()
{
	if(current_program == 0) return;
	
	GLint location;
	if(uniform_exists("MVMatrix", &location)) glUniformMatrix4fv(location, 1, GL_FALSE, mat_modelview);
	if(uniform_exists("MVPMatrix", &location))
	{
		float mvp[16];
		copy_m4_m4(mvp, mat_projection);
		mul_m4_m4(mvp, mat_modelview);
		glUniformMatrix4fv(location, 1, GL_FALSE, mvp);
	}
	if(uniform_exists("GaussValues", &location)) calc_gauss_values(location);
	if(uniform_exists("ScreenSize", &location))
	{
		int size[2];
		window_get_size(size);
		glUniform2iv(location, 1, size);
	}
	if(uniform_exists("Noise", &location)) glUniform1i(location, NOISE_TEXTURE_LAYER);
	if(uniform_exists("Time", &location)) glUniform1f(location, global_time);
}

static void screenshot()
{
	int size[2];
	window_get_size(size);
	const unsigned int w = size[0], h = size[1];
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

static void print_glinfo()
{
	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	if(GLEW_ARB_vertex_buffer_object) printf("VBOs are supported. Yeah!\n");
	else printf("VBOs are not supported.\n");
}

static void write_glinfo()
{
	FILE *file = fopen("glinfo.txt", "w");
	fprintf(file, "OpenGL Info\n");
	fprintf(file, "Version: %s\n", glGetString(GL_VERSION));
	fprintf(file, "GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	fprintf(file, "Renderer: %s\n", glGetString(GL_RENDERER));
	fprintf(file, "Vendor: %s\n", glGetString(GL_VENDOR));
	fprintf(file, "Extensions: %s\n", glGetString(GL_EXTENSIONS));
	fclose(file);
}

static void handle_keypress(char key)
{
	if(key == 'r') render_3d_mode = (render_3d_mode+1) % RENDER_3D_MODES_COUNT;
	else if(key == 'i') write_glinfo();
	else if(key == 'p') screenshot();
	int i;
	for(i=0; i<pp_passes_count; i++)
	{
		struct PostProcessPass *p = &pp_passes[i];
		if(p->key == key) p->enabled ^= 1;
	}
}
