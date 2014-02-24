#include <ui/Element.h>

namespace ui
{
	Element::Element()
	{
	}
	
	Element::~Element()
	{
	}
	
	void Element::setEventHandler( const string& event, Event callback )
	{
		this->events[event] = callback;
	}

	void Element::trigger( const string& event )
	{
		map<string, Event>::iterator it = this->events.find( event );
		
		if( it != this->events.end() && it->second != NULL )
		{
			// This could trigger a bad behavior if events are badly setted.
			(it->second)( this );
		}
	}
}
