#include <game/ObjectEventHandler.h>

namespace game
{
	ObjectEventHandler::ObjectEventHandler( Object * object ) : object(object)
	{
	}
	
	ObjectEventHandler::~ObjectEventHandler()
	{
	}
	
	void ObjectEventHandler::handleEvent( Controller * controller, Mapping::Button button, short int value, unsigned int timestamp )
	{
		if( button == Mapping::AXH )
		{
			if( value > 0 )
				this->object->moveBy( 5, 0 );
				
			else if( value < 0 )
				this->object->moveBy( -5, 0 );
		}
		else if( button == Mapping::AXV )
		{
			if( value > 0 )
				this->object->moveBy( 0, 5 );
				
			else if( value < 0 )
				this->object->moveBy( 0, -5 );
		}
	}
}

