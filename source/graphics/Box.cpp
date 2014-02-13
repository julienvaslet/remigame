#include <graphics.h>

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
		return this->point;
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
}
