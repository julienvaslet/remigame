#include <graphics/Object.h>

#ifdef DEBUG0
#include <iostream>
#endif

namespace graphics
{
	Object::Object() : sprite(NULL)
	{
	}
	
	Object::Object( const char * filename ) : sprite(NULL)
	{
		this->load( filename );
	}
	
	Object::Object( const string& filename ) : sprite(NULL)
	{
		this->load( filename.c_str() );
	}
	
	void Object::load( const char * filename )
	{
	}
}

