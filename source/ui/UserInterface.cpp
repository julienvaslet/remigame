#include <ui/UserInterface.h>

namespace ui
{
	UserInterface::UserInterface()
	{
	}
	
	UserInterface::~UserInterface()
	{
	}
	
	void UserInterface::addElement( const string& name, const Element& element )
	{
		this->elements[name] = element;
	}
	
	Element& UserInterface::getElement( const string& name )
	{
		return this->elements[name];
	}
	
	void UserInterface::render( unsigned int ticks )
	{
		for( map<string, Element>::iterator it = this->elements.begin() ; it != this->elements.end() ; it++ )
			it->second.render( ticks );
	}
	
	bool UserInterface::dispatchEvent( const SDL_Event * event )
	{
		bool eventHandled = false;
		
		switch( event.type )
		{
			case SDL_MOUSEMOTION:
			{
				Point point( event.motion.x, event.motion.y );
				for( map<string, Element>::iterator it = this->elements.begin() ; it != this->elements.end() ; it++ )
				{
					if( it->second.getBox().isInCollision( point ) )
					{
						it->second.trigger( "mouseenter" );
					}
					else
					{
						it->second.trigger( "mouseleave" );
					}
				}

				break;
			}
			
			case SDL_MOUSEBUTTONDOWN:
			{
				Point point( event.button.x, event.button.y );
				for( map<string, Element>::iterator it = this->elements.begin() ; it != this->elements.end() ; it++ )
				{
					if( it->second.getBox().isInCollision( point ) )
					{
						it->second.trigger( "mousedown" );
					}
				}
				
				break;
			}
			
			case SDL_MOUSEBUTTONUP:
			{
				Point point( event.button.x, event.button.y );
				for( map<string, Element>::iterator it = this->elements.begin() ; it != this->elements.end() ; it++ )
				{
					if( it->second.getBox().isInCollision( point ) )
					{
						it->second.trigger( "mouseup" );
					}
				}
				
				break;
			}
		}
		
		return eventHandled;
	}
}
