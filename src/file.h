/* Copyright (C) 2016 ultitech - All Rights Reserved
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#ifndef H_FILE
#define H_FILE

enum FileResource
{
	RESOURCE_TEXTURE,
	RESOURCE_SHADER
};

char* file_text(char *filename);
void file_set_resource_dir(char *dir);
char* file_resource(char *filename, enum FileResource type);
void file_set_output_dir(char *dir);
char* file_output(char *filename);

#endif //H_FILE
