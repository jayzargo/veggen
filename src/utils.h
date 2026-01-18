#pragma once
#include <cstdio>
// This file is inspired from PGII 

/*! \fn long long GetFileSize64( const char * file_name )
\brief Vrátí velikost souboru v bytech.
\param file_name Úplná cesta k souboru
*/
long long GetFileSize64(const char* file_name);

#define SAFE_DELETE_ARRAY( p ) { \
	if ( p != NULL ) \
	{ \
		delete [] p; \
		p = NULL; \
	} \
}
