#ifndef __DRAWABLE_H__
#define __DRAWABLE_H__

#include <GL/glut.h>
#include <string>
using std::string;

class DrawableObj 
{
	public:
		virtual void draw() = 0;
};

class TexMap
{
	public:
		class TexException 
		{
			private:
				string msg;
			public:
				TexException( const char *m ) :
					msg( m )
				{
				}
				const char *what() { return msg.c_str(); }
		};

		TexMap( const string &filename ); // throw( TexException )
		bool loadTGA( const char *filename );
		void bind();

		GLuint texHandle;
};

#endif // __DRAWABLE_H__
