#include <ui/UserInterface.h>
#include <graphics/Point.h>

namespace ui
{
	UserInterface::UserInterface()
	{
	}
	
	UserInterface::~UserInterface()
	{
		for( map<string, Element *>::iterator it = this->elements.begin() ; it != this->elements.end() ; it ++ )
			delete it->second;
	}
	
	void UserInterface::addElement( const string& name, Element * element, bool hidden )
	{
		this->elements[name] = element;
		
		if( hidden )
			this->hideElement( name );
	}
	
	void UserInterface::showElement( const string& name )
	{
		this->hiddenElements.erase( name );
	}
	
	void UserInterface::hideElement( const string& name )
	{
		this->hiddenElements.insert( name );
	}
	
	Element * UserInterface::getElement( const string& name )
	{
		return this->elements[name];
	}
	
	void UserInterface::render( unsigned int ticks )
	{
		for( map<string, Element *>::iterator it = this->elements.begin() ; it != this->elements.end() ; it++ )
		{
			if( this->hiddenElements.count( it->first ) == 0 )
				it->second->render( ticks );
		}
	}
	
	bool UserInterface::dispatchEvent( const SDL_Event * event )
	{
		bool eventHandled = false;
		
		switch( event->type )
		{
			case SDL_MOUSEMOTION:
			{
				graphics::Point point( event->motion.x, event->motion.y );
				for( map<string, Element *>::iterator it = this->elements.begin() ; it != this->elements.end() ; it++ )
				{
					if( this->hiddenElements.count( it->first ) == 0 && !it->second->isDisabled() )
					{
						if( it->second->getBox().isInCollision( point ) )
						{
							if( this->mouseoverElements.count( it->first ) == 0 )
							{
								this->mouseoverElements.insert( it->first );
								it->second->trigger( "mouseenter" );
							}
						}
						else if( this->mouseoverElements.count( it->first ) > 0 )
						{
							this->mouseoverElements.erase( it->first );
							it->second->trigger( "mouseleave" );
						}
					}
				}

				break;
			}
			
			case SDL_MOUSEBUTTONDOWN:
			{
				graphics::Point point( event->button.x, event->button.y );
				for( map<string, Element *>::iterator it = this->elements.begin() ; it != this->elements.end() ; it++ )
				{
					if( this->hiddenElements.count( it->first ) == 0 && !it->second->isDisabled() )
					{
						if( it->second->getBox().isInCollision( point ) )
						{
							it->second->trigger( "mousedown" );
							eventHandled = true;
						}
					}
				}
				
				break;
			}
			
			case SDL_MOUSEBUTTONUP:
			{
				graphics::Point point( event->button.x, event->button.y );
				for( map<string, Element *>::iterator it = this->elements.begin() ; it != this->elements.end() ; it++ )
				{
					if( this->hiddenElements.count( it->first ) == 0 && !it->second->isDisabled() )
					{
						if( it->second->getBox().isInCollision( point ) )
						{
							it->second->trigger( "mouseup" );
							eventHandled = true;
						}
					}
				}
				
				break;
			}
		}
		
		return eventHandled;
	}
}
