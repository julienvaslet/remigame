#ifndef _CONTROLLER_CONTROLLER_H
#define	_CONTROLLER_CONTROLLER_H	1

#include <SDL2/SDL.h>
#include <map>

using namespace std;

namespace controller
{
	class Controller
	{
		protected:
			static map<SDL_JoystickID, Controller *> controllers;
			SDL_Joystick * joystick;
			SDL_JoystickID id;
			
			Controller( SDL_Joystick * joystick );
			~Controller();
		
		public:
			
			static void initialize();
			static void destroy();
			
			static void handleEvent( const SDL_Event * event );
	};
};

#endif

