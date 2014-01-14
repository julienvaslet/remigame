#include <SDL2/SDL.h>
#include <iostream>

#include <graphics/Screen.h>
#include <graphics/Object.h>
#include <graphics/Sprite.h>

using namespace graphics;
using namespace std;

int main( int argc, char ** argv )
{
	if( !Screen::initialize() )
	{
		cout << "Unable to initialize screen. Exiting." << endl;
		return 1;
	}
	
	Object * object = NULL;

	bool running = true;
	SDL_Event lastEvent;
	unsigned int lastDrawTicks = 0;
	
	SDL_EventState( SDL_DROPFILE, SDL_ENABLE );

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
				
				case SDL_DROPFILE:
				{
					
					Object * nObject = new Object( "object.xml" );
					
					if( nObject->isLoaded() )
					{
						nObject->move( (800 - 512) / 2, (600 - 512) / 2 );
						nObject->resize( 512, 512 );
						
						if( object != NULL )
							delete object;
							
						object = nObject;
					}
					else
					{
						delete nObject;
						cout << "Unable to load the object. Keeping old one." << endl;
					}
					
					SDL_free( lastEvent.drop.file );
					break;
				}
			
				case SDL_KEYDOWN:
				{
					if( lastEvent.key.keysym.sym == SDLK_ESCAPE )
						running = false;
						
					else if( lastEvent.key.keysym.sym == SDLK_KP_PLUS || lastEvent.key.keysym.sym ==  SDLK_PLUS )
					{
						/*
						if( speed > 50 )
							speed -= 50;*/
					}
					
					else if( lastEvent.key.keysym.sym == SDLK_KP_MINUS || lastEvent.key.keysym.sym == SDLK_MINUS )
					{
						/*
						speed += 50;*/
					}

					break;
				}
			}
		}

		unsigned int ticks = SDL_GetTicks();
		
		if( ticks - lastDrawTicks > 15 )
		{
			Screen::get()->clear();
			
			if( object != NULL )
				object->render( ticks );

			Screen::get()->render();
			
			lastDrawTicks = ticks;
		}
	}
	
	if( object != NULL )
		delete object;

	Screen::destroy();
	
	return 0;
}
