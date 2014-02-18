#include <game/Character.h>

#ifdef DEBUG0
#include <iostream>
#endif

using namespace std;

namespace game
{
	Character::Character() : object(NULL)
	{
	}
	
	Character::~Character()
	{
		if( this->object != NULL )
			delete this->object;
	}
}

