#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
#include <vector>

#include <graphics/Screen.h>
#include <graphics/Object.h>
#include <graphics/Font.h>
#include <controller/Controller.h>
#include <game/ObjectEventHandler.h>

using namespace graphics;
using namespace std;
using namespace controller;

int main( int argc, char ** argv )
{
	vector<Object *> objects;
	vector<Player *> players;
	
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
	background->getOrigin().move( 0, 0 );
	
	if( !background->isLoaded() )
	{
		cout << "Unable to load background object. Exiting." << endl;
		Font::destroy( "font0" );
		Screen::destroy();
		return 1;
	}
	
	// Controller & character assignment
	Controller::scan();
	Controller * controller = Controller::getFreeController();
	
	while( controller != NULL )
	{
		stringstream playername;
		playername << "Player#" << controller->getId();
		
		Player * player = new Player( playername.str().c_str() );
		player->setController( controller );
		
		players.push_back( player );
		
		Object * character = new Object( "data/object.xml" );
		character->setZoom( 35 );
		character->getOrigin().move( 100, 600 );
		objects.push_back( character );
		
		player->setEventHandler( new ObjectEventHandler( character ) );
		
		controller = Controller::getFreeController();
	}
	

	bool running = true;
	SDL_Event lastEvent;
	unsigned int lastDrawTicks = 0;
	unsigned int lastTickEvent = 0;

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
				
				case SDL_JOYDEVICEADDED:
				{
					Controller::scan();
					break;
				}
				
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:
				case SDL_JOYAXISMOTION:
				case SDL_JOYDEVICEREMOVED:
				{
					Controller::handleEvent( &lastEvent );
					break;
				}
			}
		}

		unsigned int ticks = SDL_GetTicks();
		
		if( ticks - lastTickEvent > 15 )
		{
			lastTickEvent = ticks;
			Controller::tickEvent( lastTickEvent );
		}
		
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
			
			// Show objects
			for( vector<Object *>::iterator itObject = objects.begin() ; itObject != objects.end() ; itObject++ )
				(*itObject)->render( ticks );
			
			// Show FPS
			int fpsX = (800 - Font::get( "font0" )->renderWidth( framesPerSecondText.str() )) / 2;
			Font::get( "font0" )->render( fpsX, 10, framesPerSecondText.str() );

			Screen::get()->render();
			
			lastDrawTicks = ticks;
		}
	}
	
	for( vector<Object *>::iterator itObject = objects.begin() ; itObject != objects.end() ; itObject++ )
		delete *itObject;
		
	objects.clear();
	
	// WARN: Could have some segfault when live disconnecting controller.
	for( vector<Player *>::iterator itPlayer = players.begin() ; itPlayer != players.end() ; itPlayer++ )
		delete *itPlayer;

	players.clear();

	Controller::destroy();
	Font::destroy( "font0" );
	Screen::destroy();
	
	return 0;
}
