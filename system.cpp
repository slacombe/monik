#include <stdio.h>
#include <stdlib.h>

#include "chess.h"

void MakePath(char* path, const char* filename)
{
	strcpy(path, MonikHome);
	int l = strlen(path);
	if (path[l-1] != '/') {
		strcat(path, "/");
	}
	strcpy(path, filename);
}
