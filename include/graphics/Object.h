#ifndef _GRAPHICS_OBJECT_H
#define	_GRAPHICS_OBJECT_H	1

#include <SDL2/SDL.h>
#include <string>
#include <graphics/Sprite.h>

using namespace std;

namespace graphics
{
	class Object
	{
		protected:
			Sprite * sprite;
	
		public:
			Object();
			Object( const char * filename );
			Object( const string& filename );
			~Object();
		
			bool load( const char * filename );
	};
}

#endif

