#include <SDL2/SDL.h>
#include <iostream>

#include <graphics/Screen.h>
#include <graphics/Font.h>
#include <graphics/Box.h>
#include <graphics/Color.h>

#include <ui/UserInterface.h>
#include <ui/Button.h>

#include <vector>

using namespace graphics;
using namespace ui;
using namespace std;

int main( int argc, char ** argv )
{
	if( !Screen::initialize( "Object Editor", 1024, 768, true ) )
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
	
	UserInterface ui;
	
	vector<string> panelButtons;
	Box actionPanel( Screen::get()->getWidth() - 150, 0, 150, Screen::get()->getHeight() );
	
	ui.addElement( "btn_load", new Button( "font0", "Load" ) );
	ui.getElement( "btn_load" )->getBox().setWidth( 80 );
	ui.getElement( "btn_load" ).getOrigin().move( Screen::get()->getWidth() - ui.getElement( "btn_load" )->getBox().getWidth() - 10, 10 );
	panelButtons.push_back( "btn_load" );
	
	ui.addElement( "btn_save", new Button( "font0", "Save" ) );
	ui.getElement( "btn_save" )->getBox().setWidth( 80 );
	ui.getElement( "btn_save" ).getOrigin().move( Screen::get()->getWidth() - ui.getElement( "btn_save" )->getBox().getWidth() - 10, 40 );
	panelButtons.push_back( "btn_save" );

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
				
				case SDL_WINDOWEVENT_RESIZED:
				{
					// Resizing & moving panel
					actionPanel.getOrigin().setX( Screen::get()->getWidth() - actionPanel.getWidth() );
					actionPanel.setHeight( Screen::get()->getHeight() );
					
					// Moving buttons
					for( vector<string>::iterator it = panelButtons.begin() ; it != panelButtons.end() ; it++ )
						ui.getElement( *it )->getBox().getOrigin().setX( Screen::get()->getWidth() - ui.getElement( *it )->getBox().getWidth() - 10 );
					
					break;
				}
				
				case SDL_MOUSEMOTION:
				{
					ui.dispatchEvent( &lastEvent );

					break;
				}
				
				case SDL_MOUSEBUTTONDOWN:
				{
					ui.dispatchEvent( &lastEvent );
						
					break;
				}
				
				case SDL_MOUSEBUTTONUP:
				{
					ui.dispatchEvent( &lastEvent );
					
					break;
				}
			}
		}

		unsigned int ticks = SDL_GetTicks();
		
		if( ticks - lastDrawTicks > 15 )
		{		
			Screen::get()->clear();
			
			Box actionPanelBorder( actionPanel );
			actionPanelBorder.resizeBy( 1, 2 );
			actionPanelBorder.moveBy( 0, -1 );
			actionPanel.renderFill( Color( 0xEE, 0xEE, 0xEE, 128 ) );
			actionPanel.render( Color( 0xAA, 0xAA, 0xAA, 128 ) );
			
			ui.render( ticks );
					
			Screen::get()->render();	
			lastDrawTicks = ticks;
		}
	}
		
	Font::destroy( "font0" );
	Screen::destroy();
	
	return 0;
}
