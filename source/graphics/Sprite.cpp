#include <graphics/Sprite.h>

#ifdef DEBUG0
#include <iostream>
#endif

namespace graphics
{
	Sprite::Sprite() : texture(NULL), width(0), height(0)
	{
	}

	Sprite::Sprite( const char * filename ) : texture(NULL), width(0), height(0)
	{
		this->load( filename );
	}

	Sprite::Sprite( const string& filename ) : texture(NULL), width(0), height(0)
	{
		this->load( filename.c_str() );
	}

	Sprite::~Sprite()
	{
		if( this->texture != NULL )
		{
			SDL_DestroyTexture( this->texture );
			
			#ifdef DEBUG0
			cout << "[Sprite#" << this << "] Destroyed texture." << endl;
			#endif
		}
	}

	bool Sprite::load( const char * filename )
	{
		bool success = true;
	
		#ifdef DEBUG0
		cout << "[Sprite#" << this << "] Loading texture file \"" << filename << "\"." << endl;
		#endif
	
		SDL_Surface * surface = SDL_LoadBMP( filename );
	
		if( surface != NULL )
		{
			this->width = surface->w;
			this->height = surface->h;
		
			this->view.x = 0;
			this->view.y = 0;
			this->view.width = surface->w;
			this->view.height = surface->h;
		
			this->texture = SDL_CreateTextureFromSurface( Screen::get()->getRenderer(), surface );
			SDL_FreeSurface( surface );
		}
		else
		{
			success = false;
		
			#ifdef DEBUG0
			cout << "[Sprite#" << this << "] Unable to load texture file \"" << filename << "\": " << SDL_GetError() << endl;
			#endif
		}
	
		return success;
	}

	bool Sprite::isLoaded()
	{
		return (this->texture != NULL);
	}

	int Sprite::getViewX()
	{
		return this->view.x;
	}

	int Sprite::getViewY()
	{
		return this->view.y;
	}

	int Sprite::getViewWidth()
	{
		return this->view.width;
	}

	int Sprite::getViewHeight()
	{
		return this->view.height;
	}

	int Sprite::getWidth()
	{
		return this->width;
	}

	int Sprite::getHeight()
	{
		return this->height;
	}

	void Sprite::setView( int x, int y, int width, int height )
	{
		this->view.x = x;
		this->view.y = y;
		this->view.width = width;
		this->view.height = height;
	}
	
	SDL_Texture * Sprite::getTexture()
	{
		return this->texture;
	}
}

