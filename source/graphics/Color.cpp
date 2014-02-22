#include <graphics/Color.h>
#include <sstream>

namespace graphics
{
	Color::Color() : red(0), green(0), blue(0), alpha(255)
	{
	}
	
	Color::Color( unsigned char r, unsigned char g, unsigned char b, unsigned char a ) : red(r), green(g), blue(b), alpha(a)
	{
	}
	
	Color::Color( const string& hexstring ) : red(0), green(0), blue(0), alpha(255)
	{
		this->parseHexString( hexstring );
	}
	
	Color::~Color()
	{
	}
	
	void Color::parseHexString( const string& hexstring )
	{
		istringstream iss;
		int value = 0;
		
		if( hexstring.length() >= 6 )
		{
			iss.str( hexstring.substr( 0, 2 ) );
			iss >> hex >> value;
			this->red = static_cast<unsigned char>( value );
			
			iss.str( hexstring.substr( 2, 2 ) );
			iss >> hex >> value;
			this->green = static_cast<unsigned char>( value );
			
			iss.str( hexstring.substr( 4, 2 ) );
			iss >> hex >> value;
			this->blue = static_cast<unsigned char>( value );
			
			if( hexstring.length() >= 8 )
			{
				iss.str( hexstring.substr( 6, 2 ) );
				iss >> hex >> value;
				this->alpha = static_cast<unsigned char>( value );
			}
		}
	}
	
	unsigned char Color::getRed() const
	{
		return this->red;
	}
	
	unsigned char Color::getGreen() const
	{
		return this->green;
	}
	
	unsigned char Color::getBlue() const
	{
		return this->blue;
	}
	
	unsigned char Color::getAlpha() const
	{
		return this->alpha;
	}
	
	string Color::getHexString( bool printAlpha ) const
	{
		stringstream ss;
		ss.setf( std::ios::hex, std::ios::basefield );
		ss << static_cast<int>( this->red ) << static_cast<int>( this->green ) << static_cast<int>( this->blue );
		
		if( printAlpha )
			ss << static_cast<int>( this->alpha );
		
		return ss.str();
	}
	
	void Color::setRed( unsigned char r )
	{
		this->red = r;
	}
	
	void Color::setGreen( unsigned char g )
	{
		this->green = g;
	}
	
	void Color::setBlue( unsigned char b )
	{
		this->blue = b;
	}
	
	void Color::setAlpha( unsigned char a )
	{
		this->alpha = a;
	}
	
	void Color::setColor( const string& hexstring )
	{
		this->parseHexString( hexstring );
	}
}
