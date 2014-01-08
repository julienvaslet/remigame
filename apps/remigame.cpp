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

	Sprite * sprite = NULL;
	
	unsigned int speed = 500;
	unsigned int lastChangedSprite = 0;

	bool running = true;
	SDL_Event lastEvent;
	unsigned int lastDrawTicks = 0;
	
	//SDL_EventState( SDL_DROPFILE, SDL_ENABLE );

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
					
					if( nSprite.isLoaded() )
					{
						sprite->move( (800 - 512) / 2, (600 - 512) / 2 );
						sprite->resize( 512, 512 );
						sprite->setView( 0, 0, 512, 512 );
						
						if( sprite != NULL )
						{
							Sprite * oSprite = sprite;
							sprite = nSprite;
							delete oSprite;
						}
						else
							sprite = nSprite;
					}
					else
					{
						delete nSprite;
						cout << "Unable to load the sprite. Keeping old one." << endl;
					}
					
					SDL_Free( lastEvent.drop.file );
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
