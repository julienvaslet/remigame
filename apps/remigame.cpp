#include <SDL2/SDL.h>
#include <iostream>

using namespace std;

int main( int argc, char ** argv )
{
	SDL_Window * window = NULL;
	SDL_Renderer * renderer = NULL;
	SDL_Init( SDL_INIT_VIDEO );

	window = SDL_CreateWindow(
		"Remigame",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,
		600,
		0
	);
	
	// do no specify width & height and put in fullscreen
	// then => SDL_RenderSetLogicalSize( renderer, 800, 600 );

	if( window == NULL )
	{
		cout << "Unable to create window: " << SDL_GetError() << endl;
		return 1;
	}
	
	renderer = SDL_CreateRenderer( window, -1, 0 );
	
	if( renderer == NULL )
	{
		cout << "Unable to create the renderer: " << SDL_GetError() << endl;
		return 1;
	}
	
	SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
	SDL_RenderClear( renderer );
	SDL_RenderPresent( renderer );
	
	SDL_Surface * image = SDL_LoadBMP( "texture.bmp" );
	
	//SDL_Texture * texture = SDL_CreateTexure( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 512, 512 );
	SDL_Texture * texture = NULL;
	
	if( image != NULL )
	{
		texture = SDL_CreateTextureFromSurface( renderer, image );
		SDL_FreeSurface( image );
	}
	else
	{
		cout << "Unable to load image: " << SDL_GetError() << endl;
		return 1;
	}

	
	SDL_Rect rect;
	rect.x = 800 / 2 - 512 / 2;
	rect.y = 600 / 2 - 512 / 2;
	rect.w = 512;
	rect.h = 512;


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
			SDL_RenderClear( renderer );
			
			SDL_RenderCopy( renderer, texture, NULL, &rect );
			
			SDL_RenderPresent( renderer );
			lastDrawTicks = ticks;
		}
	}
	
	SDL_DestroyTexture( texture );
	
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( window );
	SDL_Quit();
	return 0;
}
