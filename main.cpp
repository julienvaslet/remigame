#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glx.h>
//#include <GL/glext.h>

#include <iostream>

using namespace std;

int main( int argc, char ** argv )
{
	SDL_Window * window = NULL;
	SDL_Init( SDL_INIT_VIDEO );

	window = SDL_CreateWindow(
		"Remigame",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640,
		480,
		SDL_WINDOW_OPENGL
	);

	if( window == NULL )
	{
		cout << "Unable to create window: " << SDL_GetError() << endl;
		return 1;
	}

	SDL_GLContext context = SDL_GL_CreateContext( window );

	bool running = true;
	SDL_Event lastEvent;
	unsigned int lastDrawTicks = 0;

	while( running )
	{
		while( SDL_PollEvent( &lastEvent ) )
		{
			switch( lastEvent.type )
			{
				case SDL_QUIT:
				{
					running = false;
					break;
				}
			
				case SDL_KEYDOWN:
				{
		            if( lastEvent.key.keysym.sym == SDLK_ESCAPE )
		                running = false;

					break;
				}
			}
		}

		unsigned int ticks = SDL_GetTicks();
		
		if( ticks - lastDrawTicks > 15 )
		{
			glClearColor( 0, 0, 0, 1 );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
			
			lastDrawTicks = ticks;
			SDL_GL_SwapWindow( window );
		}
	}

	SDL_GL_DeleteContext( context );
	SDL_DestroyWindow( window );
	SDL_Quit();
	return 0;
}
