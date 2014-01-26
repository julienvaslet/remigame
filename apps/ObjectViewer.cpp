#include <SDL2/SDL.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <graphics/Screen.h>
#include <graphics/Object.h>
#include <graphics/Sprite.h>
#include <graphics/Font.h>

using namespace graphics;
using namespace std;

int main( int argc, char ** argv )
{
	if( !Screen::initialize( "Object Viewer" ) )
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
	
	Object * object = NULL;

	bool running = true;
	SDL_Event lastEvent;
	unsigned int lastDrawTicks = 0;

	stringstream framesPerSecondText;
	int framesPerSecond = 0;
	unsigned int lastSecond = 0;
	
	SDL_EventState( SDL_DROPFILE, SDL_ENABLE );
	
	unsigned int currentAnimation = 0;
	vector<string> animations;
	animations.push_back( "idle" );
	animations.push_back( "walk" );
	animations.push_back( "run" );
	animations.push_back( "jump" );

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
					Object * nObject = new Object( lastEvent.drop.file );
					
					if( nObject->isLoaded() )
					{
						nObject->move( (800 - 512) / 2, (600 - 512) / 2 );
						nObject->resize( 512, 512 );
						
						if( object != NULL )
							delete object;
							
						object = nObject;
						currentAnimation = 0;
					}
					else
					{
						delete nObject;
						cout << "Unable to load the object. Keeping old one." << endl;
					}
					
					SDL_free( lastEvent.drop.file );
					break;
				}
			
				case SDL_KEYDOWN:
				{
					switch( lastEvent.key.keysym.sym )
					{
						case SDLK_ESCAPE:
						{
							running = false;
							break;
						}
						
						case SDLK_KP_PLUS:
						case SDLK_PLUS:
						{
							// Increase animation speed
							if( object != NULL && object->getAnimationSpeed() + object->getSpeedModulation() > 10 )
								 object->setSpeedModulation( object->getSpeedModulation() - 10 );
							break;
						}
						
						case SDLK_KP_MINUS:
						case SDLK_MINUS:
						{
							// Decrease animation speed
							if( object != NULL )
								 object->setSpeedModulation( object->getSpeedModulation() + 10 );
							break;
						}
						
						case SDLK_KP_TAB:
						case SDLK_TAB:
						{
							// Switch animation
							currentAnimation = (currentAnimation + 1) % animations.size();
							
							if( object != NULL )
							{
								object->setAnimation( animations.at( currentAnimation ) );
								object->setSpeedModulation( 0 );
							}
							
							break;
						}
					}

					break;
				}
			}
		}

		unsigned int ticks = SDL_GetTicks();
		
		if( ticks - lastDrawTicks > 15 )
		{
			if( ticks - lastSecond >= 1000 )
			{
				lastSecond = ticks;
				framesPerSecondText.str("");
				framesPerSecondText << framesPerSecond << " FPS";
				framesPerSecond = 0;
			}
			else
				framesPerSecond++;
				
			Screen::get()->clear();
			
			int fpsX = (800 - Font::get( "font0" )->renderWidth( framesPerSecondText.str() )) / 2;
			Font::get( "font0" )->render( fpsX, 10, framesPerSecondText.str() );
			
			if( object != NULL )
			{
				int animationX = Font::get( "font0" )->renderWidth( "Animation: " );
				int speedTextWidth = Font::get( "font0" )->renderWidth( "Speed: " );
				int speedWidth = Font::get( "font0" )->renderWidth( object->getAnimationSpeed() + object->getSpeedModulation() );
				
				Font::get( "font0" )->render( 10, 10, "Animation: " );
				Font::get( "font0" )->render( 790 - speedWidth - speedTextWidth, 10, "Speed: " );
				Font::get( "font0" )->render( animationX, 10, animations[currentAnimation] );
				Font::get( "font0" )->render( 790 - speedWidth, 10, object->getAnimationSpeed() + object->getSpeedModulation() );
				
				object->render( ticks );
			}
			else
			{
				// Show "drop file" message
				// window height & width are hardcoded
				int firstMessageX = 0;
				int firstMessageY = 0;
				int firstMessageWidth = 0;
				int firstMessageHeight = 0;
				Font::get( "font0" )->renderSize( &firstMessageWidth, &firstMessageHeight, "Drop an object XML file here" );
				firstMessageX = (800 - firstMessageWidth) / 2;
				firstMessageY = (600 - firstMessageHeight) / 2;
				Font::get( "font0" )->render( firstMessageX, firstMessageY, "Drop an object XML file here" );
			}

			Screen::get()->render();
			
			lastDrawTicks = ticks;
		}
	}
	
	if( object != NULL )
		delete object;
		
	Font::destroy( "font0" );
	Screen::destroy();
	
	return 0;
}
