#include <stdio.h>
#include "drawable.h"
#include "ltga.h"

TexMap::TexMap( const string &filename )
{
	bool success = false;

	string::size_type ext_indx = filename.rfind( "." );

	if ( ext_indx != string::npos )
	{
		string ext = filename.substr( ext_indx, string::npos );

		if ( ext == string( ".tga" ) )
		{
			success = loadTGA( filename.c_str() );
		}
		else fprintf( stderr, "Unsupported image type: '%s'.", ext.c_str() );
	}
	else fprintf( stderr, "Filename '%s' has no extension.\n", filename.c_str() );

	if ( !success )
		throw TexException( "Couldn't load texture." );
}

bool TexMap::loadTGA( const char *filename )
{
	LTGA l;
	bool success = false;

	if( l.LoadFromFile( filename ) )
	{
		glGenTextures( 1, &texHandle );
		bind();
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		int format = ( l.GetAlphaDepth() == 8 ) ? GL_RGBA : GL_RGB;

		glTexImage2D(	GL_TEXTURE_2D,
		      			0,
		      			format,
		      			l.GetImageWidth(),
		      			l.GetImageHeight(),
		      			0,
		      			format,
		      			GL_UNSIGNED_BYTE,
		      			l.GetPixels() );
		success = true;
	} 
	else printf( "Failed to load texture file: %s\n", filename );

	return success;
} // end loadTextures()

void TexMap::bind()
{
	glBindTexture( GL_TEXTURE_2D, texHandle );
}
