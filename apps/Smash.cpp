#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>

#include <graphics/Screen.h>
#include <graphics/Object.h>
#include <graphics/Font.h>
#include <controller/Controller.h>

using namespace graphics;
using namespace std;
using namespace controller;

int main( int argc, char ** argv )
{
	if( !Screen::initialize( "Smash", 800, 600 ) )
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
	
	// Background initialization
	Object * background = new Object( "data/background.xml" );
	background->move( 0, 0 );
	background->resize( 800, 600 );
	
	if( !background->isLoaded() )
	{
		cout << "Unable to load background object. Exiting." << endl;
		Font::destroy( "font0" );
		Screen::destroy();
		return 1;
	}
	
	// Controller & character assignment
	Controller::initialize();
	
	

	bool running = true;
	SDL_Event lastEvent;
	unsigned int lastDrawTicks = 0;

	stringstream framesPerSecondText;
	int framesPerSecond = 0;
	unsigned int lastSecond = 0;

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
			if( ticks - lastSecond >= 1000 )
			{
				lastSecond = ticks;
				framesPerSecondText.str("");
				framesPerSecondText << framesPerSecond << " FPS";
				framesPerSecond = 0;
			}
			else
				framesPerSecond++;
				
			Screen::get()->clear();
			
			// Show background
			background->render( ticks );
			
			// Show FPS
			int fpsX = (800 - Font::get( "font0" )->renderWidth( framesPerSecondText.str() )) / 2;
			Font::get( "font0" )->render( fpsX, 10, framesPerSecondText.str() );

			Screen::get()->render();
			
			lastDrawTicks = ticks;
		}
	}

	Controller::destroy();
	Font::destroy( "font0" );
	Screen::destroy();
	
	return 0;
}
