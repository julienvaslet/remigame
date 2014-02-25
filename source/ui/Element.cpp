#include <ui/Element.h>

namespace ui
{
	Element::Element()
	{
	}
	
	Element::~Element()
	{
	}
	
	Box& Button::getBox()
	{
		return this->box;
	}
	
	void Element::addEventHandler( const string& event, Event callback )
	{
		map<string, vector<Event> >::iterator it = this->events.find( event );
		
		if( it == this->events.end() )
		{
			vector<Event> evts;
			evts.push_back( callback );
			this->events[event] = evts;
		}
		else
			this->events[event].push_back( callback );
	}

	void Element::trigger( const string& event )
	{
		map<string, vector<Event> >::iterator it = this->events.find( event );
		
		if( it != this->events.end() )
		{
			for( vector<Event>::reverse_iterator itEvent = it->second.rbegin() ; itEvent != it->second.rend() ; itEvent++ )
			{
				if( *itEvent != NULL )
				{
					// This could trigger a bad behavior if events are badly setted.
					if( (*itEvent)( this ) == false )
						break;
				}
				else
					break;
			}
		}
	}
}
