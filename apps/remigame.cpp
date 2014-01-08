#include <SDL2/SDL.h>
#include <iostream>

#include <graphics/Screen.h>
#include <graphics/Sprite.h>

using namespace graphics;
using namespace std;

int main( int argc, char ** argv )
{
	if( !Screen::initialize() )
	{
		cout << "Unable to initialize screen. Exiting." << endl;
		return 1;
	}

	Sprite * sprite = NULL;
	
	unsigned int speed = 500;
	unsigned int lastChangedSprite = 0;

	bool running = true;
	SDL_Event lastEvent;
	unsigned int lastDrawTicks = 0;
	
	SDL_EventState( SDL_DROPFILE, SDL_ENABLE );

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
				
				case SDL_DROPFILE:
				{
					Sprite * nSprite = new Sprite( lastEvent.drop.file );
					
					if( nSprite->isLoaded() )
					{
						nSprite->move( (800 - 512) / 2, (600 - 512) / 2 );
						nSprite->resize( 512, 512 );
						nSprite->setView( 0, 0, 512, 512 );
						
						if( sprite != NULL )
							delete sprite;

						sprite = nSprite;
					}
					else
					{
						delete nSprite;
						cout << "Unable to load the sprite. Keeping old one." << endl;
					}
					
					SDL_free( lastEvent.drop.file );
					break;
				}
			
				case SDL_KEYDOWN:
				{
					if( lastEvent.key.keysym.sym == SDLK_ESCAPE )
						running = false;
						
					else if( lastEvent.key.keysym.sym == SDLK_KP_PLUS || lastEvent.key.keysym.sym ==  SDLK_PLUS )
					{
						if( speed > 50 )
							speed -= 50;					
					}
					
					else if( lastEvent.key.keysym.sym == SDLK_KP_MINUS || lastEvent.key.keysym.sym == SDLK_MINUS )
					{
						speed += 50;
					}

					break;
				}
			}
		}

		unsigned int ticks = SDL_GetTicks();
		
		if( sprite != NULL && ticks - lastChangedSprite > speed )
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
			
			if( sprite != NULL )
				sprite->render();

			Screen::get()->render();
			
			lastDrawTicks = ticks;
		}
	}
	
	if( sprite != NULL )
		delete sprite;

	Screen::destroy();
	
	return 0;
}
