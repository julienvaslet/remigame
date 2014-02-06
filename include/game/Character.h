#ifndef _GAME_CHARACTER_H
#define _GAME_CHARACTER_H	1

#include <graphics/Object.h>
#include <controller/EventHandler.h>

using namespace controller;
using namespace graphics;

namespace game
{
	class Character : public EventHandler
	{
		protected:
			Object * object;
		
		public:
			Character();
			~Character();
			
			void handleEvent( Controller * controller, Mapping::Button button, short int value, unsigned int timestamp );
	};
}

#endif

