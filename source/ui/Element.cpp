#include <ui/Element.h>

#ifdef DEBUG0
#include <iostream>
using namespace std;
#endif

namespace ui
{
	Element::Element() : disabledState(false)
	{
	}
	
	Element::~Element()
	{
	}
	
	graphics::Box& Element::getBox()
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
	
	void Element::removeEventHandler( const string& event )
	{
		map<string, vector<Event> >::iterator it = this->events.find( event );
		
		if( it != this->events.end() )
			this->events.erase( it );
	}

	void Element::trigger( const string& event )
	{
		map<string, vector<Event> >::iterator it = this->events.find( event );
		
		if( it != this->events.end() )
		{
			#ifdef DEBUG0
			cout << "[Element#" << this << "] Triggered event \"" << event << "\"." << endl;
			#endif
			
			for( vector<Event>::reverse_iterator itEvent = it->second.rbegin() ; itEvent != it->second.rend() ; itEvent++ )
			{
				if( *itEvent != NULL )
				{
					// This could trigger a bad behavior if events are badly setted.
					if( (*itEvent)( this ) == false )
					{
						#ifdef DEBUG0
						cout << "[Element#" << this << "] Event propagation stopped." << endl;
						#endif
						
						break;
					}
				}
				else
					break;
			}
		}
	}

	bool Element::isDisabled() const
	{
		return this->disabledState;
	}
	
	void Element::setDisabledState( bool state )
	{
		this->disabledState = state;
	}
}
