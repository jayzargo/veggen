#pragma once
#include <cstdio>
// This file is inspired from PGII 

/*! \fn long long GetFileSize64( const char * file_name )
\brief Returns the file size in bytes.
\param file_name Full path to the file
*/
long long GetFileSize64(const char* file_name);

#define SAFE_DELETE_ARRAY( p ) { \
	if ( p != NULL ) \
	{ \
		delete [] p; \
		p = NULL; \
	} \
}
