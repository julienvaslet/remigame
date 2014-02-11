#ifndef _GAME_PLAYER_H
#define _GAME_PLAYER_H	1

namespace game
{
	class Player;
}

#include <controller/Controller.h>
#include <controller/EventHandler.h>

#include <string>

#ifdef DEBUG0
#include <iostream>
#endif

using namespace std;
using namespace controller;

namespace game
{
	class Player
	{
		protected:
			string name;
			Controller * controller;
			//EventHandler * eventHandler;
		
		public:
			Player( const char * name );
			~Player();
			
			void handleEvent( Mapping::Button button, short int value, unsigned int timestamp );
			void setController( Controller * controller );
	};
}

#endif
