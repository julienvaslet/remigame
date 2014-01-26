#include <controller/Controller.h>
#include <vector>

#ifdef DEBUG0
#include <iostream>
#endif

using namespace std;

namespace controller
{
	map<SDL_JoystickID, Controller *> Controller::controllers;
	
	Controller::Controller( SDL_Joystick * joystick )
	{
		this->joystick = joystick;
		this->id = SDL_JoystickInstanceID( this->joystick );
		
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
				// Only if the controller is not initialized
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
					cout << "Controller#" << static_cast<int>( event->jbutton.which ) << " released button " << static_cast<int>( event->jbutton.button ) << endl;
				}
				
				break;
			}
			
			case SDL_JOYBUTTONDOWN:
			{
				itController = Controller::controllers.find( event->jbutton.which );
				
				if( itController != Controller::controllers.end() )
				{
					cout << "Controller#" << static_cast<int>( event->jbutton.which ) << " pushed button " << static_cast<int>( event->jbutton.button ) << endl;
				}
				
				break;
			}
			
			case SDL_JOYAXISMOTION:
			{
				itController = Controller::controllers.find( event->jbutton.which );
				
				if( itController != Controller::controllers.end() )
				{
					cout << "Controller#" << static_cast<int>( event->jaxis.which ) << " set axis " << static_cast<int>( event->jaxis.axis ) << " at " << static_cast<int>( event->jaxis.value ) << endl;
				}
				
				break;
			}
		}
	}
}

