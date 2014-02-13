#ifndef __GRAPHICS_BOX_H
#define __GRAPHICS_BOX_H	1

#include <graphics/Point.h>

namespace graphics
{
	class Box
	{
		protected:
			Point origin;
			int width;
			int height;
			
		public:
			Box();
			Box( const Point& point, int width, int height );
			Box( int x, int y, int width, int height );
			Box( const Box& box );
			~Box();
			
			Point& getOrigin();
			int getWidth() const;
			int getHeight() const;
			
			void setWidth( int width );
			void setHeight( int height );
			void resize( int width, int height );
	};
}

#endif
