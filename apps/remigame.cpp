#include <SDL2/SDL.h>
#include <iostream>

#include <Screen.h>
#include <Sprite.h>

using namespace std;

int main( int argc, char ** argv )
{
	if( !Screen::initialize() )
	{
		cout << "Unable to initialize screen. Exiting." << endl;
		return 1;
	}

	Sprite * sprite = new Sprite( "texture2.bmp" );
	
	if( !sprite->isLoaded() )
	{
		cout << "Unable to load the sprite. Exiting." << endl;
		delete sprite;
		Screen::destroy();
		
		return 1;
	}
	
	sprite->move( (800 - 512) / 2, (600 - 512) / 2 );
	sprite->resize( 512, 512 );
	sprite->setView( 512, 0, 512, 512 );
	
	unsigned int speed = 500;
	unsigned int lastChangedSprite = 0;

	bool running = true;
	SDL_Event lastEvent;
	unsigned int lastDrawTicks = 0;

	while( running )
	{
		while( SDL_PollEvent( &lastEvent ) )
		{
			switch( lastEvent.type )
			{
				case SDL_QUIT:
				{
					running = false;
					break;
				}
			
				case SDL_KEYDOWN:
				{
		            if( lastEvent.key.keysym.sym == SDLK_ESCAPE )
		                running = false;

					break;
				}
			}
		}

		unsigned int ticks = SDL_GetTicks();
		
		if( ticks - lastChangedSprite > speed )
		{
			if( sprite->getViewX() + 512 >= sprite->getSpriteWidth() )
				sprite->setView( 0, 0, 512, 512 );
			else
				sprite->setView( sprite->getViewX() + 512, 0, 512, 512 );
				
			lastChangedSprite = ticks;
		}
		
		if( ticks - lastDrawTicks > 15 )
		{
			Screen::get()->clear();
			sprite->render();
			Screen::get()->render();
			
			lastDrawTicks = ticks;
		}
	}
	
	delete sprite;
	Screen::destroy();
	
	return 0;
}
