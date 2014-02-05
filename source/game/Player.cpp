#include <game/Player.h>

#ifdef DEBUG0
#include <iostream>
#endif

using namespace std;

namespace game
{
	Player::Player( const char * name ) : controller(NULL)
	{
		this->name = string(name);
	}
	
	Player::~Player()
	{
	}
	
	void Player::handleEvent( Mapping::Button button, short int value )
	{
		
	}
}
