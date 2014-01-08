#ifndef _GRAPHICS_SCREEN_H
#define	_GRAPHICS_SCREEN_H	1

#include <SDL2/SDL.h>

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
			static bool initialize();
			static Screen * get();
			static void destroy();
		
			SDL_Renderer * getRenderer();
			void render();
			void clear();	
	};
}

#endif

