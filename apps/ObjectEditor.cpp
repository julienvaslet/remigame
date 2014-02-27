#include <SDL2/SDL.h>

#include <graphics/Screen.h>
#include <graphics/Font.h>
#include <graphics/Box.h>
#include <graphics/Color.h>
#include <graphics/Object.h>

#include <ui/UserInterface.h>
#include <ui/Label.h>
#include <ui/Button.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

using namespace graphics;
using namespace ui;
using namespace std;

// Object's variables
Point origin;
Sprite * sprite = NULL;
map<string, Animation *> animations;
vector<string> animationsNames;
string currentAnimation = "idle";
int currentZoom = 100;

// User Interface
UserInterface editorUi;

// Global functions
void cleanObjectVariables();

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
	
	// Set animations
	animationsNames.push_back( "idle" );
	animationsNames.push_back( "run" );
	animationsNames.push_back( "jump" );
	animationsNames.push_back( "attack" );
	animationsNames.push_back( "defence" );
	
	vector<string> panelButtons;
	int currentScreenWidth = Screen::get()->getWidth();
	Box actionPanel( currentScreenWidth - 300, 0, 300, Screen::get()->getHeight() );
	
	// h: 310 - Save & Load
	editorUi.addElement( "btn_load", new Button( "font0", "Load" ) );
	editorUi.getElement( "btn_load" )->getBox().setWidth( 137 );
	editorUi.getElement( "btn_load" )->getBox().getOrigin().move( currentScreenWidth - 290, 310 );
	panelButtons.push_back( "btn_load" );
	
	editorUi.addElement( "btn_save", new Button( "font0", "Save" ) );
	editorUi.getElement( "btn_save" )->getBox().setWidth( 137 );
	editorUi.getElement( "btn_save" )->getBox().getOrigin().move( currentScreenWidth - 147, 310 );
	panelButtons.push_back( "btn_save" );
	
	// h: 340 - Animation selection
	editorUi.addElement( "btn_prev_animation", new Button( "font0", "<" ) );
	editorUi.getElement( "btn_prev_animation" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_prev_animation" )->getBox().getOrigin().move( currentScreenWidth - 290 , 340 );
	panelButtons.push_back( "btn_prev_animation" );
	
	editorUi.addElement( "btn_next_animation", new Button( "font0", ">" ) );
	editorUi.getElement( "btn_next_animation" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_next_animation" )->getBox().getOrigin().move( currentScreenWidth - 50, 340 );
	panelButtons.push_back( "btn_next_animation" );
	
	editorUi.addElement( "lbl_animation", new Label( "font0", "idle" ) );
	editorUi.getElement( "lbl_animation" )->getBox().setWidth( 190 );
	editorUi.getElement( "lbl_animation" )->getBox().getOrigin().move( currentScreenWidth - 245, 340 );
	panelButtons.push_back( "lbl_animation" );
	
	// h: 370 - Speed selection
	editorUi.addElement( "btn_increase_speed", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_increase_speed" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_increase_speed" )->getBox().getOrigin().move( currentScreenWidth - 290 , 370 );
	panelButtons.push_back( "btn_increase_speed" );
	
	editorUi.addElement( "btn_decrease_speed", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_decrease_speed" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_decrease_speed" )->getBox().getOrigin().move( currentScreenWidth - 50, 370 );
	panelButtons.push_back( "btn_decrease_speed" );
	
	editorUi.addElement( "lbl_speed", new Label( "font0", "100 fps" ) );
	editorUi.getElement( "lbl_speed" )->getBox().setWidth( 190 );
	editorUi.getElement( "lbl_speed" )->getBox().getOrigin().move( currentScreenWidth - 245, 370 );
	panelButtons.push_back( "lbl_speed" );
	
	// h: 400 - Frame selection
	editorUi.addElement( "btn_prev_frame", new Button( "font0", "<" ) );
	editorUi.getElement( "btn_prev_frame" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_prev_frame" )->getBox().getOrigin().move( currentScreenWidth - 290, 400 );
	panelButtons.push_back( "btn_prev_frame" );
	
	editorUi.addElement( "btn_del_frame", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_del_frame" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_del_frame" )->getBox().getOrigin().move( currentScreenWidth - 245, 400 );
	panelButtons.push_back( "btn_del_frame" );
	
	editorUi.addElement( "btn_add_frame", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_add_frame" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_add_frame" )->getBox().getOrigin().move( currentScreenWidth - 95, 400 );
	panelButtons.push_back( "btn_add_frame" );
	
	editorUi.addElement( "btn_next_frame", new Button( "font0", ">" ) );
	editorUi.getElement( "btn_next_frame" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_next_frame" )->getBox().getOrigin().move( currentScreenWidth - 50, 400 );
	panelButtons.push_back( "btn_next_frame" );
	
	editorUi.addElement( "lbl_frame", new Button( "font0", "frame#0" ) );
	editorUi.getElement( "lbl_frame" )->getBox().setWidth( 100 );
	editorUi.getElement( "lbl_frame" )->getBox().getOrigin().move( currentScreenWidth - 200, 400 );
	panelButtons.push_back( "lbl_frame" );
	
	// h: 440 - Anchor selection
	editorUi.addElement( "btn_anchor", new Button( "font0", "Set the frame anchor" ) );
	editorUi.getElement( "btn_anchor" )->getBox().setWidth( 280 );
	editorUi.getElement( "btn_anchor" )->getBox().getOrigin().move( currentScreenWidth - 290, 440 );
	panelButtons.push_back( "btn_anchor" );
	
	// h: 470 - Bounding box selection
	editorUi.addElement( "btn_prev_boundingbox", new Button( "font0", "<" ) );
	editorUi.getElement( "btn_prev_boundingbox" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_prev_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 290, 470 );
	panelButtons.push_back( "btn_prev_boundingbox" );
	
	editorUi.addElement( "btn_del_boundingbox", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_del_boundingbox" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_del_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 245, 470 );
	panelButtons.push_back( "btn_del_boundingbox" );
	
	editorUi.addElement( "btn_add_boundingbox", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_add_boundingbox" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_add_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 95, 470 );
	panelButtons.push_back( "btn_add_boundingbox" );
	
	editorUi.addElement( "btn_next_boundingbox", new Button( "font0", ">" ) );
	editorUi.getElement( "btn_next_boundingbox" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_next_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 50, 470 );
	panelButtons.push_back( "btn_next_boundingbox" );
	
	editorUi.addElement( "lbl_boundingbox", new Button( "font0", "bounding#0" ) );
	editorUi.getElement( "lbl_boundingbox" )->getBox().setWidth( 100 );
	editorUi.getElement( "lbl_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 200, 470 );
	panelButtons.push_back( "lbl_boundingbox" );

	// h: 500 - Attack area selection
	editorUi.addElement( "btn_prev_attackarea", new Button( "font0", "<" ) );
	editorUi.getElement( "btn_prev_attackarea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_prev_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 290, 500 );
	panelButtons.push_back( "btn_prev_attackarea" );
	
	editorUi.addElement( "btn_del_attackarea", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_del_attackarea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_del_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 245, 500 );
	panelButtons.push_back( "btn_del_attackarea" );
	
	editorUi.addElement( "btn_add_attackarea", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_add_attackarea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_add_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 95, 500 );
	panelButtons.push_back( "btn_add_attackarea" );
	
	editorUi.addElement( "btn_next_attackarea", new Button( "font0", ">" ) );
	editorUi.getElement( "btn_next_attackarea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_next_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 50, 500 );
	panelButtons.push_back( "btn_next_attackarea" );
	
	editorUi.addElement( "lbl_attackarea", new Button( "font0", "attack#0" ) );
	editorUi.getElement( "lbl_attackarea" )->getBox().setWidth( 100 );
	editorUi.getElement( "lbl_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 200, 500 );
	panelButtons.push_back( "lbl_attackarea" );
	
	// h: 530 - Defence area selection
	editorUi.addElement( "btn_prev_defencearea", new Button( "font0", "<" ) );
	editorUi.getElement( "btn_prev_defencearea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_prev_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 290, 530 );
	panelButtons.push_back( "btn_prev_defencearea" );
	
	editorUi.addElement( "btn_del_defencearea", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_del_defencearea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_del_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 245, 530 );
	panelButtons.push_back( "btn_del_defencearea" );
	
	editorUi.addElement( "btn_add_defencearea", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_add_defencearea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_add_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 95, 530 );
	panelButtons.push_back( "btn_add_defencearea" );
	
	editorUi.addElement( "btn_next_defencearea", new Button( "font0", ">" ) );
	editorUi.getElement( "btn_next_defencearea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_next_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 50, 530 );
	panelButtons.push_back( "btn_next_defencearea" );
	
	editorUi.addElement( "lbl_defencearea", new Button( "font0", "defence#0" ) );
	editorUi.getElement( "lbl_defencearea" )->getBox().setWidth( 100 );
	editorUi.getElement( "lbl_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 200, 530 );
	panelButtons.push_back( "lbl_defencearea" );
	
	// h: 570 - Global zoom
	editorUi.addElement( "btn_decrease_zoom", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_decrease_zoom" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_decrease_zoom" )->getBox().getOrigin().move( currentScreenWidth - 290 , 570 );
	panelButtons.push_back( "btn_decrease_zoom" );
	
	editorUi.addElement( "btn_increase_zoom", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_increase_zoom" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_increase_zoom" )->getBox().getOrigin().move( currentScreenWidth - 50, 570 );
	panelButtons.push_back( "btn_increase_zoom" );
	
	editorUi.addElement( "lbl_zoom", new Label( "font0", "100%" ) );
	editorUi.getElement( "lbl_zoom" )->getBox().setWidth( 190 );
	editorUi.getElement( "lbl_zoom" )->getBox().getOrigin().move( currentScreenWidth - 245, 570 );
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
								int delta = currentScreenWidth - editorUi.getElement( *it )->getBox().getOrigin().getX();
								editorUi.getElement( *it )->getBox().getOrigin().setX( newScreenWidth - delta );
							}
							
							currentScreenWidth = newScreenWidth;
							break;
						}
					}
					
					break;
				}
				
				case SDL_MOUSEMOTION:
				{
					editorUi.dispatchEvent( &lastEvent );

					break;
				}
				
				case SDL_MOUSEBUTTONDOWN:
				{
					editorUi.dispatchEvent( &lastEvent );
						
					break;
				}
				
				case SDL_MOUSEBUTTONUP:
				{
					editorUi.dispatchEvent( &lastEvent );
					
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
			
			editorUi.render( ticks );
					
			Screen::get()->render();	
			lastDrawTicks = ticks;
		}
	}
	
	cleanObjectVariables();
		
	Font::destroy( "font0" );
	Screen::destroy();
	
	return 0;
}

void cleanObjectVariables()
{
	// Destroy animations
	for( map<string, Animation *>::iterator it = animations.begin() ; it != animations.end() ; it++ )
		delete it->second;

	animations.clear();
	origin.move( 0, 0 );
	   
	if( sprite != NULL )
	{
		delete sprite;
		sprite = NULL;
	}

	currentAnimation = "idle";
	Label * lblAnimation = reinterpret_cast<Label *>( editorUi.getElement( "lbl_animation" ) );
	lblAnimation->setValue( currentAnimation );

	currentZoom = 100;
	stringstream ss;
	ss << currentZoom;
	Label * lblZoom = reinterpret_cast<Label *>( editorUi.getElement( "lbl_zoom" ) );
	lblZoom->setValue( ss.str() );
}

