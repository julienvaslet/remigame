#ifndef _GAME_PLAYER_H
#define _GAME_PLAYER_H	1

#include <controller/Controller.h>
#include <controller/Mapping.h>

#include <string>

#ifdef DEBUG0
#include <iostream.h>
#endif

using namespace std;

namespace game
{
	class Player
	{
		protected:
			string name;
			Controller * controller;
		
		public:
			Player( const char * name );
			~Player();
			
			void handleEvent( Mapping::Button button, short int value );
	};
}

#endif
