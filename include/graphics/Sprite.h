#ifndef _GRAPHICS_SPRITE_H
#define	_GRAPHICS_SPRITE_H	1

#include <SDL2/SDL.h>
#include <string>
#include <graphics/Screen.h>
#include <graphics/Frame.h>

using namespace std;

namespace graphics
{
	class Sprite
	{
		protected:
			SDL_Texture * texture;
			int width;
			int height;
			Frame view;
	
		public:
			Sprite();
			Sprite( const char * filename );
			Sprite( const string& filename );
			~Sprite();
		
			bool load( const char * filename );
			bool isLoaded();
		
			void setView( int x, int y, int width, int height );
		
			Frame& getView();
		
			int getWidth();
			int getHeight();
			
			SDL_Texture * getTexture();
	};
}

#endif

