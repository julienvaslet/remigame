#include <graphics/Sprite.h>

#include <SDL2/SDL_image.h>

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

		SDL_Surface * surface = IMG_Load( filename );
	
		if( surface != NULL )
		{
			this->width = surface->w;
			this->height = surface->h;
		
			this->view.getBox().getOrigin().move( 0, 0 );
			this->view.getBox().resize( surface->w, surface->h );
			this->view.getAnchor().move( 0, surface->h );
		
			this->texture = SDL_CreateTextureFromSurface( Screen::get()->getRenderer(), surface );
			SDL_FreeSurface( surface );
		}
		else
		{
			success = false;
		
			#ifdef DEBUG0
			cout << "[Sprite#" << this << "] Unable to load texture file \"" << filename << "\": " << IMG_GetError() << endl;
			#endif
		}
	
		return success;
	}

	bool Sprite::isLoaded()
	{
		return (this->texture != NULL);
	}

	Frame& Sprite::getView()
	{
		return this->view;
	}

	int Sprite::getWidth()
	{
		return this->width;
	}

	int Sprite::getHeight()
	{
		return this->height;
	}
	
	SDL_Texture * Sprite::getTexture()
	{
		return this->texture;
	}
}

