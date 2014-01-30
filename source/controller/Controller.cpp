#include <controller/Controller.h>
#include <data/parser/NodeParser.h>

#ifdef DEBUG0
#include <iostream>
#endif

#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;
using namespace data;

namespace controller
{
	map<SDL_JoystickID, Controller *> Controller::controllers;
	map<string, Mapping *> Controller::mappings;
	
	Controller::Controller( SDL_Joystick * joystick ) : joystick(joystick), id(0), mapping(NULL)
	{
		this->id = SDL_JoystickInstanceID( this->joystick );
		this->loadMapping( SDL_JoystickName( this->joystick ) );
		
		// Initialize buttons states
		this->states[Mapping::BTNUP] = Mapping::STATE_RELEASED;
		this->states[Mapping::BTNRIGHT] = Mapping::STATE_RELEASED;
		this->states[Mapping::BTNDOWN] = Mapping::STATE_RELEASED;
		this->states[Mapping::BTNLEFT] = Mapping::STATE_RELEASED;
		this->states[Mapping::LT1] = Mapping::STATE_RELEASED;
		this->states[Mapping::LT2] = Mapping::STATE_RELEASED;
		this->states[Mapping::LT3] = Mapping::STATE_RELEASED;
		this->states[Mapping::RT1] = Mapping::STATE_RELEASED;
		this->states[Mapping::RT2] = Mapping::STATE_RELEASED;
		this->states[Mapping::RT3] = Mapping::STATE_RELEASED;
		this->states[Mapping::SELECT] = Mapping::STATE_RELEASED;
		this->states[Mapping::START] = Mapping::STATE_RELEASED;
		this->states[Mapping::AXH] = Mapping::STATE_RELEASED;
		this->states[Mapping::AXV] = Mapping::STATE_RELEASED;
		
		#ifdef DEBUG0
		cout << "[Controller#" << this->id << "] Initialized." << endl;
		cout << "[Controller#" << this->id << "] Name: " << SDL_JoystickName( this->joystick ) << endl;
		#endif
	}
	
	Controller::~Controller()
	{
		if( this->joystick != NULL )
		{
			SDL_JoystickClose( this->joystick );
			
			#ifdef DEBUG0
			cout << "[Controller#" << this->id << "] Closed." << endl;
			#endif
		}
	}
	
	void Controller::initialize()
	{
		#ifdef DEBUG0
		cout << "[Controller] Looking for available controllers..." << endl;
		#endif
	
		SDL_Joystick * joystick = NULL;

		for( int i = 0 ; i < SDL_NumJoysticks() ; i++ )
		{
			joystick = SDL_JoystickOpen( i );
		
			if( joystick != NULL )
			{
				// Only if the controller is not already initialized
				SDL_JoystickID controllerId = SDL_JoystickInstanceID( joystick );
				map<SDL_JoystickID, Controller *>::iterator itCtrl = Controller::controllers.find( controllerId );
				
				if( itCtrl == Controller::controllers.end() )
					Controller::controllers[controllerId] = new Controller( joystick );
			}
			#ifdef DEBUG0
			else
			{
				cout << "[Controller] Can not open #" << i << ": " << SDL_GetError() << endl;
			}
			#endif
		}
		
		#ifdef DEBUG0
		cout << "[Controller] Controllers loaded: " << Controller::controllers.size() << "." << endl;
		#endif
	}
	
	void Controller::destroy()
	{
		for( map<SDL_JoystickID, Controller *>::iterator itCtrl = Controller::controllers.begin() ; itCtrl != Controller::controllers.end() ; )
		{
			delete itCtrl->second;
			Controller::controllers.erase( itCtrl++ );
		}
		
		for( map<string, Mapping *>::iterator itMapping = Controller::mappings.begin() ; itMapping != Controller::mappings.end() ; )
		{
			delete itMapping->second;
			Controller::mappings.erase( itMapping++ );
		}
	}
	
	void Controller::loadMapping( const char * name )
	{
		#ifdef DEBUG0
		cout << "[Controller#" << this->id << "] Loading file \"data/controller/mappings.xml\"..." << endl;
		#endif
		
		ifstream file( "data/controller/mappings.xml" );
		
		if( file.is_open() )
		{
			stringstream ss;
			ss << file.rdbuf();
			
			parser::NodeParser nParser( ss.str() );
			node::Node * root = nParser.parse();
			
			if( root != NULL )
			{
				node::Node * nMappings = root->find( "mappings" );
				
				if( nMappings != NULL )
				{
					string mapping = "";
					node::Node * nMapping = nMappings->childAt( 0 );
					
					while( nMapping != NULL )
					{
						if( nMapping->getType() == node::Node::Tag && nMapping->getName() == "mapping" )
						{
							if( nMapping->hasAttr( "name" ) && nMapping->hasAttr( "file" ) )
							{
								// Loading generic mapping by default
								if( mapping == "" && nMapping->attr( "name" ) == "generic" )
								{
									mapping = nMapping->attr( "file" );
								}
								
								// Loading matching mapping
								else if( false )
								{
								}
							}
						}
						
						nMapping = nMapping->next();
					}
					
					if( mapping != "" )
					{
						map<string, Mapping *>::iterator itMapping = Controller::mappings.find( mapping );
						
						if( itMapping == Controller::mappings.end() )
						{
							Mapping * m = new Mapping( mapping );
							
							if( m->isLoaded() )
							{
								this->mapping = m;
								Controller::mappings[mapping] = m;
							}
							else
							{
								delete m;
								this->mapping = NULL;
							}
						}
						else
							this->mapping = itMapping->second;
					}
					else
					{
						this->mapping = NULL;

						#ifdef DEBUG0
						cout << "[Controller#" << this->id << "] Unable to find a suitable mapping." << endl;
						#endif
					}
				}
				else
				{
					#ifdef DEBUG0
					cout << "[Controller#" << this->id << "] Unable to find \"mappings\" tag." << endl;
					#endif
				}
			}
			else
			{
				#ifdef DEBUG0
				cout << "[Controller#" << this->id << "] Unable to parse file." << endl;
				#endif
			}
		}
		else
		{
			#ifdef DEBUG0
			cout << "[Controller#" << this->id << "] Unable to open file \"data/controller/mappings.xml\"." << endl;
			#endif
		}
	}
	
	void Controller::handleEvent( const SDL_Event * event )
	{
		map<SDL_JoystickID, Controller *>::iterator itController = Controller::controllers.end();
		
		switch( event->type )
		{
			case SDL_JOYBUTTONUP:
			{
				itController = Controller::controllers.find( event->jbutton.which );
				
				if( itController != Controller::controllers.end() )
				{
					if( itController->second->mapping != NULL )
					{
						itController->second->updateState( itController->second->mapping->getButtonFromButton( event->jbutton.button ), Mapping::STATE_RELEASED );
						//cout << "[Controller#" << itController->second->id << "] Action \"" << itController->second->mapping->getButtonFromButton( event->jbutton.button ) << "\" released." << endl;
						//cout << "Controller#" << static_cast<int>( event->jbutton.which ) << " released button " << static_cast<int>( event->jbutton.button ) << endl;
					}
				}
				
				break;
			}
			
			case SDL_JOYBUTTONDOWN:
			{
				itController = Controller::controllers.find( event->jbutton.which );
				
				if( itController != Controller::controllers.end() )
				{
					if( itController->second->mapping != NULL )
					{
						itController->second->updateState( itController->second->mapping->getButtonFromButton( event->jbutton.button ), Mapping::STATE_PUSHED );
						//cout << "[Controller#" << itController->second->id << "] Action \"" << itController->second->mapping->getButtonFromButton( event->jbutton.button ) << "\" pushed." << endl;
						//cout << "Controller#" << static_cast<int>( event->jbutton.which ) << " pushed button " << static_cast<int>( event->jbutton.button ) << endl;
					}
				}
				
				break;
			}
			
			case SDL_JOYAXISMOTION:
			{
				itController = Controller::controllers.find( event->jbutton.which );
				
				if( itController != Controller::controllers.end() )
				{
					itController->second->updateState( itController->second->mapping->getButtonFromAxis( event->jaxis.axis ), event->jaxis.value );
					//cout << "[Controller#" << itController->second->id << "] Action \"" << itController->second->mapping->getButtonFromAxis( event->jaxis.axis ) << "\" set with value " << static_cast<int>( event->jaxis.value ) << "." << endl;
					//cout << "Controller#" << static_cast<int>( event->jaxis.which ) << " set axis " << static_cast<int>( event->jaxis.axis ) << " at " << static_cast<int>( event->jaxis.value ) << endl;
				}
				
				break;
			}
		}
	}
	
	void Controller::updateState( Mapping::Button button, short int value )
	{
		this->states[button] = value;
		//cout << "[Controller#" << this->id << "] Button#" << button << " set with value " << static_cast<int>( value ) << "." << endl;
		// Pop action?
	}
	
	short int Controller::getState( Mapping::Button button )
	{
		short int value = Mapping::STATE_RELEASED;
		map<Mapping::Button, short int>::iterator it = this->states.find( button );
		
		if( it != this->states.end() )
			value = it->second;
		
		return value;
	}
}

