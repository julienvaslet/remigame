#include <graphics/Font.h>

namespace graphics
{
	Font::Font() : image(NULL)
	{
	}
	
	Font::~Font()
	{
	}
            
	bool Font::load( const string& filename )
	{
		return this->load( filename.c_str() );
	}
	
	bool Font::load( const char * filename )
	{
		bool success = false;
		
		//if success store it in map<string,Font*> fonts
		
		return success;
	}
	
	Font * Font::get( const string& name )
	{
		Font * font = NULL;
		
		return font;
	}
	
	Font * Font::get( const char * name )
	{
		return this->get( string( name ) );
	}
	
	void Font::destroy( const string& name )
	{
	}
	
	void Font::destroy( const char * name )
	{
		this->destroy( string( name ) );
	}
			
	void Font::render( int x, int y, const string& text )
	{
		this->render( x, y, text.c_str() );
	}
	
	void Font::render( int x, int y, const char * text )
	{
		int i = 0;
		SDL_Rect dstRect;
		dstRect.x = x;
		dstRect.y = y;
		dstRect.w = 0;
		dstRect.h = 0;
		
		SDL_Rect srcRect;
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.w = 0;
		srcRect.h = 0;
		
		while( text[i] != 0 )
		{
			map<char,Sprite::Frame>::iterator it = this->characters.find( text[i] );
			
			if( it != this->characters.end() )
			{
				srcRect.x = it->second.x;
				srcRect.y = it->second.y;
				srcRect.w = it->second.width;
				srcRect.h = it->second.height;
				
				dstRect.w = it->second.width;
				dstRect.h = it->second.height;
				
				SDL_RenderCopy( Screen::get()->getRenderer(), this->image->getTexture(), &srcRect, &dstRect );
				
				dstRect.x += dstRect.w;
			}
			else if( text[i] == '\n' )
				dstRect.y += dstRect.height;
			
			i++;
		}
	}
}
