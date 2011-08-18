#include "texture.h"

#include <IL/il.h>
#include <IL/ilu.h>

#include <stdlib.h>

void texture_init()
{
	ilInit();
	iluInit();
}

GLuint texture_create(char *filename)
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
	
	return texture;
}
