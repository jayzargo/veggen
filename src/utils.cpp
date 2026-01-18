#include "pch.h"
#include "utils.h"

long long GetFileSize64(const char* file_name)
{
	FILE* file = fopen(file_name, "rb");

	if (file != NULL)
	{
		_fseeki64(file, 0, SEEK_END);
		long long file_size = _ftelli64(file);
		_fseeki64(file, 0, SEEK_SET);
		fclose(file);
		file = NULL;

		return file_size;
	}

	return 0;
}