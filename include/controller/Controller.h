#ifndef _CONTROLLER_CONTROLLER_H
#define	_CONTROLLER_CONTROLLER_H	1

#include <SDL2/SDL.h>
#include <controller/Mapping.h>

#include <map>
#include <string>

using namespace std;

namespace controller
{
	class Controller
	{
		protected:
			static map<SDL_JoystickID, Controller *> controllers;
			static map<string, Mapping *> mappings;
			
			SDL_Joystick * joystick;
			SDL_JoystickID id;
			Mapping * mapping;
			
			//Event manager? Event *, Action *
			
			map<Mapping::Button, short int> states;
			
			Controller( SDL_Joystick * joystick );
			~Controller();
			
			void loadMapping( const char * name );
		
		public:
			static void initialize();
			static void destroy();
			static void handleEvent( const SDL_Event * event );
	};
};

#endif

