#include <ui/PushButton.h>
#include <graphics/Point.h>
#include <graphics/Box.h>

#include <SDL2/SDL.h>

#ifdef DEBUG0
#include <iostream>
using namespace std;
#endif

namespace ui
{
	PushButton::PushButton( const string& fontName, const string& value, bool pushed ) : Button(fontName, value), pushState(pushed)
	{
		this->pushed = this->pushState;
		
		this->removeEventHandler( "mouseup" );
		this->addEventHandler( "mouseup", PushButton::eventMouseUp );
		
		this->removeEventHandler( "mouseleave" );
		this->addEventHandler( "mouseup", PushButton::eventMouseLeave );
		
		#ifdef DEBUG0
		cout << "[Element#" << this << "] PushButton created." << endl;
		#endif
	}
	
	PushButton::~PushButton()
	{
		#ifdef DEBUG0
		cout << "[Element#" << this << "] PushButton destroyed." << endl;
		#endif
	}
	

	void PushButton::setPushState( bool state )
	{
		this->pushState = state;
		this->pushed = this->pushState;
	}
	
	bool PushButton::getPushState() const
	{
		return this->pushState;
	}
	
	// Events
	bool PushButton::eventMouseUp( Element * element )
	{
		PushButton * button = reinterpret_cast<PushButton *>( element );
		button->pushState = !button->pushState;
		button->pushed = button->pushState;
		return true;
	}
	
	bool PushButton::eventMouseLeave( Element * element )
	{
		PushButton * button = reinterpret_cast<PushButton *>( element );
		button->highlighted = false;
		button->pushed = button->pushState;
		return true;
	}
}

