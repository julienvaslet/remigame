#include <SDL2/SDL.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glu.h>
//#include <GL/glx.h>
//#include <GL/glext.h>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#endif

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
	
	// OpenGL initialization
	glEnable( GL_DEPTH_TEST );
	glViewport( 0, 0, 640, 480 );
	
	GLuint vertexBuffer = 0;
	float vertex[] =
	{
		-2.5f, -2.5f, 0.0f,
		-2.5f, 2.5f, 0.0f,
		2.5f, 2.5f, 0.0f,
		2.5f, -2.5f, 0.0f
	};
	
	GLuint colorsBuffer = 0;
	float colors[] =
	{
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f
	};
	
	unsigned int squareIndices[] = { 0, 1, 2, 2, 3, 0 };
	
	glGenBuffers( 1, &vertexBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(float) * 12, &vertex, GL_STATIC_DRAW );
	
	glGenBuffers( 1, &colorsBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, colorsBuffer );
	glBufferData( GL_ARRAY_BUFFER, sizeof(float) * 12, &colors, GL_STATIC_DRAW );
	
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -5.0f, 5.0f, -5.0f, 5.0f, -1.0f, 1.0f );

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
			
			glMatrixMode( GL_MODELVIEW );
			glLoadIdentity();
			
			glBindBuffer( GL_ARRAY_BUFFER, colorsBuffer );
			glColorPointer( 3, GL_FLOAT, 0, BUFFER_OFFSET(0) );

			glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer );
			glVertexPointer( 3, GL_FLOAT, 0, BUFFER_OFFSET(0) );
			
			glEnableClientState( GL_COLOR_ARRAY );
			glEnableClientState( GL_VERTEX_ARRAY );
			glDrawElements( GL_TRIANGLES, 6, GL_UNSIGNED_INT, &squareIndices );
			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_COLOR_ARRAY );
			
			lastDrawTicks = ticks;
			SDL_GL_SwapWindow( window );
		}
	}
	
	glDeleteBuffers( 1, &vertexBuffer );
	glDeleteBuffers( 1, &colorsBuffer );

	SDL_GL_DeleteContext( context );
	SDL_DestroyWindow( window );
	SDL_Quit();
	return 0;
}
