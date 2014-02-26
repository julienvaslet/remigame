#include <ui/Label.h>
#include <graphics/Point.h>
#include <graphics/Box.h>

#include <SDL2/SDL.h>

#ifdef DEBUG0
#include <iostream>
using namespace std;
#endif

namespace ui
{
	Label::Label( const string& fontName, const string& value ) : fontName(fontName), value(value)
	{
		this->autoResize();
		
		#ifdef DEBUG0
		cout << "[Element#" << this << "] Label created." << endl;
		#endif
	}
	
	Label::~Label()
	{
		#ifdef DEBUG0
		cout << "[Element#" << this << "] Label destroyed." << endl;
		#endif
	}
	
	const string& Label::getValue()
	{
		return this->value;
	}
	
	void Label::setValue( const string& value, bool resize )
	{
	}
	
	const string& Label::getFont()
	{
		return this->fontName;
	}
	
	void Label::setFont( const string& fontName, bool resize )
	{
	}

	void Label::autoResize()
	{
		int width = 0;
		int height = 0;
		
		Font::get( this->fontName )->renderSize( &width, &height, this->value );
		this->box.resize( width + 12, height + 6 );
	}
	
	void Label::render( unsigned int time )
	{
		int width = 0;
		int height = 0;
		
		Font::get( this->fontName )->renderSize( &width, &height, this->value );
		this->box.renderFilled( Color( 0xBB, 0xBB, 0xBB ) );
		Font::get( this->fontName )->render( this->box.getOrigin().getX() + ((this->box.getWidth() - width) / 2), this->box.getOrigin().getY() + ((this->box.getHeight() - height) / 2), this->value );
	}
}

