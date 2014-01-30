#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

#include <graphics/Screen.h>
#include <graphics/Font.h>
#include <controller/Controller.h>
#include <data/node/Node.h>

using namespace graphics;
using namespace std;
using namespace controller;
using namespace data;

int main( int argc, char ** argv )
{
	if( !Screen::initialize( "Controller Mapping" ) )
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
	
	map< pair<Mapping::Button, short int>, string> internalValues;
	intervalValues[ pair(Mapping::BTNUP, Mapping::STATE_PUSHED) ] = "BTNUP, Triangle for PS3, Y for XBOX";
	intervalValues[ pair(Mapping::BTNRIGHT, Mapping::STATE_PUSHED) ] = "BTNRIGHT, Circle for PS3, B for XBOX";
	intervalValues[ pair(Mapping::BTNDOWN, Mapping::STATE_PUSHED) ] = "BTNDOWN, Cross for PS3, A for XBOX";
	intervalValues[ pair(Mapping::BTNLEFT, Mapping::STATE_PUSHED) ] = "BTNLEFT, Square for PS3, B for XBOX";
	intervalValues[ pair(Mapping::LT1, Mapping::STATE_PUSHED) ] = "LT1, Left top trigger for PS3 and XBOX";
	intervalValues[ pair(Mapping::LT2, Mapping::STATE_PUSHED) ] = "LT2, Left bottom trigger PS3 and XBOX";
	intervalValues[ pair(Mapping::LT3, Mapping::STATE_PUSHED) ] = "LT3, Left stick PS3 and XBOX";
	intervalValues[ pair(Mapping::RT1, Mapping::STATE_PUSHED) ] = "RT1, Right top trigger for PS3 and XBOX";
	intervalValues[ pair(Mapping::RT2, Mapping::STATE_PUSHED) ] = "RT2, Right bottom trigger for PS3 and XBOX";
	intervalValues[ pair(Mapping::RT3, Mapping::STATE_PUSHED) ] = "RT3, Right stick for PS3 and XBOX";
	intervalValues[ pair(Mapping::SELECT, Mapping::STATE_PUSHED) ] = "SELECT, Select for PS3 and XBOX";
	intervalValues[ pair(Mapping::START, Mapping::STATE_PUSHED) ] = "START, Start for PS3 and XBOX";
	intervalValues[ pair(Mapping::AXH, Mapping::STATE_PUSHED) ] = "AXH, Move in the left direction for PS3 and XBOX";
	intervalValues[ pair(Mapping::AXH, Mapping::STATE_RPUSHED) ] = "AXH, Move in the right direction for PS3 and XBOX";
	intervalValues[ pair(Mapping::AXV, Mapping::STATE_PUSHED) ] = "AXV, Move in the up direction for PS3 and XBOX";
	intervalValues[ pair(Mapping::AXV, Mapping::STATE_RPUSHED) ] = "AXV, Move in the bottom direction for PS3 and XBOX";
	
	vector< pair<Mapping::Button, short int> > internalValuesOrder;
	internalValuesOrder.push_back( pair(Mapping::AXH, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::AXH, Mapping::STATE_RPUSHED) );
	internalValuesOrder.push_back( pair(Mapping::AXV, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::AXV, Mapping::STATE_RPUSHED) );
	internalValuesOrder.push_back( pair(Mapping::BTNUP, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::BTNRIGHT, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::BTNDOWN, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::BTNLEFT, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::LT1, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::RT1, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::LT2, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::RT2, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::LT3, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::RT3, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::SELECT, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( pair(Mapping::START, Mapping::STATE_PUSHED) );
	
	// Current controller mapping
	short int controllerId = -1;
	map<int, pair<Mapping::Button, short int> > buttons;
	map<int, Mapping::Button> axes;

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
			
			if( controllerId == -1 )
			{
				string message = "Please push any button of the controller to initialize";
				int messageX = (800 - Font::get( "font0" )->renderWidth( string )) / 2;
				Font::get( "font0" )->render( messageX, 10, string );
			}
			else
			{
				
			}
			
			Screen::get()->render();
			
			lastDrawTicks = ticks;
		}
	}

	Controller::destroy();		
	Font::destroy( "font0" );
	Screen::destroy();
	
	return 0;
}
