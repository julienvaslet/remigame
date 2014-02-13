#include <graphics/Point.h>

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
}
