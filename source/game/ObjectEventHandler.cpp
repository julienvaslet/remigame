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
		if( button == Mapping::NOBTN )
		{
			if( value == Mapping::STATE_PUSHED )
			{
				short int state = 0;
			
				state = controller->getState( Mapping::AXH );
				if( state < -10 ) this->object->getOrigin().moveBy( -5, 0 );
				else if( state > 10 ) this->object->getOrigin().moveBy( 5, 0 );
			
				state = controller->getState( Mapping::AXV );
				if( state < -10 ) this->object->getOrigin().moveBy( 0, -5 );
				else if( state > 10 ) this->object->getOrigin().moveBy( 0, 5 );
			}
			
			// Compute current animation
			else if( value == Mapping::STATE_RELEASED )
			{
				cout << "COMPUTE NOW!" << endl;
				this->object->setAnimation( "idle" );
			}
		}
		else if( button == Mapping::AXH )
		{
			if( value == Mapping::STATE_RELEASED && controller->getState( Mapping::AXV ) == Mapping::STATE_RELEASED )
			{
				this->object->setAnimation( "idle" );
			}
			else if( value < -10 )
			{
				this->object->invertHorizontalAxis( false );
				this->object->setAnimation( "run" );
			}
			else if( value > 10 )
			{
				this->object->invertHorizontalAxis( true );
				this->object->setAnimation( "run" );
			}
		}
		else if( button == Mapping::AXV )
		{
			if( value == Mapping::STATE_RELEASED && controller->getState( Mapping::AXH ) == Mapping::STATE_RELEASED )
			{
				this->object->setAnimation( "idle" );
			}
			else if( value < -10 )
			{
				this->object->setAnimation( "run" );
			}
			else if( value > 10 )
			{
				this->object->setAnimation( "run" );
			}
		}
		else if( button == Mapping::BTNUP )
		{
			if( value == Mapping::STATE_PUSHED )
				this->object->setAnimation( "be-hit", 1, controller->getPlayer() );
		}
		else if( button == Mapping::BTNLEFT )
		{
			if( value == Mapping::STATE_PUSHED )
				this->object->setAnimation( "attack", 1, controller->getPlayer() );
		}
		else if( button == Mapping::LT1 )
		{
			if( value == Mapping::STATE_RELEASED )
				this->object->setAnimation( "idle" );
			else if( value > 10 )
				this->object->setAnimation( "protect" );
		}
	}
}

