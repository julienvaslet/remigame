#ifndef _SPRITE_H
#define	_SPRITE_H	1

#include <SDL2/SDL.h>
#include <string>
#include <Screen.h>

using namespace std;

class Sprite
{
	protected:
		SDL_Texture * texture;
		int x;
		int y;
		int width;
		int height;
		
		int viewX;
		int viewY;
		int viewWidth;
		int viewHeight;
		
		int spriteWidth;
		int spriteHeight;
	
	public:
		Sprite();
		Sprite( const char * filename );
		Sprite( const string& filename );
		~Sprite();
		
		bool load( const char * filename );
		void render();
		bool isLoaded();
		
		void move( int x, int y );
		void resize( int width, int height );
		
		void setView( int x, int y, int width, int height );
		
		int getX();
		int getY();
		int getWidth();
		int getHeight();
		
		int getViewX();
		int getViewY();
		int getViewWidth();
		int getViewHeight();
		
		int getSpriteWidth();
		int getSpriteHeight();
};

#endif

