#include <graphics/Point.h>
#include <graphics/Screen.h>

namespace graphics
{
	Point::Point() : x(0), y(0)
	{
	}
	
	Point::Point( int x, int y ) : x(x), y(y)
	{
	}
	
	Point::Point( const Point& point ) : x(point.x), y(point.y)
	{
	}
	
	Point::~Point()
	{
	}
	
	int Point::getX() const
	{
		return this->x;
	}
	
	int Point::getY() const
	{
		return this->y;
	}
	
	void Point::setX( int x )
	{
		this->x = x;
	}
	
	void Point::setY( int y )
	{
		this->y = y;
	}
	
	void Point::move( int x, int y )
	{
		this->x = x;
		this->y = y;
	}
	
	void Point::moveBy( int dx, int dy )
	{
		this->x += dx;
		this->y += dy;
	}
	
	void Point::render( Color& color, unsigned int size )
	{
		this->render( color, size, size );
	}
	
	void render( Color& color, unsigned int width, unsigned int height )
	{
		Screen::get()->setRenderColor( color );
		SDL_RenderDrawLine( Screen::get()->getRenderer(), this->x + width, this->y, this->x - width, this->y );
		SDL_RenderDrawLine( Screen::get()->getRenderer(), this->x, this->y + height, this->x, this->y - height );
	}
}
