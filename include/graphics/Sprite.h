#ifndef _GRAPHICS_SPRITE_H
#define	_GRAPHICS_SPRITE_H	1

#include <SDL2/SDL.h>
#include <string>
#include <graphics/Screen.h>

using namespace std;

namespace graphics
{
	class Sprite
	{
		public:
			struct Frame
			{
				int x;
				int y;
				int width;
				int height;
			};
		
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
		
			int getViewX();
			int getViewY();
			int getViewWidth();
			int getViewHeight();
		
			int getWidth();
			int getHeight();
			
			SDL_Texture * getTexture();
	};
}

#endif

