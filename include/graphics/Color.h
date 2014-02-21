#ifndef __GRAPHICS_COLOR_H
#define __GRAPHICS_COLOR_H	1

#include <string>
using namespace std;

namespace graphics
{
	class Color
	{
		protected:
			unsigned char red;
			unsigned char green;
			unsigned char blue;
			unsigned char alpha;
			
			void parseHexString( const string& hexstring );

		public:
			Color();
			Color( unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255 );
			Color( const string& hexstring );
			~Color();
			
			unsigned char getRed() const;
			unsigned char getGreen() const;
			unsigned char getBlue() const;
			unsigned char getAlpha() const;
			string getHexString() const;
			void setRed( unsigned char r );
			void setGreen( unsigned char g );
			void setBlue( unsigned char b );
			void setAlpha( unsigned char a );
			void setColor( const string& hexstring );
	};
}

#endif
