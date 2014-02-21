#ifndef _GRAPHICS_SCREEN_H
#define	_GRAPHICS_SCREEN_H	1

#include <SDL2/SDL.h>
#include <graphics/Color.h>

namespace graphics
{
	class Screen
	{
		protected:
			static Screen * instance;
		
			SDL_Window * window;
			SDL_Renderer * renderer;
		
			Screen();
			~Screen();
		
		public:
			static bool initialize( const char * title, int width = 800, int height = 600 );
			static Screen * get();
			static void destroy();
		
			SDL_Renderer * getRenderer();
			void resetRenderColor();
			void setRenderColor( Color& color );
			void render();
			void clear();	
	};
}

#endif

