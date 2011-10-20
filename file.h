#ifndef H_FILE
#define H_FILE

enum FilePrefix
{
	PREFIX_TEXTURE,
	PREFIX_SHADER
};

char* file_text(char *filename);
char* file_prefix(char *filename, enum FilePrefix prefix);

#endif //H_FILE
