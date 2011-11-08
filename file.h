#ifndef H_FILE
#define H_FILE

enum FilePrefix
{
	PREFIX_TEXTURE,
	PREFIX_SHADER
};

char* file_text(char *filename);
char* file_prefix(char *filename, enum FilePrefix prefix);
void file_set_rootdir(char *dir);
char* file_rootdir(char *filename);

#endif //H_FILE
