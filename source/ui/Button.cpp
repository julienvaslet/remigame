#include <ui/Button.h>
#include <graphics/Point.h>
#include <graphics/Box.h>

#include <SDL2/SDL.h>

#ifdef DEBUG0
#include <iostream>
using namespace std;
#endif

namespace ui
{
	Button::Button( const string& fontName, const string& value ) : fontName(fontName), value(value), highlighted(false), pushed(false)
	{
		this->autoResize();
		this->addEventHandler( "mousedown", Button::eventMouseDown );
		this->addEventHandler( "mouseup", Button::eventMouseUp );
		this->addEventHandler( "mouseenter", Button::eventMouseEnter );
		this->addEventHandler( "mouseleave", Button::eventMouseLeave );
		
		#ifdef DEBUG0
		cout << "[Element#" << this << "] Button created." << endl;
		#endif
	}
	
	Button::~Button()
	{
		#ifdef DEBUG0
		cout << "[Element#" << this << "] Button destroyed." << endl;
		#endif
	}
	
	const string& Button::getValue()
	{
		return this->value;
	}
	
	void Button::setValue( const string& value, bool resize )
	{
		this->value = value;
		
		if( resize )
			this->autoResize();
	}
	
	const string& Button::getFont()
	{
		return this->fontName;
	}
	
	void Button::setFont( const string& fontName, bool resize )
	{
		this->fontName = fontName;
		
		if( resize )
			this->autoResize();
	}

	void Button::autoResize()
	{
		int width = 0;
		int height = 0;
		
		Font::get( this->fontName )->renderSize( &width, &height, this->value );
		this->box.resize( width + 12, height + 6 );
	}
	
	void Button::render( unsigned int time )
	{
		int width = 0;
		int height = 0;
		
		Font::get( this->fontName )->renderSize( &width, &height, this->value );
		
		Color background( 0xAA, 0xAA, 0xAA );
		Color highlighted( 0xBB, 0xBB, 0xBB );
		Color topBorders( 0xDD, 0xDD, 0xDD );
		Color bottomBorders( 0x55, 0x55, 0x55 );
		
		Box box( this->box );
		
		if( !this->pushed )
			box.render( topBorders );
		else
			box.render( bottomBorders );

		box.getOrigin().moveBy( 1, 1 );
		box.resizeBy( -1, -1 );
		
		if( !this->pushed )
			box.render( bottomBorders );
		else
			box.render( topBorders );
	
		box.resizeBy( -1, -1 );
		
		if( !this->highlighted )
			box.renderFilled( background );
		else
			box.renderFilled( highlighted );
		
		Font::get( this->fontName )->render( this->box.getOrigin().getX() + ( this->pushed ? -1 : 0 ) + 1 + ((this->box.getWidth() - width) / 2), this->box.getOrigin().getY() + ( this->pushed ? -1 : 0 ) + 1 + ((this->box.getHeight() - height) / 2), this->value );
	}
	
	// Events
	bool Button::eventMouseDown( Element * element )
	{
		Button * button = reinterpret_cast<Button *>( element );
		button->pushed = true;
		return true;
	}
	
	bool Button::eventMouseUp( Element * element )
	{
		Button * button = reinterpret_cast<Button *>( element );
		button->pushed = false;
		return true;
	}
	
	bool Button::eventMouseEnter( Element * element )
	{
		Button * button = reinterpret_cast<Button *>( element );
		button->highlighted = true;
		return true;
	}
	
	bool Button::eventMouseLeave( Element * element )
	{
		Button * button = reinterpret_cast<Button *>( element );
		button->highlighted = false;
		button->pushed = false;
		return true;
	}
}

