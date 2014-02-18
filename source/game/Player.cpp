#include <game/Player.h>

#ifdef DEBUG0
#include <iostream>
#endif

using namespace std;
using namespace controller;

namespace game
{
	Player::Player( const char * name ) : controller(NULL), eventHandler(NULL)
	{
		this->name = string(name);
		
		#ifdef DEBUG0
		cout << "[Player: " << this->name << "] Initialized." << endl;
		#endif
	}
	
	Player::~Player()
	{
		if( this->controller != NULL )
			this->controller->setPlayer( NULL );
		
		if( this->eventHandler != NULL )
			delete this->eventHandler;
			
		#ifdef DEBUG0
		cout << "[Player: " << this->name << "] Destroyed." << endl;
		#endif
	}
	
	void Player::handleEvent( Mapping::Button button, short int value, unsigned int timestamp )
	{
		if( this->eventHandler != NULL )
			this->eventHandler->handleEvent( this->controller, button, value, timestamp );
	}
	
	void Player::setEventHandler( EventHandler * eventHandler )
	{
		if( this->eventHandler != NULL )
			delete this->eventHandler;
			
		this->eventHandler = eventHandler;
	}
	
	void Player::setController( Controller * controller )
	{
		this->controller = controller;
		
		if( this->controller != NULL )
		{
			this->controller->setPlayer( this );
			
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
