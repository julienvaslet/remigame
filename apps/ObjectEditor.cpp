#include <SDL2/SDL.h>
#include <iostream>

#include <graphics/Screen.h>
#include <graphics/Font.h>

#include <ui/Button.h>

using namespace graphics;
using namespace ui;
using namespace std;

int main( int argc, char ** argv )
{
	if( !Screen::initialize( "Object Editor" ) )
	{
		cout << "Unable to initialize screen. Exiting." << endl;
		return 1;
	}
	
	if( !Font::load( "data/font.xml" ) )
	{
		cout << "Unable to load main font. Exiting." << endl;
		Screen::destroy();
		return 1;
	}

	bool running = true;
	SDL_Event lastEvent;
	unsigned int lastDrawTicks = 0;
	
	Button button( "font0", "Message d'essai" );
	button.getBox().getOrigin().move( 10, 10 );

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
				
				case SDL_MOUSEMOTION:
				{
					if( button.getBox().isInCollision( Point( lastEvent.motion.x, lastEvent.motion.y ) ) )
						button.trigger( "mouseenter" );
					else
						button.trigger( "mouseleave" );

					break;
				}
				
				case SDL_MOUSEBUTTONDOWN:
				{
					if( button.getBox().isInCollision( Point( lastEvent.button.x, lastEvent.button.y ) ) )
						button.trigger( "mousedown" );
						
					break;
				}
				
				case SDL_MOUSEBUTTONUP:
				{
					button.trigger( "mouseup" );
					
					break;
				}
			}
		}

		unsigned int ticks = SDL_GetTicks();
		
		if( ticks - lastDrawTicks > 15 )
		{		
			Screen::get()->clear();
			
			button.render( ticks );
					
			Screen::get()->render();	
			lastDrawTicks = ticks;
		}
	}
		
	Font::destroy( "font0" );
	Screen::destroy();
	
	return 0;
}
