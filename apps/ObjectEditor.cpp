#include <SDL2/SDL.h>
#include <iostream>

#include <graphics/Screen.h>
#include <graphics/Font.h>
#include <graphics/Box.h>
#include <graphics/Color.h>

#include <ui/UserInterface.h>
#include <ui/Label.h>
#include <ui/Button.h>

#include <vector>

using namespace graphics;
using namespace ui;
using namespace std;

int main( int argc, char ** argv )
{
	if( !Screen::initialize( "Object Editor", 1024, 600, true ) )
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
	int currentScreenWidth = Screen::get()->getWidth();
	Box actionPanel( currentScreenWidth - 300, 0, 300, Screen::get()->getHeight() );
	
	// h: 310 - Save & Load
	ui.addElement( "btn_load", new Button( "font0", "Load" ) );
	ui.getElement( "btn_load" )->getBox().setWidth( 137 );
	ui.getElement( "btn_load" )->getBox().getOrigin().move( currentScreenWidth - 290, 310 );
	panelButtons.push_back( "btn_load" );
	
	ui.addElement( "btn_save", new Button( "font0", "Save" ) );
	ui.getElement( "btn_save" )->getBox().setWidth( 137 );
	ui.getElement( "btn_save" )->getBox().getOrigin().move( currentScreenWidth - 147, 310 );
	panelButtons.push_back( "btn_save" );
	
	// h: 340 - Animation selection
	ui.addElement( "btn_prev_animation", new Button( "font0", "<" ) );
	ui.getElement( "btn_prev_animation" )->getBox().setWidth( 40 );
	ui.getElement( "btn_prev_animation" )->getBox().getOrigin().move( currentScreenWidth - 290 , 340 );
	panelButtons.push_back( "btn_prev_animation" );
	
	ui.addElement( "btn_next_animation", new Button( "font0", ">" ) );
	ui.getElement( "btn_next_animation" )->getBox().setWidth( 40 );
	ui.getElement( "btn_next_animation" )->getBox().getOrigin().move( currentScreenWidth - 50, 340 );
	panelButtons.push_back( "btn_next_animation" );
	
	ui.addElement( "lbl_animation", new Label( "font0", "idle" ) );
	ui.getElement( "lbl_animation" )->getBox().setWidth( 190 );
	ui.getElement( "lbl_animation" )->getBox().getOrigin().move( currentScreenWidth - 245, 340 );
	panelButtons.push_back( "lbl_animation" );
	
	// h: 370 - Speed selection
	ui.addElement( "btn_increase_speed", new Button( "font0", "~" ) );
	ui.getElement( "btn_increase_speed" )->getBox().setWidth( 40 );
	ui.getElement( "btn_increase_speed" )->getBox().getOrigin().move( currentScreenWidth - 290 , 370 );
	panelButtons.push_back( "btn_increase_speed" );
	
	ui.addElement( "btn_decrease_speed", new Button( "font0", "+" ) );
	ui.getElement( "btn_decrease_speed" )->getBox().setWidth( 40 );
	ui.getElement( "btn_decrease_speed" )->getBox().getOrigin().move( currentScreenWidth - 50, 370 );
	panelButtons.push_back( "btn_decrease_speed" );
	
	ui.addElement( "lbl_speed", new Label( "font0", "100 fps" ) );
	ui.getElement( "lbl_speed" )->getBox().setWidth( 190 );
	ui.getElement( "lbl_speed" )->getBox().getOrigin().move( currentScreenWidth - 245, 370 );
	panelButtons.push_back( "lbl_speed" );
	
	// h: 400 - Frame selection
	ui.addElement( "btn_prev_frame", new Button( "font0", "<" ) );
	ui.getElement( "btn_prev_frame" )->getBox().setWidth( 40 );
	ui.getElement( "btn_prev_frame" )->getBox().getOrigin().move( currentScreenWidth - 290, 400 );
	panelButtons.push_back( "btn_prev_frame" );
	
	ui.addElement( "btn_del_frame", new Button( "font0", "~" ) );
	ui.getElement( "btn_del_frame" )->getBox().setWidth( 40 );
	ui.getElement( "btn_del_frame" )->getBox().getOrigin().move( currentScreenWidth - 245, 400 );
	panelButtons.push_back( "btn_del_frame" );
	
	ui.addElement( "btn_add_frame", new Button( "font0", "+" ) );
	ui.getElement( "btn_add_frame" )->getBox().setWidth( 40 );
	ui.getElement( "btn_add_frame" )->getBox().getOrigin().move( currentScreenWidth - 95, 400 );
	panelButtons.push_back( "btn_add_frame" );
	
	ui.addElement( "btn_next_frame", new Button( "font0", ">" ) );
	ui.getElement( "btn_next_frame" )->getBox().setWidth( 40 );
	ui.getElement( "btn_next_frame" )->getBox().getOrigin().move( currentScreenWidth - 50, 400 );
	panelButtons.push_back( "btn_next_frame" );
	
	ui.addElement( "lbl_frame", new Button( "font0", "frame#0" ) );
	ui.getElement( "lbl_frame" )->getBox().setWidth( 100 );
	ui.getElement( "lbl_frame" )->getBox().getOrigin().move( currentScreenWidth - 200, 400 );
	panelButtons.push_back( "lbl_frame" );
	
	// h: 440 - Anchor selection
	ui.addElement( "btn_anchor", new Button( "font0", "Set the frame anchor" ) );
	ui.getElement( "btn_anchor" )->getBox().setWidth( 280 );
	ui.getElement( "btn_anchor" )->getBox().getOrigin().move( currentScreenWidth - 290, 440 );
	panelButtons.push_back( "btn_anchor" );
	
	// h: 470 - Bounding box selection
	ui.addElement( "btn_prev_boundingbox", new Button( "font0", "<" ) );
	ui.getElement( "btn_prev_boundingbox" )->getBox().setWidth( 40 );
	ui.getElement( "btn_prev_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 290, 470 );
	panelButtons.push_back( "btn_prev_boundingbox" );
	
	ui.addElement( "btn_del_boundingbox", new Button( "font0", "~" ) );
	ui.getElement( "btn_del_boundingbox" )->getBox().setWidth( 40 );
	ui.getElement( "btn_del_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 245, 470 );
	panelButtons.push_back( "btn_del_boundingbox" );
	
	ui.addElement( "btn_add_boundingbox", new Button( "font0", "+" ) );
	ui.getElement( "btn_add_boundingbox" )->getBox().setWidth( 40 );
	ui.getElement( "btn_add_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 95, 470 );
	panelButtons.push_back( "btn_add_boundingbox" );
	
	ui.addElement( "btn_next_boundingbox", new Button( "font0", ">" ) );
	ui.getElement( "btn_next_boundingbox" )->getBox().setWidth( 40 );
	ui.getElement( "btn_next_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 50, 470 );
	panelButtons.push_back( "btn_next_boundingbox" );
	
	ui.addElement( "lbl_boundingbox", new Button( "font0", "bounding#0" ) );
	ui.getElement( "lbl_boundingbox" )->getBox().setWidth( 100 );
	ui.getElement( "lbl_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 200, 470 );
	panelButtons.push_back( "lbl_boundingbox" );

	// h: 500 - Attack area selection
	ui.addElement( "btn_prev_attackarea", new Button( "font0", "<" ) );
	ui.getElement( "btn_prev_attackarea" )->getBox().setWidth( 40 );
	ui.getElement( "btn_prev_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 290, 500 );
	panelButtons.push_back( "btn_prev_attackarea" );
	
	ui.addElement( "btn_del_attackarea", new Button( "font0", "~" ) );
	ui.getElement( "btn_del_attackarea" )->getBox().setWidth( 40 );
	ui.getElement( "btn_del_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 245, 500 );
	panelButtons.push_back( "btn_del_attackarea" );
	
	ui.addElement( "btn_add_attackarea", new Button( "font0", "+" ) );
	ui.getElement( "btn_add_attackarea" )->getBox().setWidth( 40 );
	ui.getElement( "btn_add_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 95, 500 );
	panelButtons.push_back( "btn_add_attackarea" );
	
	ui.addElement( "btn_next_attackarea", new Button( "font0", ">" ) );
	ui.getElement( "btn_next_attackarea" )->getBox().setWidth( 40 );
	ui.getElement( "btn_next_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 50, 500 );
	panelButtons.push_back( "btn_next_attackarea" );
	
	ui.addElement( "lbl_attackarea", new Button( "font0", "attack#0" ) );
	ui.getElement( "lbl_attackarea" )->getBox().setWidth( 100 );
	ui.getElement( "lbl_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 200, 500 );
	panelButtons.push_back( "lbl_attackarea" );
	
	// h: 530 - Defence area selection
	ui.addElement( "btn_prev_defencearea", new Button( "font0", "<" ) );
	ui.getElement( "btn_prev_defencearea" )->getBox().setWidth( 40 );
	ui.getElement( "btn_prev_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 290, 530 );
	panelButtons.push_back( "btn_prev_defencearea" );
	
	ui.addElement( "btn_del_defencearea", new Button( "font0", "~" ) );
	ui.getElement( "btn_del_defencearea" )->getBox().setWidth( 40 );
	ui.getElement( "btn_del_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 245, 530 );
	panelButtons.push_back( "btn_del_defencearea" );
	
	ui.addElement( "btn_add_defencearea", new Button( "font0", "+" ) );
	ui.getElement( "btn_add_defencearea" )->getBox().setWidth( 40 );
	ui.getElement( "btn_add_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 95, 530 );
	panelButtons.push_back( "btn_add_defencearea" );
	
	ui.addElement( "btn_next_defencearea", new Button( "font0", ">" ) );
	ui.getElement( "btn_next_defencearea" )->getBox().setWidth( 40 );
	ui.getElement( "btn_next_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 50, 530 );
	panelButtons.push_back( "btn_next_defencearea" );
	
	ui.addElement( "lbl_defencearea", new Button( "font0", "defence#0" ) );
	ui.getElement( "lbl_defencearea" )->getBox().setWidth( 100 );
	ui.getElement( "lbl_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 200, 530 );
	panelButtons.push_back( "lbl_defencearea" );
	
	// h: 570 - Global zoom
	ui.addElement( "btn_decrease_zoom", new Button( "font0", "~" ) );
	ui.getElement( "btn_decrease_zoom" )->getBox().setWidth( 40 );
	ui.getElement( "btn_decrease_zoom" )->getBox().getOrigin().move( currentScreenWidth - 290 , 570 );
	panelButtons.push_back( "btn_decrease_zoom" );
	
	ui.addElement( "btn_increase_zoom", new Button( "font0", "+" ) );
	ui.getElement( "btn_increase_zoom" )->getBox().setWidth( 40 );
	ui.getElement( "btn_increase_zoom" )->getBox().getOrigin().move( currentScreenWidth - 50, 570 );
	panelButtons.push_back( "btn_increase_zoom" );
	
	ui.addElement( "lbl_zoom", new Label( "font0", "100%" ) );
	ui.getElement( "lbl_zoom" )->getBox().setWidth( 190 );
	ui.getElement( "lbl_zoom" )->getBox().getOrigin().move( currentScreenWidth - 245, 570 );
	panelButtons.push_back( "lbl_zoom" );
	
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
				
				case SDL_WINDOWEVENT:
				{
					switch( lastEvent.window.event )
					{
						case SDL_WINDOWEVENT_RESIZED:
						{
							int newScreenWidth = Screen::get()->getWidth();
							// Resizing & moving panel
							actionPanel.getOrigin().setX( newScreenWidth - actionPanel.getWidth() );
							actionPanel.setHeight( Screen::get()->getHeight() );
					
							// Moving buttons
							for( vector<string>::iterator it = panelButtons.begin() ; it != panelButtons.end() ; it++ )
							{
								int delta = currentScreenWidth - ui.getElement( *it )->getBox().getOrigin().getX();
								ui.getElement( *it )->getBox().getOrigin().setX( newScreenWidth - delta );
							}
							
							currentScreenWidth = newScreenWidth;
							break;
						}
					}
					
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
			actionPanelBorder.getOrigin().moveBy( 0, -1 );
			actionPanel.renderFilled( Color( 0xEE, 0xEE, 0xEE ) );
			actionPanel.render( Color( 0xAA, 0xAA, 0xAA ) );
			
			ui.render( ticks );
					
			Screen::get()->render();	
			lastDrawTicks = ticks;
		}
	}
		
	Font::destroy( "font0" );
	Screen::destroy();
	
	return 0;
}
