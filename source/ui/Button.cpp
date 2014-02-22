#include <ui/Button.h>
#include <SDL2/SDL.h>

namespace ui
{
	Button::Button( const string& fontName, const string& value ) : fontName(fontName), value(value)
	{
		this->autoResize();
	}
	
	Button::~Button()
	{
	}
	
	Box& Button::getBox()
	{
		return this->box;
	}
	
	const string& Button::getValue()
	{
		return this->value;
	}
	
	void Button::setValue( const string& value, bool resize )
	{
	}
	
	const string& Button::getFont()
	{
		return this->fontName;
	}
	
	void Button::setFont( const string& fontName, bool resize )
	{
	}

	void Button::autoResize()
	{
		int width = 0;
		int height = 0;
		
		Font::get( this->fontName )->renderSize( &width, &height, this->value );
		this->box.resize( width + 2, height + 2 );
	}
	
	void Button::render( unsigned int time )
	{
		int width = 0;
		int height = 0;
		
		Font::get( this->fontName )->renderSize( &width, &height, this->value );
		
		SDL_Rect rect;
		this->box.fillSDLRect( &rect );
		SDL_SetRenderDrawColor( Screen::get()->getRenderer(), 200, 200, 200, 255 );
		SDL_RenderFillRect( Screen::get()->getRenderer(), &rect );
		
		Font::get( this->fontName )->render( this->box.getOrigin().getX() + 1 + ((this->box.getWidth() - width) / 2), this->box.getOrigin().getY() + 1 + ((this->box.getHeight() - height) / 2), this->value );
	}
}

