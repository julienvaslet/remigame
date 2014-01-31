#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
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
	
	map<Mapping::Button, string> internalActions;
	internalActions[Mapping::BTNUP] = "BTNUP";
	internalActions[Mapping::BTNRIGHT] = "BTNRIGHT";
	internalActions[Mapping::BTNDOWN] = "BTNDOWN";
	internalActions[Mapping::BTNLEFT] = "BTNLEFT";
	internalActions[Mapping::LT1] = "LT1";
	internalActions[Mapping::LT2] = "LT2";
	internalActions[Mapping::LT3] = "LT3";
	internalActions[Mapping::RT1] = "RT1";
	internalActions[Mapping::RT2] = "RT2";
	internalActions[Mapping::RT3] = "RT3";
	internalActions[Mapping::SELECT] = "SELECT";
	internalActions[Mapping::START] = "START";
	internalActions[Mapping::AXH] = "AXH";
	internalActions[Mapping::AXV] = "AXV";
	
	map< pair<Mapping::Button, short int>, string> internalValues;
	internalValues[ make_pair(Mapping::BTNUP, Mapping::STATE_PUSHED) ] = "BTNUP, Triangle for PS3, Y for XBOX";
	internalValues[ make_pair(Mapping::BTNRIGHT, Mapping::STATE_PUSHED) ] = "BTNRIGHT, Circle for PS3, B for XBOX";
	internalValues[ make_pair(Mapping::BTNDOWN, Mapping::STATE_PUSHED) ] = "BTNDOWN, Cross for PS3, A for XBOX";
	internalValues[ make_pair(Mapping::BTNLEFT, Mapping::STATE_PUSHED) ] = "BTNLEFT, Square for PS3, B for XBOX";
	internalValues[ make_pair(Mapping::LT1, Mapping::STATE_PUSHED) ] = "LT1, Left top trigger for PS3 and XBOX";
	internalValues[ make_pair(Mapping::LT2, Mapping::STATE_PUSHED) ] = "LT2, Left bottom trigger PS3 and XBOX";
	internalValues[ make_pair(Mapping::LT3, Mapping::STATE_PUSHED) ] = "LT3, Left stick PS3 and XBOX";
	internalValues[ make_pair(Mapping::RT1, Mapping::STATE_PUSHED) ] = "RT1, Right top trigger for PS3 and XBOX";
	internalValues[ make_pair(Mapping::RT2, Mapping::STATE_PUSHED) ] = "RT2, Right bottom trigger for PS3 and XBOX";
	internalValues[ make_pair(Mapping::RT3, Mapping::STATE_PUSHED) ] = "RT3, Right stick for PS3 and XBOX";
	internalValues[ make_pair(Mapping::SELECT, Mapping::STATE_PUSHED) ] = "SELECT, Select for PS3 and XBOX";
	internalValues[ make_pair(Mapping::START, Mapping::STATE_PUSHED) ] = "START, Start for PS3 and XBOX";
	internalValues[ make_pair(Mapping::AXH, Mapping::STATE_RPUSHED) ] = "AXH, Move in the left direction";
	internalValues[ make_pair(Mapping::AXH, Mapping::STATE_PUSHED) ] = "AXH, Move in the right direction";
	internalValues[ make_pair(Mapping::AXV, Mapping::STATE_RPUSHED) ] = "AXV, Move in the up direction";
	internalValues[ make_pair(Mapping::AXV, Mapping::STATE_PUSHED) ] = "AXV, Move in the bottom direction";
	
	vector< pair<Mapping::Button, short int> > internalValuesOrder;
	internalValuesOrder.push_back( make_pair(Mapping::AXH, Mapping::STATE_RPUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::AXH, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::AXV, Mapping::STATE_RPUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::AXV, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::BTNUP, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::BTNRIGHT, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::BTNDOWN, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::BTNLEFT, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::LT1, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::RT1, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::LT2, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::RT2, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::LT3, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::RT3, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::SELECT, Mapping::STATE_PUSHED) );
	internalValuesOrder.push_back( make_pair(Mapping::START, Mapping::STATE_PUSHED) );
	
	// Current controller mapping
	short int controllerId = -1;
	unsigned int orderIndex = 0;
	set<int> currentButtons;
	set<int> currentAxes;
	map<int, pair<Mapping::Button,short int> > buttons;
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
					short int eController = ( lastEvent.type == SDL_JOYAXISMOTION ) ? lastEvent.jaxis.which : lastEvent.jbutton.which;
					
					//Controller::handleEvent( &lastEvent );
					if( controllerId == -1 )
					{
						controllerId = eController;
						buttons.clear();
						axes.clear();
					}
					else
					{
						if( lastEvent.type == SDL_JOYAXISMOTION )
						{
							currentAxes.insert( lastEvent.jaxis.axis );
						}
						else if( lastEvent.type == SDL_JOYBUTTONDOWN )
						{
							currentButtons.insert( lastEvent.jbutton.button );
						}
					}
					
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
						
						case SDLK_BACKSPACE:
						{
							currentButtons.clear();
							currentAxes.clear();
							break;
						}
						
						case SDLK_RETURN:
						{
							if( controllerId != -1 )
							{
								for( set<int>::const_iterator itBtn = currentButtons.begin() ; itBtn != currentButtons.end() ; itBtn++ )
									buttons[ *itBtn ] = make_pair( internalValuesOrder[orderIndex].first, internalValuesOrder[orderIndex].second );
								
								for( set<int>::const_iterator itAxis = currentAxes.begin() ; itAxis != currentAxes.end() ; itAxis++ )
									axes[ *itAxis ] = internalValuesOrder[orderIndex].first;
								
								currentButtons.clear();
								currentAxes.clear();
								
								orderIndex++;
								
								if( orderIndex >= internalValuesOrder.size() )
								{
									string filename = SDL_JoystickNameForIndex( controllerId );
									
									// XML Nodes creation
									node::Node * mapping = new node::Node( node::Node::Tag, "mapping" );
									mapping->attr( "name", filename );
									
									for( map<int, pair<Mapping::Button, short int> >::iterator itBtn = buttons.begin() ; itBtn != buttons.end() ; itBtn++ )
									{
										stringstream ss;
										ss << itBtn->first;
										
										node::Node * nButton = new node::Node( node::Node::Tag, "button" );
										nButton->attr( "id", ss.str() );
										
										ss.str( "" );
										ss << itBtn->second.second;
										
										nButton->attr( "action", internalActions[ itBtn->second.first ] );
										nButton->attr( "value", ss.str() );
										
										mapping->append( nButton );
									}
									
									for( map<int, Mapping::Button>::iterator itAxis = axes.begin() ; itAxis != axes.end() ; itAxis++ )
									{
										stringstream ss;
										ss << itAxis->first;
										
										node::Node * nAxis = new node::Node( node::Node::Tag, "axis" );
										nAxis->attr( "id", ss.str() );
										nAxis->attr( "action", internalActions[ itAxis->second ] );
										
										mapping->append( nAxis );
									}
									
									// Filename special characters replacement
									for( string::iterator it = filename.begin() ; it != filename.end() ; it++ )
									{
										if( !( *it >= 'a' && *it <= 'z' ) && !( *it >= 'A' && *it <= 'Z' ) )
											*it = '_';
									}
									
									filename += ".xml";
									
									ofstream file( filename.c_str() );
									
									if( file.is_open() )
									{
										file << mapping->text( true );
										file.close();
									}
									else
									{
										cout << "The file could not be opened. XML content is:" << endl;
										cout << mapping->text( true );
									}
									
									delete mapping;
									
									orderIndex = 0;
									controllerId = -1;
								}
							}
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
				int messageX = (800 - Font::get( "font0" )->renderWidth( message )) / 2;
				Font::get( "font0" )->render( messageX, 10, message );
			}
			else
			{
				int messageX = (800 - Font::get( "font0" )->renderWidth( internalValues[ internalValuesOrder[ orderIndex ] ] )) / 2;
				Font::get( "font0" )->render( messageX, 10, internalValues[ internalValuesOrder[ orderIndex ] ] );
				
				int btnX = 0;
				int btnY = 60;
				
				for( set<int>::const_iterator itButton = currentButtons.begin() ; itButton != currentButtons.end() ; itButton++ )
				{
					stringstream ss;
					ss << "Button#" << static_cast<int>( *itButton );
					
					btnX = (800 - Font::get( "font0" )->renderWidth( ss.str() )) / 2;
					Font::get( "font0" )->render( btnX, btnY, ss.str() );
					
					btnY += Font::get( "font0" )->renderHeight( ss.str() );
				}
				
				for( set<int>::const_iterator itAxis = currentAxes.begin() ; itAxis != currentAxes.end() ; itAxis++ )
				{
					stringstream ss;
					ss << "Axis#" << static_cast<int>( *itAxis );
					
					btnX = (800 - Font::get( "font0" )->renderWidth( ss.str() )) / 2;
					Font::get( "font0" )->render( btnX, btnY, ss.str() );
					
					btnY += Font::get( "font0" )->renderHeight( ss.str() );
				}
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
