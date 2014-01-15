#include <SDL2/SDL.h>
#include <iostream>
#include <string>
#include <vector>

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
	
	unsigned int currentAnimation = 0;
	vector<string> animations;
	animations.push_back( "idle" );
	animations.push_back( "walk" );
	animations.push_back( "run" );
	animations.push_back( "jump" );

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
						currentAnimation = 0;
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
					switch( lastEvent.key.keysym.sym )
					{
						case SDLK_ESCAPE:
						{
							running = false;
							break;
						}
						
						case SDLK_KP_PLUS:
						case SDLK_PLUS:
						{
							// Increase animation speed
							break;
						}
						
						case SDLK_KP_MINUS:
						case SDLK_MINUS:
						{
							// Decrease animation speed
							break;
						}
						
						case SDLK_KP_TAB:
						case SDLK_TAB:
						{
							// Switch animation
							currentAnimation = (currentAnimation + 1) % animations.size();
							
							if( object != NULL )
								object->setAnimation( animations.at( currentAnimation ) );
							
							break;
						}
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
