#include "math.h"

#include <string.h>

void vec_set(float dst[3], float src[3])
{
	memcpy(dst, src, sizeof(float)*3);
}
