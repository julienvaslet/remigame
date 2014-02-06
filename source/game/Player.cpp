#include <game/Player.h>

#ifdef DEBUG0
#include <iostream>
#endif

using namespace std;
using namespace controller;

namespace game
{
	Player::Player( const char * name ) : controller(NULL)
	{
		this->name = string(name);
		
		#ifdef DEBUG0
		cout << "[Player: " << this->name << "] Initialized." << endl;
		#endif
	}
	
	Player::~Player()
	{
		#ifdef DEBUG0
		cout << "[Player: " << this->name << "] Destroyed." << endl;
		#endif
	}
	
	void Player::handleEvent( Mapping::Button button, short int value )
	{
		
	}
	
	void Player::setController( Controller * controller )
	{
		this->controller = controller;
		
		if( this->controller != NULL )
		{
			#ifdef DEBUG0
			cout << "[Player: " << this->name << "] Linked to Controller#" << this->controller->getId() << "." << endl;
			#endif
		}
		else
		{
			#ifdef DEBUG0
			cout << "[Player: " << this->name << "] Detached from controller." << endl;
			#endif	
		}
	}
}
