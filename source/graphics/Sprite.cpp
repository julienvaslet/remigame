#include <graphics/Sprite.h>

#ifdef DEBUG0
#include <iostream>
#endif

namespace graphics
{
	Sprite::Sprite() : texture(NULL), x(0), y(0), width(0), height(0), viewX(0), viewY(0), viewWidth(0), viewHeight(0), spriteWidth(0), spriteHeight(0)
	{
	}

	Sprite::Sprite( const char * filename ) : texture(NULL), x(0), y(0), width(0), height(0), viewX(0), viewY(0), viewWidth(0), viewHeight(0), spriteWidth(0), spriteHeight(0)
	{
		this->load( filename );
	}

	Sprite::Sprite( const string& filename ) : texture(NULL), x(0), y(0), width(0), height(0), viewX(0), viewY(0), viewWidth(0), viewHeight(0), spriteWidth(0), spriteHeight(0)
	{
		this->load( filename.c_str() );
	}

	Sprite::~Sprite()
	{
		if( this->texture != NULL )
		{
			SDL_DestroyTexture( this->texture );
			
			#ifdef DEBUG0
			cout << "[Sprite] Destroyed texture (" << this->texture << ")." << endl;
			#endif
		}
	}

	bool Sprite::load( const char * filename )
	{
		bool success = true;
	
		#ifdef DEBUG0
		cout << "[Sprite] Loading texture file \"" << filename << "\"." << endl;
		#endif
	
		SDL_Surface * surface = SDL_LoadBMP( filename );
	
		if( surface != NULL )
		{
			this->spriteWidth = surface->w;
			this->spriteHeight = surface->h;
		
			if( this->viewWidth == 0 ) this->viewWidth = surface->w;
			if( this->viewHeight == 0 ) this->viewHeight = surface->h;
		
			this->texture = SDL_CreateTextureFromSurface( Screen::get()->getRenderer(), surface );
			SDL_FreeSurface( surface );
		}
		else
		{
			success = false;
		
			#ifdef DEBUG0
			cout << "[Sprite] Unable to load texture file \"" << filename << "\": " << SDL_GetError() << endl;
			#endif
		}
	
		return success;
	}

	bool Sprite::isLoaded()
	{
		return (this->texture != NULL);
	}

	void Sprite::render()
	{
		SDL_Rect dstRect;
		dstRect.x = this->x;
		dstRect.y = this->y;
		dstRect.w = this->width;
		dstRect.h = this->height;
	
		SDL_Rect srcRect;
		srcRect.x = this->viewX;
		srcRect.y = this->viewY;
		srcRect.w = this->viewWidth;
		srcRect.h = this->viewHeight;
	
		SDL_RenderCopy( Screen::get()->getRenderer(), this->texture, &srcRect, &dstRect );
	}

	int Sprite::getX()
	{
		return this->x;
	}

	int Sprite::getY()
	{
		return this->y;
	}

	int Sprite::getWidth()
	{
		return this->width;
	}

	int Sprite::getHeight()
	{
		return this->height;
	}

	int Sprite::getViewX()
	{
		return this->viewX;
	}

	int Sprite::getViewY()
	{
		return this->viewY;
	}

	int Sprite::getViewWidth()
	{
		return this->viewWidth;
	}

	int Sprite::getViewHeight()
	{
		return this->viewHeight;
	}

	int Sprite::getSpriteWidth()
	{
		return this->spriteWidth;
	}

	int Sprite::getSpriteHeight()
	{
		return this->spriteHeight;
	}

	void Sprite::move( int x, int y )
	{
		this->x = x;
		this->y = y;
	}

	void Sprite::resize( int width, int height )
	{
		this->width = width;
		this->height = height;
	}

	void Sprite::setView( int x, int y, int width, int height )
	{
		this->viewX = x;
		this->viewY = y;
		this->viewWidth = width;
		this->viewHeight = height;
	}
}

