#ifndef	_GRAPHICS_FONT_H
#define	_GRAPHICS_FONT_H	1

#include <graphics/Sprite.h>

#include <SDL2/SDL.h>
#include <string>

using namespace std;

namespace graphics
{
	class Font
	{
		protected:
			static Font * fonts;
			Sprite * image;
			map<char,Sprite::Frame> characters;
			
			Font();
			~Font();
            
		public:
			static bool load( const string& filename );
			static bool load( const char * filename );
			static Font * get( const string& name );
			static Font * get( const char * name );
			static void destroy( const string& name );
			static void destroy( const char * name );
			
			void render( int x, int y, const string& text );
			void render( int x, int y, const char * text );
	};
}

#endif
