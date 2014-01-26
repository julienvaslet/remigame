#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <graphics/Screen.h>
#include <graphics/Font.h>
#include <controller/Controller.h>

using namespace graphics;
using namespace std;
using namespace controller;

int main( int argc, char ** argv )
{
	if( !Screen::initialize( "Controller Manager" ) )
	{
		cout << "Unable to initialize screen. Exiting." << endl;
		return 1;
	}
	
	if( !Font::load( "data/font.xml" ) )
	{
		cout << "Unable to load main font. Exiting." << endl;
		Screen::destroy();
		return 1;
	}
	
	Controller::initialize();
	
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
				
				case SDL_JOYBUTTONUP:
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYAXISMOTION:
				{
					Controller::handleEvent( &lastEvent );
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
					}

					break;
				}
			}
		}

		unsigned int ticks = SDL_GetTicks();
		
		if( ticks - lastDrawTicks > 15 )
		{
			Screen::get()->clear();
			Screen::get()->render();
			
			lastDrawTicks = ticks;
		}
	}

	Controller::destroy();		
	Font::destroy( "font0" );
	Screen::destroy();
	
	return 0;
}
