#ifndef __GRAPHICS_POINT_H
#define __GRAPHICS_POINT_H	1

namespace graphics
{
	class Point
	{
		private:
			int x;
			int y;
			
		public:
			Point();
			Point( int x, int y );
			~Point();
			
			int getX();
			int getY();
			void setX( int x );
			void setY( int y );
			void move( int x, int y );
	};
}

#endif
