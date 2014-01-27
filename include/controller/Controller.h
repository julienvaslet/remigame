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
		public:
			// enum class Button : short int ?
			enum Button
			{
				BTNUP,
				BTNRIGHT,
				BTNDOWN,
				BTNLEFT,
				LT1,
				RT1,
				LT2,
				RT2,
				LT3,
				RT3,
				SELECT,
				START,
				AXH,
				AXV
			};
		
		protected:
			static map<SDL_JoystickID, Controller *> controllers;
			static map<string, Mapping *> mappings;
			
			SDL_Joystick * joystick;
			SDL_JoystickID id;
			string mapping;
			
			//Event manager? Event *, Action *
			
			map<Button, short int value> states;
			
			Controller( SDL_Joystick * joystick );
			~Controller();
		
		public:
			
			
			static void initialize();
			static void destroy();
			static void handleEvent( const SDL_Event * event );
	};
};

#endif

