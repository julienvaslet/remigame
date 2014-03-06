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
unsigned int currentFrame = 0;
unsigned int currentBoundingBox = 0;
unsigned int currentAttackArea = 0;
unsigned int currentDefenceArea = 0;

// Global variables
int currentZoom = 100;
UserInterface editorUi;
vector<string> panelButtons;
map<string, string> tools;
string currentTool = "move";
bool loadingState = false;
bool savingState = false;
bool shiftKeyState = false;
bool ctrlKeyState = false;
bool altKeyState = false;

// Global functions
bool loadSprite( const string& filename );
bool loadObject( const string& filename );
void cleanObjectVariables();
void adjustSpriteToScreen();
void synchronizeLabel( const string& name, const string& value );
void synchronizeLabels();
int applyZoom( int value, int zoom = 0 );
int revertZoom( int value, int zoom = 0 );
void renderBoxInformation( Box& box, Point& relative );
void renderPointInformation( Point& point, Point& relative );

// Events
bool changeTool( Element * element );

// Zoom Events
bool decreaseZoom( Element * element );
bool increaseZoom( Element * element );

// Speed Events
bool decreaseSpeed( Element * element );
bool increaseSpeed( Element * element );

// Frame Events
bool addFrame( Element * element );
bool deleteFrame( Element * element );
bool nextFrame( Element * element );
bool prevFrame( Element * element );

// File Events
bool loadFile( Element * element );
bool cancelLoading( Element * element );
bool saveFile( Element * element );

// Animation Events
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
	editorUi.addElement( "btn_increase_speed", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_increase_speed" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_increase_speed" )->getBox().getOrigin().move( currentScreenWidth - 50, 370 );
	editorUi.getElement( "btn_increase_speed" )->addEventHandler( "mouseup", increaseSpeed );
	panelButtons.push_back( "btn_increase_speed" );
	
	editorUi.addElement( "btn_decrease_speed", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_decrease_speed" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_decrease_speed" )->getBox().getOrigin().move( currentScreenWidth - 290 , 370 );
	editorUi.getElement( "btn_decrease_speed" )->addEventHandler( "mouseup", decreaseSpeed );
	panelButtons.push_back( "btn_decrease_speed" );
	
	editorUi.addElement( "lbl_speed", new Label( "font0", "100 fps" ) );
	editorUi.getElement( "lbl_speed" )->getBox().setWidth( 190 );
	editorUi.getElement( "lbl_speed" )->getBox().getOrigin().move( currentScreenWidth - 245, 370 );
	panelButtons.push_back( "lbl_speed" );
	
	// h: 400 - Frame selection
	editorUi.addElement( "btn_prev_frame", new Button( "font0", "<" ) );
	editorUi.getElement( "btn_prev_frame" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_prev_frame" )->getBox().getOrigin().move( currentScreenWidth - 290, 400 );
	editorUi.getElement( "btn_prev_frame" )->addEventHandler( "mouseup", prevFrame );
	panelButtons.push_back( "btn_prev_frame" );
	
	editorUi.addElement( "btn_del_frame", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_del_frame" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_del_frame" )->getBox().getOrigin().move( currentScreenWidth - 245, 400 );
	editorUi.getElement( "btn_del_frame" )->addEventHandler( "mouseup", deleteFrame );
	panelButtons.push_back( "btn_del_frame" );
	
	editorUi.addElement( "btn_add_frame", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_add_frame" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_add_frame" )->getBox().getOrigin().move( currentScreenWidth - 95, 400 );
	editorUi.getElement( "btn_add_frame" )->addEventHandler( "mouseup", addFrame );
	panelButtons.push_back( "btn_add_frame" );
	
	editorUi.addElement( "btn_next_frame", new Button( "font0", ">" ) );
	editorUi.getElement( "btn_next_frame" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_next_frame" )->getBox().getOrigin().move( currentScreenWidth - 50, 400 );
	editorUi.getElement( "btn_next_frame" )->addEventHandler( "mouseup", nextFrame );
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
					if( lastEvent.button == lastEvent.button == SDL_BUTTON_LEFT )
					{
						if( !editorUi.dispatchEvent( &lastEvent ) && lastEvent.button.x < currentScreenWidth - 300 )
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
					}
					else if( lastEvent.button == SDL_BUTTON_RIGHT )
					{
						if( toolActive )
							toolActive = false;
					}
						
					break;
				}
				
				case SDL_MOUSEBUTTONUP:
				{
					if( lastEvent.button == SDL_BUTTON_LEFT )
					{
						if!editorUi.dispatchEvent( &lastEvent ) )
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
										Animation * animation = animations[animationsNames[currentAnimation]];
										
										if( animation != NULL )
										{
											//TODO: Test if frame exist or not!!! ok for the frame but not for other boxes
											animation->getFrameByIndex( currentFrame ).getBox().getOrigin().move( revertZoom( toolBox.getOrigin().getX() - origin.getX() ), revertZoom( toolBox.getOrigin().getY() - origin.getY() ) );
											animation->getFrameByIndex( currentFrame ).getBox().resize( revertZoom( toolBox.getWidth() ), revertZoom( toolBox.getHeight() ) );
										}
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
					loadingState = false;
					
					SDL_EventState( SDL_DROPFILE, SDL_DISABLE );
					break;
				}
				
				case SDL_KEYDOWN:
				{
					switch( lastEvent.key.keysym.sym )
					{
						case SDLK_LCTRL:
						case SDLK_RCTRL:
						{
							if( lastEvent.key.repeat == 0 )
								ctrlKeyState = true;
								
							break;
						}
						
						case SDLK_LALT:
						case SDLK_RALT:
						{
							if( lastEvent.key.repeat == 0 )
								altKeyState = true;
								
							break;
						}
						
						case SDLK_LSHIFT:
						case SDLK_RSHIFT:
						{
							if( lastEvent.key.repeat == 0 )
								shiftKeyState = true;

							break;
						}
						
						case SDLK_LEFT:
						{
							if( currentTool.compare( "move" ) == 0 )
								origin.moveBy( -1 * ( shiftKeyState ? 10 : 1 ), 0 );

							else if( currentTool.compare( "anchor" ) == 0 )
							{
							}
							else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
							{
								Animation * animation = animations[animationsNames[currentAnimation]];
								
								if( animation != NULL )
								{
									if( currentTool.compare( "box.frame" ) == 0 )
									{
										// Move action
										if( !altKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().getOrigin().moveBy( -1 * ( shiftKeyState ? 10 : 1 ), 0 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().resizeBy( shiftKeyState ? 10 : 1, 0 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().resizeBy( shiftKeyState ? -10 : -1, 0 );
										
										// TODO: Limit frame to be in the sprite area
										
									}
									else if( currentTool.compare( "box.bounding" ) == 0 )
									{
									}
									else if( currentTool.compare( "box.attack" ) == 0 )
									{
									}
									else if( currentTool.compare( "box.defence" ) == 0 )
									{
									}
								}
							}
							
							break;
						}
						
						case SDLK_RIGHT:
						{
							if( currentTool.compare( "move" ) == 0 )
								origin.moveBy( shiftKeyState ? 10 : 1, 0 );

							else if( currentTool.compare( "anchor" ) == 0 )
							{
							}
							else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
							{
								Animation * animation = animations[animationsNames[currentAnimation]];
								
								if( animation != NULL )
								{
									if( currentTool.compare( "box.frame" ) == 0 )
									{
										// Move action
										if( !ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().getOrigin().moveBy( shiftKeyState ? 10 : 1, 0 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().resizeBy( shiftKeyState ? 10 : 1, 0 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().resizeBy( shiftKeyState ? -10 : -1, 0 );
										
										// TODO: Limit frame to be in the sprite area
									}
									else if( currentTool.compare( "box.bounding" ) == 0 )
									{
									}
									else if( currentTool.compare( "box.attack" ) == 0 )
									{
									}
									else if( currentTool.compare( "box.defence" ) == 0 )
									{
									}
								}
							}
							
							break;
						}
						
						case SDLK_UP:
						{
							if( currentTool.compare( "move" ) == 0 )
								origin.moveBy( 0, -1 * ( shiftKeyState ? 10 : 1 ) );

							else if( currentTool.compare( "anchor" ) == 0 )
							{
							}
							else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
							{
								Animation * animation = animations[animationsNames[currentAnimation]];
								
								if( animation != NULL )
								{
									if( currentTool.compare( "box.frame" ) == 0 )
									{
										// Move action
										if( !altKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().getOrigin().moveBy( 0, -1 * ( shiftKeyState ? 10 : 1 ) );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().resizeBy( 0, shiftKeyState ? 10 : 1 );
											
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().resizeBy( 0, shiftKeyState ? -10 : -1 );
											
										// TODO: Limit frame to be in the sprite area
									}
									else if( currentTool.compare( "box.bounding" ) == 0 )
									{
									}
									else if( currentTool.compare( "box.attack" ) == 0 )
									{
									}
									else if( currentTool.compare( "box.defence" ) == 0 )
									{
									}
								}
							}
							
							break;
						}
						
						case SDLK_DOWN:
						{
							if( currentTool.compare( "move" ) == 0 )
								origin.moveBy( 0, shiftKeyState ? 10 : 1 );

							else if( currentTool.compare( "anchor" ) == 0 )
							{
							}
							else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
							{
								Animation * animation = animations[animationsNames[currentAnimation]];
								
								if( animation != NULL )
								{
									if( currentTool.compare( "box.frame" ) == 0 )
									{
										// Move action
										if( !ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().getOrigin().moveBy( 0, shiftKeyState ? 10 : 1 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().resizeBy( 0, shiftKeyState ? 10 : 1 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getBox().resizeBy( 0, shiftKeyState ? -10 : -1 );

										// TODO: Limit frame to be in the sprite area
									}
									else if( currentTool.compare( "box.bounding" ) == 0 )
									{
									}
									else if( currentTool.compare( "box.attack" ) == 0 )
									{
									}
									else if( currentTool.compare( "box.defence" ) == 0 )
									{
									}
								}
							}
							
							break;
						}
					}
					
					break;
				}
				
				case SDL_KEYUP:
				{
					switch( lastEvent.key.keysym.sym )
					{
						case SDLK_LCTRL:
						case SDLK_RCTRL:
						{
							ctrlKeyState = false;
							break;
						}
						
						case SDLK_LALT:
						case SDLK_RALT:
						{
							altKeyState = false;
							break;
						}
						
						case SDLK_LSHIFT:
						case SDLK_RSHIFT:
						{
							shiftKeyState = false;
							break;
						}
						
						case SDLK_m:
						{
							// ChangeTool to "move"
							editorUi.getElement( "btn_move" )->trigger( "mouseup" );
							break;
						}
						
						case SDLK_f:
						{
							// ChangeTool to "frame"
							editorUi.getElement( "lbl_frame" )->trigger( "mouseup" );
							break;
						}
						
						case SDLK_p:
						{
							// ChangeTool to "anchor point"
							editorUi.getElement( "btn_anchor" )->trigger( "mouseup" );
							break;
						}
						
						case SDLK_b:
						{
							// ChangeTool to "bounding box"
							editorUi.getElement( "lbl_boundingbox" )->trigger( "mouseup" );
							break;
						}
						
						case SDLK_a:
						{
							// ChangeTool to "attack area"
							editorUi.getElement( "lbl_attackarea" )->trigger( "mouseup" );
							break;
						}
						
						case SDLK_d:
						{
							// ChangeTool to "defence area"
							editorUi.getElement( "lbl_defencearea" )->trigger( "mouseup" );
							break;
						}
					}
					
					break;
				}
				
				case SDL_MOUSEWHEEL:
				{
					if( altKeyState )
					{
						if( lastEvent.wheel.y > 0 )
						{
							currentZoom += 5;
							
							synchronizeLabels();
						}
						else if( lastEvent.wheel.y < 0 )
						{
							currentZoom -= 5;
							if( currentZoom < 5 )
								currentZoom = 5;
							
							synchronizeLabels();
						}
					}
					
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
				Box spriteBox( origin, applyZoom( sprite->getWidth() ), applyZoom( sprite->getHeight() ) );
				spriteBox.fillSDLRect( &dstRect );
				
				SDL_RenderCopy( Screen::get()->getRenderer(), sprite->getTexture(), &srcRect, &dstRect );
				spriteBox.render( Color( 0xAA, 0xAA, 0xAA ) );
			
				// Render current animation frames
				Animation * cAnimation = animations[animationsNames[currentAnimation]];
				
				for( unsigned int i = 0 ; i < cAnimation->getFrameCount() ; i++ )
				{
					Box fBox( cAnimation->getFrameByIndex( i ).getBox() );
					fBox.getOrigin().move( origin.getX() + applyZoom( fBox.getOrigin().getX() ), origin.getY() + applyZoom( fBox.getOrigin().getY() ) );
					fBox.resize( applyZoom( fBox.getWidth() ), applyZoom( fBox.getHeight() ) );
					fBox.render( Color( 0xFF, 0xFF, 0xFF ) );
					
					if( currentFrame == i )
					{
						if( !toolActive && currentTool.compare( "box.frame" ) == 0 )
							renderBoxInformation( fBox, origin );
						
						// Render anchor
						Point aPt( cAnimation->getFrameByIndex( i ).getAnchor() );
						aPt.move( origin.getX() + applyZoom( fBox.getOrigin().getX() ) + applyZoom( aPt.getX() ), origin.getY() + applyZoom( fBox.getOrigin().getY() ) + applyZoom( aPt.getY() ) );
						aPt.render( Color( 0x00, 0xFF, 0x00 ), 5 * currentZoom / 100 );
						
						if( !toolActive && currentTool.compare( "anchor" ) == 0 )
							renderPointInformation( aPt, fBox.getOrigin() );
						
						// Render bounding boxes
						for( unsigned int iBox = 0 ; i < cAnimation->getFrameByIndex( i ).getBoundingBoxesCount() ; iBox++ )
						{
							Box bBox( cAnimation->getFrameByIndex( i ).getBoundingBox( iBox ) );
							bBox.getOrigin().move( origin.getX() + fBox.getOrigin().getX() + applyZoom( bBox.getOrigin().getX() ), origin.getY() + fBox.getOrigin().getY() + applyZoom( bBox.getOrigin().getY() ) );
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
							aBox.getOrigin().move( origin.getX() + fBox.getOrigin().getX() + applyZoom( aBox.getOrigin().getX() ), origin.getY() + fBox.getOrigin().getY() + applyZoom( aBox.getOrigin().getY() ) );
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
							dBox.getOrigin().move( origin.getX() + fBox.getOrigin().getX() + applyZoom( dBox.getOrigin().getX() ), origin.getY() + fBox.getOrigin().getY() + applyZoom( dBox.getOrigin().getY() ) );
							dBox.render( Color( 0x00, 0x00, 0xFF ) );
							
							if( iDefence == currentDefenceArea )
							{
								if( !toolActive && currentTool.compare( "box.defence" ) == 0 )
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
					renderPointInformation( toolBox.getOrigin(), origin );
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
			Box animationBackground( currentScreenWidth - 300, 0, 300, 300 );
			
			actionPanelBorder.resizeBy( 1, 2 );
			actionPanelBorder.getOrigin().moveBy( 0, -1 );
			actionPanel.renderFilled( Color( 0xEE, 0xEE, 0xEE ) );
			animationBackground.renderFilled( Color( 0xFF, 0xFF, 0xFF ) );
			actionPanel.render( Color( 0xAA, 0xAA, 0xAA ) );
			
			editorUi.render( ticks );
			
			// Render animation preview
			if( !loadingState && !savingState && animations.size() > 0 )
			{
				Frame * frame = animations[animationsNames[currentAnimation]]->getFrame( ticks );
			
				if( frame != NULL )
				{
					int animationZoom = frame->getBox().getWidth() > frame->getBox().getHeight() ? static_cast<int>( 300.0 / static_cast<double>( frame->getBox().getWidth() ) * 100.0 ) : static_cast<int>( 300.0 / static_cast<double>( frame->getBox().getHeight() ) * 100.0 );

					SDL_Rect dstRect;
				
					dstRect.x = currentScreenWidth - 300;//applyZoom( frame->getAnchor().getX(), animationZoom );
					dstRect.y = 0;//applyZoom( frame->getAnchor().getY(), animationZoom );
					dstRect.w = applyZoom( frame->getBox().getWidth(), animationZoom );
					dstRect.h = applyZoom( frame->getBox().getHeight(), animationZoom );

					SDL_Rect srcRect;
					frame->getBox().fillSDLRect( &srcRect );
	
					SDL_RenderCopy( Screen::get()->getRenderer(), sprite->getTexture(), &srcRect, &dstRect );
					
					// TODO: Compute local zoom based on each animation frame (get the max width & max height & take part of anchor point)
					/*int frameZoom = 100;
					SDL_Rect dstRect;
					int anchorX = origin.getX();
					int anchorY = origin.getY();
				
					dstRect.x = anchorX - (frame->getAnchor().getX() * animationZoom / 100.0);
					dstRect.y = anchorY - (frame->getAnchor().getY() * animationZoom / 100.0);
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
	currentFrame = 0;
	currentBoundingBox = 0;
	currentAttackArea = 0;
	currentDefenceArea = 0;
	currentZoom = 100;
	
	synchronizeLabels();
}

int applyZoom( int value, int zoom )
{
	return static_cast<int>( round( static_cast<double>( value ) * static_cast<double>( zoom == 0 ? currentZoom : zoom ) / 100.0 ) );
}

int revertZoom( int value, int zoom )
{
	return static_cast<int>( round( static_cast<double>( value ) * 100.0 / static_cast<double>( zoom == 0 ? currentZoom : zoom ) ) );
}

void renderPointInformation( Point& point, Point& relative )
{
	int infoWidth = 0;
	int infoHeight = 0;
	
	stringstream pointInfo;
	pointInfo << "x: " << revertZoom( point.getX() - relative.getX() ) << " y: " << revertZoom( point.getY() - relative.getY() );
	Font::get( "font0" )->renderSize( &infoWidth, &infoHeight, pointInfo.str() );
	Font::get( "font0" )->render( point.getX() - infoWidth / 2, point.getY() + - infoHeight, pointInfo.str() );
}

void renderBoxInformation( Box& box, Point& relative )
{
	int infoWidth = 0;
	int infoHeight = 0;
	
	stringstream boxInfo;
	boxInfo << "x: " << revertZoom( box.getOrigin().getX() - relative.getX() ) << " y: " << revertZoom( box.getOrigin().getY() - relative.getY() );
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
	
	synchronizeLabels();
	origin.move( (availableWidth - applyZoom( sprite->getWidth() )) / 2, (availableHeight - applyZoom( sprite->getHeight() )) / 2 );
}

void synchronizeLabel( const string& name, const string& value )
{
	Label * label = reinterpret_cast<Label *>( editorUi.getElement( name ) );
	label->setValue( value );
}

void synchronizeLabels()
{
	stringstream ss;
	
	synchronizeLabel( "lbl_animation", animationsNames[currentAnimation] );
	
	ss << "Frame#" << currentFrame;
	synchronizeLabel( "lbl_frame", ss.str() );

	ss.str( "" );
	ss << "Bounding#" << currentBoundingBox;
	synchronizeLabel( "lbl_boundingbox", ss.str() );
	
	ss.str( "" );
	ss << "Attack#" << currentAttackArea;
	synchronizeLabel( "lbl_attackarea", ss.str() );
	
	ss.str( "" );
	ss << "Defence#" << currentDefenceArea;
	synchronizeLabel( "lbl_defencearea", ss.str() );
	
	ss.str( "" );
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
		ss << animations[animationsNames[currentAnimation]]->getSpeed() << " fps";
	else
		ss << "-- fps";
		
	synchronizeLabel( "lbl_speed", ss.str() );

	ss.str( "" );
	ss << currentZoom << " %";
	synchronizeLabel( "lbl_zoom", ss.str() );
}

bool decreaseZoom( Element * element )
{
	currentZoom -= 5;
	
	if( currentZoom <= 0 )
		currentZoom = 5;
	
	synchronizeLabels();
	return true;
}

bool increaseZoom( Element * element )
{
	currentZoom += 5;
	synchronizeLabels();
	return true;
}

bool loadFile( Element * element )
{
	for( vector<string>::iterator it = panelButtons.begin() ; it != panelButtons.end() ; it++ )
		editorUi.hideElement( *it );
	
	loadingState = true;
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
	if( currentAnimation == 0 )
		currentAnimation = animationsNames.size() - 1;
	else
		currentAnimation--;
		
	currentFrame = 0;
	currentBoundingBox = 0;
	currentAttackArea = 0;
	currentDefenceArea = 0;
	
	synchronizeLabels();
	return true;
}

bool nextAnimation( Element * element )
{
	currentAnimation++;
	
	if( currentAnimation >= animationsNames.size() )
		currentAnimation = 0;
	
	currentFrame = 0;
	currentBoundingBox = 0;
	currentAttackArea = 0;
	currentDefenceArea = 0;
		
	synchronizeLabels();
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
	loadingState = false;
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
			animations[*it]->setSpeed( 100 );
			animations[*it]->addFrame( frame );
		}
		
		synchronizeLabels();
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

// Speed Events
bool decreaseSpeed( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		if( animation->getSpeed() > 5 )
			animation->setSpeed( animation->getSpeed() - 5 );
		else
			animation->setSpeed( 0 );
			
		synchronizeLabels();
	}
	
	return true;
}

bool increaseSpeed( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		animation->setSpeed( animation->getSpeed() + 5 );		
		synchronizeLabels();
	}
	
	return true;
}

// Frame Events
bool addFrame( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL && sprite != NULL )
	{
		Frame frame( animation->getFrameByIndex( animation->getFrameCount() - 1 ) );
		
		if( frame.getBox().getWidth() + frame.getBox().getOrigin().getX() <= sprite->getWidth() )
			frame.getBox().getOrigin().moveBy( frame.getBox().getWidth(), 0 );
		else if( frame.getBox().getHeight() + frame.getBox().getOrigin().getY() <= sprite->getHeight() )
			frame.getBox().getOrigin().moveBy( -1 * frame.getBox().getOrigin().getX(), frame.getBox().getHeight() );
			
		animation->addFrame( frame );
		currentFrame = animation->getFrameCount() - 1;
		
		synchronizeLabels();
	}
	
	return true;
}

bool deleteFrame( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL && animation->getFrameCount() > 1 )
	{
		animation->removeFrameByIndex( currentFrame );
		currentFrame = animation->getFrameCount() - 1;
		synchronizeLabels();
	}
	
	return true;
}

bool nextFrame( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL  )
	{
		currentFrame++;
		
		if( currentFrame >= animation->getFrameCount() )
			currentFrame = 0;
			
		synchronizeLabels();
	}
	
	return true;
}

bool prevFrame( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL  )
	{
		if( currentFrame == 0 )
			currentFrame = animation->getFrameCount() - 1;
		else
			currentFrame--;
			
		synchronizeLabels();
	}
	
	return true;
}

