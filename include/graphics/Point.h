#ifndef __GRAPHICS_POINT_H
#define __GRAPHICS_POINT_H	1

namespace graphics
{
	class Point
	{
		protected:
			int x;
			int y;
			
		public:
			Point();
			Point( int x, int y );
			Point( const Point& point );
			~Point();
			
			int getX() const;
			int getY() const;
			void setX( int x );
			void setY( int y );
			void move( int x, int y );
			void moveBy( int dx, int dy );
			
			void render( unsigned int size = 0 );
			void render( unsigned int width, unsigned int height );
	};
}

#endif
