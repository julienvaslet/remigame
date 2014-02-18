#ifndef _GAME_CHARACTER_H
#define _GAME_CHARACTER_H	1

#include <graphics/Object.h>

using namespace graphics;

namespace game
{
	class Character
	{
		protected:
			Object * object;
		
		public:
			Character();
			~Character();
	};
}

#endif

