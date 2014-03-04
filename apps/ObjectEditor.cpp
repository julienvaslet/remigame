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
#include <cmath>

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
int currentFrame = -1;
int currentBoundingBox = -1;
int currentAttackArea = -1;
int currentDefenceArea = -1;

// Global variables
int currentZoom = 100;
UserInterface editorUi;
vector<string> panelButtons;
map<string, string> tools;
string currentTool = "move";
bool loadingState = false;
bool saveingState = false;

// Global functions
bool loadSprite( const string& filename );
bool loadObject( const string& filename );
void cleanObjectVariables();
void adjustSpriteToScreen();
void synchronizeLabel( const string& name, const string& value );
int applyZoom( int value, int zoom = 0 );
int revertZoom( int value, int zoom = 0 );
void renderBoxInformation( Box& box, Point& relative );

// Events
bool changeTool( Element * element );
bool decreaseZoom( Element * element );
bool increaseZoom( Element * element );
bool loadFile( Element * element );
bool cancelLoading( Element * element );
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
	editorUi.addElement( "zlbl_loading", new Label( "font0", "Drop a XML or PNG file here\nSimply click here to cancel" ), true );
	editorUi.getElement( "zlbl_loading" )->getBox().resize( currentScreenWidth, Screen::get()->getHeight() );
	editorUi.getElement( "zlbl_loading" )->addEventHandler( "mousedown", cancelLoading );
	
	editorUi.addElement( "zlbl_saving", new Label( "font0", "Saving the XML file..." ), true );
	editorUi.getElement( "zlbl_saving" )->getBox().resize( currentScreenWidth, Screen::get()->getHeight() );
	
	// TODO: DEBUG
	loadSprite( "data/texture2.png" );
	
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
									if( toolBox.getOrigin().getX() + toolBox.getWidth() > origin.getX() + applyZoom( sprite->getWidth() ) ) toolBox.setWidth( origin.getX() + applyZoom( sprite->getWidth() ) - toolBox.getOrigin().getX() );
									else if( toolBox.getOrigin().getX() + toolBox.getWidth() < origin.getX() ) toolBox.setWidth( origin.getX() - toolBox.getOrigin().getX() + 1 );
									
									if( toolBox.getOrigin().getY() + toolBox.getHeight() > origin.getY() + applyZoom( sprite->getHeight() ) ) toolBox.setHeight( origin.getY() + applyZoom( sprite->getHeight() ) - toolBox.getOrigin().getY() );
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
					// TODO: May test which button ? cancel with right one ?
					
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
									else if( toolBox.getOrigin().getX() > origin.getX() + applyZoom( sprite->getWidth() ) ) toolBox.getOrigin().setX( origin.getX() + applyZoom( sprite->getWidth() ) - 1 );
									
									if( toolBox.getOrigin().getY() < origin.getY() ) toolBox.getOrigin().setY( origin.getY() );
									else if( toolBox.getOrigin().getY() > origin.getY() + applyZoom( sprite->getHeight() ) ) toolBox.getOrigin().setY( origin.getY() + applyZoom( sprite->getHeight() ) - 1 );
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
						if( !loadSprite( filename ) )
							cout << "[ObjectEditor] Unable to load sprite." << endl;
					}
					else if( filename.substr( filename.length() - 4, 4 ).compare( ".xml" ) == 0 )
					{
						if( !loadObject( filename ) )
							cout << "[ObjectEditor] Unable to load object." << endl;
					}
					else
						cout << "[ObjectEditor] Invalid extension." << endl;
					
					for( vector<string>::iterator it = panelButtons.begin() ; it != panelButtons.end() ; it++ )
						editorUi.showElement( *it );
						
					editorUi.hideElement( "zlbl_loading" );
					
					SDL_EventState( SDL_DROPFILE, SDL_DISABLE );
					break;
				}
				
				// TODO: Handle keydown (keyrepeat??)
				// keyboard arrow with shift or ctrl pressed for different action
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
				Box spriteBox( origin, applyZoom( sprite->getWidth() ), applyZoom( sprite->getHeight() ) );
				spriteBox.fillSDLRect( &dstRect );
				
				SDL_RenderCopy( Screen::get()->getRenderer(), sprite->getTexture(), &srcRect, &dstRect );
				spriteBox.render( Color( 0xAA, 0xAA, 0xAA ) );
			
				// Render current animation frames
				Animation * cAnimation = animations[animationsNames[currentAnimation]];
				
				for( unsigned int i = 0 ; i < cAnimation->getFrameCount() ; i++ )
				{
					Box fBox( cAnimation->getFrameByIndex( i ).getBox() );
					fBox.getOrigin().moveBy( origin.getX(), origin.getY() );
					fBox.resize( applyZoom( fBox.getWidth() ), applyZoom( fBox.getHeight() ) );
					fBox.render( Color( 0xFF, 0xFF, 0xFF ) );
					
					if( currentFrame == i )
					{
						if( !toolActive && currentTool.compare( "box.frame" ) == 0 )
							renderBoxInformation( fBox, origin );
						
						// Render anchor
						Point aPt( cAnimation->getFrameByIndex( i ).getAnchor() );
						aPt.moveBy( origin.getX() + fBox.getOrigin().getX(), origin.getY() + fBox.getOrigin.getY() );
						aPt.render( Color( 0x00, 0xFF, 0x00 ), 5 * currentZoom / 100 );
						
						// Render bounding boxes
						for( unsigned int iBox = 0 ; i < cAnimation->getFrameByIndex( i ).getBoundingBoxesCount() ; iBox++ )
						{
							Box bBox( cAnimation->getFrameByIndex( i ).getBoundingBox( iBox ) );
							bBox.moveBy( origin.getX() + fBox.getOrigin().getX(), origin.getY() + fBox.getOrigin.getY() );
							bBox.render( Color( 0x00, 0xFF, 0x00 ) );
							
							if( iBox == currentBoundingBox )
							{
								if( !toolActive && currentTool.compare( "box.bounding" ) == 0 )
									renderBoxInformation( bBox, fBox.getOrigin() );
							}
						}
						
						// Render attack areas
						for( unsigned int iAttack = 0 ; i < cAnimation->getFrameByIndex( i ).getAttackAreasCount() ; iAttack++ )
						{
							Box aBox( cAnimation->getFrameByIndex( i ).getAttackArea( iAttack ) );
							aBox.moveBy( origin.getX() + fBox.getOrigin().getX(), origin.getY() + fBox.getOrigin.getY() );
							aBox.render( Color( 0xFF, 0x00, 0x00 ) );
							
							if( iAttack == currentAttackArea )
							{
								if( !toolActive && currentTool.compare( "box.attack" ) == 0 )
									renderBoxInformation( aBox, fBox.getOrigin() );
							}
						}
			
						
						// Render defence areas
						for( unsigned int iDefence = 0 ; i < cAnimation->getFrameByIndex( i ).getDefenceAreasCount() ; iDefence++ )
						{
							Box dBox( cAnimation->getFrameByIndex( i ).getDefenceArea( iDefence ) );
							dBox.moveBy( origin.getX() + fBox.getOrigin().getX(), origin.getY() + fBox.getOrigin.getY() );
							dBox.render( Color( 0x00, 0x00, 0xFF ) );
							
							if( iDefence == currentDefenceArea )
							{
								if( !toolActive && currentTool.compare( "box.defence" )
									renderBoxInformation( dBox, fBox.getOrigin() );
							}
						}
					}
				}
			}
			
			// Render current tool
			if( toolActive )
			{
				if( currentTool.compare( "anchor" ) == 0 )
				{
					int infoWidth = 0;
					int infoHeight = 0;
					
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
					renderBoxInformation( toolBox, origin );
				}
			}
			
			// UserInterface rendering
			Box actionPanelBorder( actionPanel );
			actionPanelBorder.resizeBy( 1, 2 );
			actionPanelBorder.getOrigin().moveBy( 0, -1 );
			actionPanel.renderFilled( Color( 0xEE, 0xEE, 0xEE ) );
			actionPanel.render( Color( 0xAA, 0xAA, 0xAA ) );
			
			editorUi.render( ticks );
			
			// Render animation preview
			if( animations.size() > 0 )
			{
				Frame * frame = animations[animationsNames[currentAnimation]]->getFrame( ticks );
			
				if( frame != NULL )
				{
					Box animationBackground( currentScreenWidth - 300, 0, 300, 300 );
					box.animationBackground( Color( 0xFF, 0xFF, 0xFF ) );
				
					int animationZoom = frame->getBox().getWidth() > frame->getBox().getHeight() ? : static_cast<int>( 300.0 / static_cast<double>( frame->getBox().getWidth() ) * 100.0 ) : static_cast<int>( 300.0 / static_cast<double>( frame->getBox().getHeight() ) * 100.0 );
					
					
					// TODO: Compute local zoom based on each animation frame (get the max width & max height & take part of anchor point)
					/*int frameZoom = 100;
					SDL_Rect dstRect;
					int anchorX = origin.getX();
					int anchorY = origin.getY();
				
					dstRect.x = anchorX - (frame->getAnchor().getX() * frameZoom / 100.0);
					dstRect.y = anchorY - (frame->getAnchor().getY() * frameZoom / 100.0);
					dstRect.w = (frameZoom * frame->getBox().getWidth()) / 100;
					dstRect.h = (frameZoom * frame->getBox().getHeight()) / 100;
	
					SDL_Rect srcRect;
					frame->getBox().fillSDLRect( &srcRect );
	
					SDL_RenderCopy( Screen::get()->getRenderer(), sprite->getTexture(), &srcRect, &dstRect );*/
				
					// Render anchor
				
					// Render bounding boxes
				
					// Render attack areas
				
					// Render defence areas
				}
			}
					
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
	
	currentFrame = -1;
	synchronizeLabel( "lbl_frame", "Frame" );
	currentBoundingBox = -1;
	synchronizeLabel( "lbl_frame", "Bounding" );
	currentAttackArea = -1;
	synchronizeLabel( "lbl_frame", "Attack" );
	currentDefenceArea = -1;
	synchronizeLabel( "lbl_frame", "Defence" );

	stringstream ss;
	currentZoom = 100;
	ss << currentZoom << " %";
	synchronizeLabel( "lbl_zoom", ss.str() );
}

int applyZoom( int value, int zoom )
{
	return static_cast<int>( round( static_cast<double>( value ) * static_cast<double>( zoom == 0 ? currentZoom : zoom ) / 100.0 ) );
}

int revertZoom( int value, int zoom )
{
	return static_cast<int>( round( static_cast<double>( value ) * 100.0 / static_cast<double>( zoom == 0 ? currentZoom : zoom ) ) );
}

void renderBoxInformation( Box& box, Point& relative )
{
	int infoWidth = 0;
	int infoHeight = 0;
	
	stringstream boxInfo;
	boxInfo << "x: " << box.getOrigin().getX() - relative.getX() << " y: " << box.getOrigin().getY() - relative.getY();
	Font::get( "font0" )->renderSize( &infoWidth, &infoHeight, boxInfo.str() );
	Font::get( "font0" )->render( box.getOrigin().getX() + ((box.getWidth() - infoWidth) / 2), box.getOrigin().getY() + (box.getHeight() / 2) - infoHeight, boxInfo.str() );

	infoWidth = 0;
	infoHeight = 0;
	boxInfo.str( "" );
	boxInfo << revertZoom( box.getWidth() ) << " x " << revertZoom( box.getHeight() );
	Font::get( "font0" )->renderSize( &infoWidth, &infoHeight, boxInfo.str() );
	Font::get( "font0" )->render( box.getOrigin().getX() + ((box.getWidth() - infoWidth) / 2), box.getOrigin().getY() + (box.getHeight() / 2), boxInfo.str() );
}

void adjustSpriteToScreen()
{
	int availableWidth = Screen::get()->getWidth() - 300;
	int availableHeight = Screen::get()->getHeight();
	int zoomOnWidth = static_cast<int>( static_cast<double>( availableWidth ) / static_cast<double>( sprite->getWidth() ) * 100.0 );
	int zoomOnHeight = static_cast<int>( static_cast<double>( availableHeight ) / static_cast<double>( sprite->getHeight() ) * 100.0 );
	currentZoom = zoomOnWidth > zoomOnHeight ? zoomOnHeight : zoomOnWidth;
	
	currentZoom -=  currentZoom % 5;
	
	stringstream ss;
	ss << currentZoom << " %";
	synchronizeLabel( "lbl_zoom", ss.str() );
	
	origin.move( (availableWidth - applyZoom( sprite->getWidth() )) / 2, (availableHeight - applyZoom( sprite->getHeight() )) / 2 );
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

bool cancelLoading( Element * element )
{
	for( vector<string>::iterator it = panelButtons.begin() ; it != panelButtons.end() ; it++ )
		editorUi.showElement( *it );
						
	editorUi.hideElement( "zlbl_loading" );
	SDL_EventState( SDL_DROPFILE, SDL_DISABLE );
	return true;
}

bool loadSprite( const string& filename )
{
	bool success = true;
	Sprite * nSprite = new Sprite( filename );
	
	if( nSprite->isLoaded() )
	{
		cleanObjectVariables();
		sprite = nSprite;
		
		spriteFilename = filename;
		objectFilename = filename.substr( 0, filename.length() - 4 ) + ".xml";
		
		adjustSpriteToScreen();
		
		// Initialize each animation with one frame
		Frame frame( Box( 0, 0, sprite->getWidth(), sprite->getHeight() ) );
		
		for( vector<string>::iterator it = animationsNames.begin() ; it != animationsNames.end() ; it++ )
		{
			animations[*it] = new Animation();
			animations[*it]->addFrame( frame );
		}
	}
	else
	{
		delete nSprite;
		success = false;
	}
	
	return success;
}

bool loadObject( const string& filename )
{
	cout << "[ObjectEditor] Object loading not supported." << endl;
	return false;
}
