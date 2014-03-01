#include <SDL2/SDL.h>

#include <graphics/Screen.h>
#include <graphics/Font.h>
#include <graphics/Box.h>
#include <graphics/Color.h>
#include <graphics/Object.h>

#include <ui/UserInterface.h>
#include <ui/Label.h>
#include <ui/Button.h>
#include <ui/PushButton.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

// TODO: make a function who translate with precision currentZoom to real sizes

using namespace graphics;
using namespace ui;
using namespace std;

// Object's variables
string spriteFilename = "";
string objectFilename = "";

Point origin;
Sprite * sprite = NULL;
map<string, Animation *> animations;
vector<string> animationsNames;
unsigned int currentAnimation = 0;

// Global variables
int currentZoom = 100;
UserInterface editorUi;
vector<string> panelButtons;
map<string, string> tools;
string currentTool = "move";
bool loadingState = false;
bool saveingState = false;

// Global functions
void cleanObjectVariables();
void adjustSpriteToScreen();
void synchronizeLabel( const string& name, const string& value );

// Events
bool changeTool( Element * element );
bool decreaseZoom( Element * element );
bool increaseZoom( Element * element );
bool loadFile( Element * element );
bool saveFile( Element * element );
bool prevAnimation( Element * element );
bool nextAnimation( Element * element );

// TODO: add z-index value to???
// TODO: add disabled state to ui::Element

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
	
	// Tools variables
	Box toolBox;
	bool toolActive = false;
	
	// Set animations
	animationsNames.push_back( "idle" );
	animationsNames.push_back( "run" );
	animationsNames.push_back( "jump" );
	animationsNames.push_back( "attack" );
	animationsNames.push_back( "defence" );
	
	int currentScreenWidth = Screen::get()->getWidth();
	Box actionPanel( currentScreenWidth - 300, 0, 300, Screen::get()->getHeight() );
	
	// h: 310 - Save & Load
	editorUi.addElement( "btn_load", new Button( "font0", "Load" ) );
	editorUi.getElement( "btn_load" )->getBox().setWidth( 137 );
	editorUi.getElement( "btn_load" )->getBox().getOrigin().move( currentScreenWidth - 290, 310 );
	editorUi.getElement( "btn_load" )->addEventHandler( "mouseup", loadFile );
	panelButtons.push_back( "btn_load" );
	
	editorUi.addElement( "btn_save", new Button( "font0", "Save" ) );
	editorUi.getElement( "btn_save" )->getBox().setWidth( 137 );
	editorUi.getElement( "btn_save" )->getBox().getOrigin().move( currentScreenWidth - 147, 310 );
	editorUi.getElement( "btn_save" )->addEventHandler( "mouseup", saveFile );
	panelButtons.push_back( "btn_save" );
	
	// h: 340 - Animation selection
	editorUi.addElement( "btn_prev_animation", new Button( "font0", "<" ) );
	editorUi.getElement( "btn_prev_animation" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_prev_animation" )->getBox().getOrigin().move( currentScreenWidth - 290 , 340 );
	editorUi.getElement( "btn_prev_animation" )->addEventHandler( "mouseup", prevAnimation );
	panelButtons.push_back( "btn_prev_animation" );
	
	editorUi.addElement( "btn_next_animation", new Button( "font0", ">" ) );
	editorUi.getElement( "btn_next_animation" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_next_animation" )->getBox().getOrigin().move( currentScreenWidth - 50, 340 );
	editorUi.getElement( "btn_next_animation" )->addEventHandler( "mouseup", nextAnimation );
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
	
	editorUi.addElement( "lbl_frame", new PushButton( "font0", "frame#0" ) );
	editorUi.getElement( "lbl_frame" )->getBox().setWidth( 100 );
	editorUi.getElement( "lbl_frame" )->getBox().getOrigin().move( currentScreenWidth - 200, 400 );
	editorUi.getElement( "lbl_frame" )->addEventHandler( "mouseup", changeTool );
	panelButtons.push_back( "lbl_frame" );
	tools["lbl_frame"] = "box.frame";
	
	// h: 440 - Anchor &move selection
	editorUi.addElement( "btn_move", new PushButton( "font0", "Move", true ) );
	editorUi.getElement( "btn_move" )->getBox().setWidth( 137 );
	editorUi.getElement( "btn_move" )->getBox().getOrigin().move( currentScreenWidth - 290, 440 );
	editorUi.getElement( "btn_move" )->addEventHandler( "mouseup", changeTool );
	panelButtons.push_back( "btn_move" );
	tools["btn_move"] = "move";
	
	editorUi.addElement( "btn_anchor", new PushButton( "font0", "Set anchor" ) );
	editorUi.getElement( "btn_anchor" )->getBox().setWidth( 137 );
	editorUi.getElement( "btn_anchor" )->getBox().getOrigin().move( currentScreenWidth - 147, 440 );
	editorUi.getElement( "btn_anchor" )->addEventHandler( "mouseup", changeTool );
	panelButtons.push_back( "btn_anchor" );
	tools["btn_anchor"] = "anchor";
	
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
	
	editorUi.addElement( "lbl_boundingbox", new PushButton( "font0", "bounding#0" ) );
	editorUi.getElement( "lbl_boundingbox" )->getBox().setWidth( 100 );
	editorUi.getElement( "lbl_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 200, 470 );
	editorUi.getElement( "lbl_boundingbox" )->addEventHandler( "mouseup", changeTool );
	panelButtons.push_back( "lbl_boundingbox" );
	tools["lbl_boundingbox"] = "box.bounding";

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
	
	editorUi.addElement( "lbl_attackarea", new PushButton( "font0", "attack#0" ) );
	editorUi.getElement( "lbl_attackarea" )->getBox().setWidth( 100 );
	editorUi.getElement( "lbl_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 200, 500 );
	editorUi.getElement( "lbl_attackarea" )->addEventHandler( "mouseup", changeTool );
	panelButtons.push_back( "lbl_attackarea" );
	tools["lbl_attackarea"] = "box.attack";
	
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
	
	editorUi.addElement( "lbl_defencearea", new PushButton( "font0", "defence#0" ) );
	editorUi.getElement( "lbl_defencearea" )->getBox().setWidth( 100 );
	editorUi.getElement( "lbl_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 200, 530 );
	editorUi.getElement( "lbl_defencearea" )->addEventHandler( "mouseup", changeTool );
	panelButtons.push_back( "lbl_defencearea" );
	tools["lbl_defencearea"] = "box.defence";
	
	// h: 570 - Global zoom
	editorUi.addElement( "btn_decrease_zoom", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_decrease_zoom" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_decrease_zoom" )->getBox().getOrigin().move( currentScreenWidth - 290 , 570 );
	editorUi.getElement( "btn_decrease_zoom" )->addEventHandler( "mouseup", decreaseZoom );
	panelButtons.push_back( "btn_decrease_zoom" );
	
	editorUi.addElement( "btn_increase_zoom", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_increase_zoom" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_increase_zoom" )->getBox().getOrigin().move( currentScreenWidth - 50, 570 );
	editorUi.getElement( "btn_increase_zoom" )->addEventHandler( "mouseup", increaseZoom );
	panelButtons.push_back( "btn_increase_zoom" );
	
	editorUi.addElement( "lbl_zoom", new Label( "font0", "100%" ) );
	editorUi.getElement( "lbl_zoom" )->getBox().setWidth( 190 );
	editorUi.getElement( "lbl_zoom" )->getBox().getOrigin().move( currentScreenWidth - 245, 570 );
	panelButtons.push_back( "lbl_zoom" );
	
	// Loading & saving status hidden labels
	// TODO: Add "\nClick here to cancel" and link to one event, but correctly handle new lines ;)
	editorUi.addElement( "zlbl_loading", new Label( "font0", "Drop a XML or PNG file here" ), true );
	editorUi.getElement( "zlbl_loading" )->getBox().resize( currentScreenWidth, Screen::get()->getHeight() );
	
	editorUi.addElement( "zlbl_saving", new Label( "font0", "Saving the XML file..." ), true );
	editorUi.getElement( "zlbl_saving" )->getBox().resize( currentScreenWidth, Screen::get()->getHeight() );
	
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
							int newScreenHeight = Screen::get()->getHeight();
							
							// Resizing & moving panel
							actionPanel.getOrigin().setX( newScreenWidth - actionPanel.getWidth() );
							actionPanel.setHeight( newScreenHeight );
							
							editorUi.getElement( "zlbl_loading" )->getBox().resize( newScreenWidth, newScreenHeight );
							editorUi.getElement( "zlbl_saving" )->getBox().resize( newScreenWidth, newScreenHeight );
					
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
					
					if( toolActive )
					{
						if( currentTool.compare( "move" ) == 0 )
						{
							origin.move( lastEvent.button.x - toolBox.getOrigin().getX(), lastEvent.button.y - toolBox.getOrigin().getY() );
						}
						else if( currentTool.compare( "anchor" ) == 0 )
						{
							toolBox.getOrigin().move( lastEvent.motion.x, lastEvent.motion.y );
						}
						else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
						{
							toolBox.resize( lastEvent.motion.x - toolBox.getOrigin().getX(), lastEvent.motion.y - toolBox.getOrigin().getY() );
							
							if( currentTool.compare( "box.frame" ) == 0 )
							{
								// Restrict to the current sprite
								if( sprite != NULL )
								{
									if( toolBox.getOrigin().getX() + toolBox.getWidth() > origin.getX() + (sprite->getWidth() * currentZoom / 100) ) toolBox.setWidth( origin.getX() + (sprite->getWidth() * currentZoom / 100) - toolBox.getOrigin().getX() );
									else if( toolBox.getOrigin().getX() + toolBox.getWidth() < origin.getX() ) toolBox.setWidth( origin.getX() - toolBox.getOrigin().getX() + 1 );
									
									if( toolBox.getOrigin().getY() + toolBox.getHeight() > origin.getY() + (sprite->getHeight() * currentZoom / 100) ) toolBox.setHeight( origin.getY() + (sprite->getHeight() * currentZoom / 100) - toolBox.getOrigin().getY() );
									else if( toolBox.getOrigin().getY() + toolBox.getHeight() < origin.getY() ) toolBox.setHeight( origin.getY() - toolBox.getOrigin().getY() + 1);
								}
							}
							
							else if( currentTool.compare( "box.bounding" ) == 0
							      || currentTool.compare( "box.attack" ) == 0
							      || currentTool.compare( "box.defnce" ) == 0 )
							{
								// Restrict to the current frame
							}
						}
					}

					break;
				}
				
				case SDL_MOUSEBUTTONDOWN:
				{
					if( !editorUi.dispatchEvent( &lastEvent ) )
					{
						if( currentTool.compare( "move" ) == 0 )
						{
							toolBox.getOrigin().move( lastEvent.button.x - origin.getX(), lastEvent.button.y - origin.getY() );
							toolActive = true;
						}
						else if( currentTool.compare( "anchor" ) == 0 )
						{
							toolBox.getOrigin().move( lastEvent.button.x, lastEvent.button.y );
							toolActive = true;
						}
						else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
						{
							toolBox.getOrigin().move( lastEvent.button.x, lastEvent.button.y );
							toolBox.resize( 0, 0 );
							toolActive = true;
							
							if( currentTool.compare( "box.frame" ) == 0 )
							{
								// Restrict to the current sprite
								if( sprite != NULL )
								{
									if( toolBox.getOrigin().getX() < origin.getX() ) toolBox.getOrigin().setX( origin.getX() );
									else if( toolBox.getOrigin().getX() > origin.getX() + (sprite->getWidth() * currentZoom / 100) ) toolBox.getOrigin().setX( origin.getX() + (sprite->getWidth() * currentZoom / 100) - 1 );
									
									if( toolBox.getOrigin().getY() < origin.getY() ) toolBox.getOrigin().setY( origin.getY() );
									else if( toolBox.getOrigin().getY() > origin.getY() + (sprite->getHeight() * currentZoom / 100) ) toolBox.getOrigin().setY( origin.getY() + (sprite->getHeight() * currentZoom / 100) - 1 );
								}
							}
							
							else if( currentTool.compare( "box.bounding" ) == 0
							      || currentTool.compare( "box.attack" ) == 0
							      || currentTool.compare( "box.defnce" ) == 0 )
							{
								// Restrict to the current frame
							}
						}
					}
						
					break;
				}
				
				case SDL_MOUSEBUTTONUP:
				{
					if( !editorUi.dispatchEvent( &lastEvent ) )
					{
						if( toolActive )
						{
							if( currentTool.compare( "move" ) == 0 )
							{
								toolActive = false;
							}
							else if( currentTool.compare( "anchor" ) == 0 )
							{
								toolActive = false;
							
								// Set the frame anchor to toolBox.getOrigin()
							}
							else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
							{
								toolActive = false;
								
								if( currentTool.compare( "box.frame" ) == 0 )
								{
									// Set the frame
								}
								else if( currentTool.compare( "box.bounding" ) == 0 )
								{
									// Set the bounding box
								}
								else if( currentTool.compare( "box.attack" ) == 0 )
								{
									// Set the attack area
								}
								else if( currentTool.compare( "box.defence" ) == 0 )
								{
									// Set the defence
								}
							}
						}
					}
					
					break;
				}
				
				case SDL_DROPFILE:
				{
					string filename = string( lastEvent.drop.file );
					SDL_free( lastEvent.drop.file );
					
					if( filename.substr( filename.length() - 4, 4 ).compare( ".png" ) == 0 )
					{
						Sprite * nSprite = new Sprite( filename );
						
						if( nSprite->isLoaded() )
						{
							cleanObjectVariables();
							sprite = nSprite;
							
							spriteFilename = filename;
							objectFilename = filename.substr( 0, filename.length() - 4 ) + ".xml";
							
							adjustSpriteToScreen();
						}
						else
						{
							cout << "[ObjectEditor] Unable to load sprite." << endl;
							delete nSprite;
						}
					}
					else if( filename.substr( filename.length() - 4, 4 ).compare( ".xml" ) == 0 )
					{
						cout << "[ObjectEditor] Object loading not supported." << endl;
					}
					else
						cout << "[ObjectEditor] Invalid extension." << endl;
					
					for( vector<string>::iterator it = panelButtons.begin() ; it != panelButtons.end() ; it++ )
						editorUi.showElement( *it );
						
					editorUi.hideElement( "zlbl_loading" );
					
					SDL_EventState( SDL_DROPFILE, SDL_DISABLE );
					break;
				}
			}
		}

		unsigned int ticks = SDL_GetTicks();
		
		if( ticks - lastDrawTicks > 15 )
		{
			Screen::get()->clear();
			
			// Object rendering
			if( sprite != NULL )
			{
				SDL_Rect srcRect;
				srcRect.x = 0;
				srcRect.y = 0;
				srcRect.w = sprite->getWidth();
				srcRect.h = sprite->getHeight();
				
				SDL_Rect dstRect;
				Box spriteBox( origin, sprite->getWidth() * currentZoom / 100, sprite->getHeight() * currentZoom / 100 );
				spriteBox.fillSDLRect( &dstRect );
				
				SDL_RenderCopy( Screen::get()->getRenderer(), sprite->getTexture(), &srcRect, &dstRect );
				spriteBox.render( Color( 0xAA, 0xAA, 0xAA ) );
			}
			
			if( toolActive )
			{
				int infoWidth = 0;
				int infoHeight = 0;
				
				if( currentTool.compare( "anchor" ) == 0 )
				{
					stringstream toolBoxInfo;
					toolBoxInfo << "x: " << toolBox.getOrigin().getX() - origin.getX() << " y: " << toolBox.getOrigin().getY() - origin.getY();
					Font::get( "font0" )->renderSize( &infoWidth, &infoHeight, toolBoxInfo.str() );
					Font::get( "font0" )->render( toolBox.getOrigin().getX() - (infoWidth / 2), toolBox.getOrigin().getY() - infoHeight, toolBoxInfo.str() );
				}
				else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
				{
					Color color;
				
					if( currentTool.compare( "box.frame" ) == 0 ) color.setColor( "FFFFFF" );
					else if( currentTool.compare( "box.bounding" ) == 0 ) color.setColor( "00FF00" );
					else if( currentTool.compare( "box.attack" ) == 0 ) color.setColor( "FF0000" );
					else if( currentTool.compare( "box.defence" ) == 0 ) color.setColor( "0000FF" );
				
					toolBox.render( color );
				
					stringstream toolBoxInfo;
					toolBoxInfo << "x: " << toolBox.getOrigin().getX() - origin.getX() << " y: " << toolBox.getOrigin().getY() - origin.getY();
					Font::get( "font0" )->renderSize( &infoWidth, &infoHeight, toolBoxInfo.str() );
					Font::get( "font0" )->render( toolBox.getOrigin().getX() + ((toolBox.getWidth() - infoWidth) / 2), toolBox.getOrigin().getY() + (toolBox.getHeight() / 2) - infoHeight, toolBoxInfo.str() );
				
					infoWidth = 0;
					infoHeight = 0;
					toolBoxInfo.str( "" );
					toolBoxInfo << toolBox.getWidth() * 100 / currentZoom << " x " << toolBox.getHeight() * 100 / currentZoom;
					Font::get( "font0" )->renderSize( &infoWidth, &infoHeight, toolBoxInfo.str() );
					Font::get( "font0" )->render( toolBox.getOrigin().getX() + ((toolBox.getWidth() - infoWidth) / 2), toolBox.getOrigin().getY() + (toolBox.getHeight() / 2), toolBoxInfo.str() );
				}
			}
			
			// UserInterface rendering
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
	
	spriteFilename = "";
	objectFilename = "";

	currentAnimation = 0;
	synchronizeLabel( "lbl_animation", animationsNames[currentAnimation] );

	currentZoom = 100;
	stringstream ss;
	ss << currentZoom << " %";
	synchronizeLabel( "lbl_zoom", ss.str() );
}

void adjustSpriteToScreen()
{
	int availableWidth = Screen::get()->getWidth() - 300;
	int availableHeight = Screen::get()->getHeight();
	int zoomOnWidth = static_cast<int>( static_cast<double>( availableWidth ) / static_cast<double>( sprite->getWidth() ) * 100 );
	int zoomOnHeight = static_cast<int>( static_cast<double>( availableHeight ) / static_cast<double>( sprite->getHeight() ) * 100 );
	currentZoom = zoomOnWidth > zoomOnHeight ? zoomOnHeight : zoomOnWidth;
	
	currentZoom -=  currentZoom % 5;
	
	stringstream ss;
	ss << currentZoom << " %";
	synchronizeLabel( "lbl_zoom", ss.str() );
	
	origin.move( (availableWidth - (sprite->getWidth() * currentZoom / 100)) / 2, (availableHeight - (sprite->getHeight() * currentZoom / 100)) / 2 );
}

void synchronizeLabel( const string& name, const string& value )
{
	Label * label = reinterpret_cast<Label *>( editorUi.getElement( name ) );
	label->setValue( value );
}

bool decreaseZoom( Element * element )
{
	stringstream ss;
	currentZoom -= 5;
	
	if( currentZoom <= 0 )
		currentZoom = 5;
	
	ss << currentZoom << " %";
	synchronizeLabel( "lbl_zoom", ss.str() );
	return true;
}

bool increaseZoom( Element * element )
{
	stringstream ss;
	currentZoom += 5;
	ss << currentZoom << " %";
	synchronizeLabel( "lbl_zoom", ss.str() );
	return true;
}

bool loadFile( Element * element )
{
	for( vector<string>::iterator it = panelButtons.begin() ; it != panelButtons.end() ; it++ )
		editorUi.hideElement( *it );
	
	editorUi.showElement( "zlbl_loading" );
	
	SDL_EventState( SDL_DROPFILE, SDL_ENABLE );
	return true;
}

bool saveFile( Element * element )
{
	return true;
}

bool prevAnimation( Element * element )
{
	currentAnimation--;
	
	if( currentAnimation < 0 )
		currentAnimation = animationsNames.size() - 1;
	
	synchronizeLabel( "lbl_animation", animationsNames[currentAnimation] );
	return true;
}

bool nextAnimation( Element * element )
{
	currentAnimation++;
	
	if( currentAnimation >= animationsNames.size() )
		currentAnimation = 0;
		
	synchronizeLabel( "lbl_animation", animationsNames[currentAnimation] );
	return true;
}

bool changeTool( Element * element )
{
	PushButton * button = reinterpret_cast<PushButton *>( element );

	for( map<string, string>::iterator it = tools.begin() ; it != tools.end() ; it++ )
	{
		if( editorUi.getElement( it->first ) == button )
		{
			currentTool = it->second;
			
			// Trick: native event will invert the push state value
			button->setPushState( false );
		}
		else
		{	
			PushButton * oButton = reinterpret_cast<PushButton *>( editorUi.getElement( it->first ) );
			oButton->setPushState( false );
		}
	}
	
	return true;
}
