#include <graphics/Box.h>
#include <graphics/Screen.h>

#include <iostream>

namespace graphics
{
	Box::Box() : origin(0,0), width(0), height(0)
	{
	}
	
	Box::Box( const Point& point, int width, int height ) : origin(point), width(width), height(height)
	{
	}
	
	Box::Box( int x, int y, int width, int height ) : origin(x, y), width(width), height(height)
	{
	}
	
	Box::Box( const Box& box ) : origin(box.origin), width(box.width), height(box.height)
	{
	}
	
	Box::~Box()
	{
	}
	
	Point& Box::getOrigin()
	{
		return this->origin;
	}
	
	int Box::getWidth() const
	{
		return this->width;
	}
	
	int Box::getHeight() const
	{
		return this->height;
	}
	
	void Box::setWidth( int width )
	{
		this->width = width;
	}
	
	void Box::setHeight( int height )
	{
		this->height = height;
	}
	
	void Box::resize( int width, int height )
	{
		this->width = width;
		this->height = height;
	}
	
	void Box::resizeBy( int dwidth, int dheight )
	{
		this->width += dwidth;
		this->height += dheight;
	}
		
	void Box::fillSDLRect( SDL_Rect * rect )
	{
		rect->x = this->getOrigin().getX();
		rect->y = this->getOrigin().getY();
		rect->w = this->width;
		rect->h = this->height;
	}
	
	void Box::render( const Color& color )
	{
		Screen::get()->setRenderColor( color );
		SDL_Rect rect;
		this->fillSDLRect( &rect );
		SDL_RenderDrawRect( Screen::get()->getRenderer(), &rect );
	}
	
	void Box::renderFilled( const Color& color )
	{
		Screen::get()->setRenderColor( color );
		SDL_Rect rect;
		this->fillSDLRect( &rect );
		SDL_RenderFillRect( Screen::get()->getRenderer(), &rect );
	}
	
	bool Box::isInCollision( const Point& point )
	{
		return ( point.getX() >= this->getOrigin().getX() && ( point.getX() <= this->getOrigin().getX() + this->width ) && point.getY() >= this->getOrigin().getY() && (point.getY() <= this->getOrigin().getY() + this->height ) );
	}
	
	bool Box::isInCollision( const Box& box )
	{
		return false;
	}
}
