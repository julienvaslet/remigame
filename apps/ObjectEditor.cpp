#include <SDL2/SDL.h>

#include <data/parser/NodeParser.h>

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
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>

using namespace graphics;
using namespace ui;
using namespace std;
using namespace data;

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
void initUserInterface();
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
void restrictPointToBoxArea( Point& point, Box& area );
void restrictBoxToBoxArea( Box& box, Box& area );

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

// Bounding Box Events
bool addBoundingBox( Element * element );
bool deleteBoundingBox( Element * element );
bool nextBoundingBox( Element * element );
bool prevBoundingBox( Element * element );

// Attack Area Events
bool addAttackArea( Element * element );
bool deleteAttackArea( Element * element );
bool nextAttackArea( Element * element );
bool prevAttackArea( Element * element );

// Defence Area Events
bool addDefenceArea( Element * element );
bool deleteDefenceArea( Element * element );
bool nextDefenceArea( Element * element );
bool prevDefenceArea( Element * element );

// File Events
bool loadFile( Element * element );
bool cancelLoading( Element * element );
bool saveFile( Element * element );

// Animation Events
bool prevAnimation( Element * element );
bool nextAnimation( Element * element );

// TODO: add z-index value to UserInterface element, and a tab index too
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
	
	initUserInterface();
	
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
							// Restrict to the current frame
							Animation * animation = animations[animationsNames[currentAnimation]];
							
							if( animation != NULL )
							{
								toolBox.getOrigin().move( revertZoom( lastEvent.button.x - origin.getX() ) - animation->getFrameByIndex( currentFrame ).getBox().getOrigin().getX(), revertZoom( lastEvent.button.y - origin.getY() ) - animation->getFrameByIndex( currentFrame ).getBox().getOrigin().getY() );
								restrictPointToBoxArea( toolBox.getOrigin(), animation->getFrameByIndex( currentFrame ).getBox() );
							}
						}
						else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
						{
							Animation * animation = animations[animationsNames[currentAnimation]];
							
							if( animation != NULL )
							{
								if( currentTool.compare( "box.frame" ) == 0 )
								{
									toolBox.resize( revertZoom( lastEvent.motion.x - origin.getX() ) - toolBox.getOrigin().getX(), revertZoom( lastEvent.motion.y - origin.getY() ) - toolBox.getOrigin().getY() );
									
									// Restrict to the current sprite
									if( sprite != NULL )
									{
										Box spriteBox( 0, 0, sprite->getWidth(), sprite->getHeight() );
										restrictBoxToBoxArea( toolBox, spriteBox );
									}
								}
							
								else if( currentTool.compare( "box.bounding" ) == 0
									  || currentTool.compare( "box.attack" ) == 0
									  || currentTool.compare( "box.defence" ) == 0 )
								{
									toolBox.resize( revertZoom( lastEvent.motion.x - origin.getX() ) - toolBox.getOrigin().getX() - animation->getFrameByIndex( currentFrame ).getBox().getOrigin().getX(), revertZoom( lastEvent.motion.y - origin.getY() ) - toolBox.getOrigin().getY() - animation->getFrameByIndex( currentFrame ).getBox().getOrigin().getY() );
									
									// Restrict to the current frame
									restrictBoxToBoxArea( toolBox, animation->getFrameByIndex( currentFrame ).getBox() );
								}
							}
						}
					}

					break;
				}
				
				case SDL_MOUSEBUTTONDOWN:
				{
					if( lastEvent.button.button == SDL_BUTTON_LEFT )
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
								// Restrict to the current frame
								Animation * animation = animations[animationsNames[currentAnimation]];
								
								if( animation != NULL )
								{
									toolBox.getOrigin().move( revertZoom( lastEvent.button.x - origin.getX() ) - animation->getFrameByIndex( currentFrame ).getBox().getOrigin().getX(), revertZoom( lastEvent.button.y - origin.getY() ) - animation->getFrameByIndex( currentFrame ).getBox().getOrigin().getY() );
									restrictPointToBoxArea( toolBox.getOrigin(), animation->getFrameByIndex( currentFrame ).getBox() );
									
									toolActive = true;
								}
							}
							else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
							{
								toolBox.getOrigin().move( revertZoom( lastEvent.button.x - origin.getX() ), revertZoom( lastEvent.button.y - origin.getY() ) );
								toolBox.resize( 0, 0 );
								toolActive = true;
								
								if( currentTool.compare( "box.frame" ) == 0 )
								{
									
									// Restrict to the current sprite
									if( sprite != NULL )
									{
										Box spriteBox( 0, 0, sprite->getWidth(), sprite->getHeight() );
										restrictBoxToBoxArea( toolBox, spriteBox );
									}
								}
								
								else if( currentTool.compare( "box.bounding" ) == 0
								      || currentTool.compare( "box.attack" ) == 0
								      || currentTool.compare( "box.defence" ) == 0 )
								{
									// Restrict to the current frame
									Animation * animation = animations[animationsNames[currentAnimation]];
									
									if( animation != NULL )
									{
										toolBox.getOrigin().moveBy( -1 * animation->getFrameByIndex( currentFrame ).getBox().getOrigin().getX(), -1 * animation->getFrameByIndex( currentFrame ).getBox().getOrigin().getY() );
										restrictPointToBoxArea( toolBox.getOrigin(), animation->getFrameByIndex( currentFrame ).getBox() );
									}
								}
							}
						}
					}
					else if( lastEvent.button.button == SDL_BUTTON_RIGHT )
					{
						if( toolActive )
							toolActive = false;
					}
						
					break;
				}
				
				case SDL_MOUSEBUTTONUP:
				{
					if( lastEvent.button.button == SDL_BUTTON_LEFT )
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
								
									Animation * animation = animations[animationsNames[currentAnimation]];
									
									if( animation != NULL )
										animation->getFrameByIndex( currentFrame ).getAnchor().move( toolBox.getOrigin().getX(), toolBox.getOrigin().getY() );
								}
								else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
								{
									toolActive = false;
									
									// Negative width & height cases
									if( toolBox.getWidth() < 0 )
									{
										toolBox.getOrigin().moveBy( toolBox.getWidth() - 1, 0 );
										toolBox.setWidth( -1 * toolBox.getWidth() );
									}
									
									if( toolBox.getHeight() < 0 )
									{
										toolBox.getOrigin().moveBy( 0, toolBox.getHeight() - 1 );
										toolBox.setHeight( -1 * toolBox.getHeight() );
									}
									
									if( currentTool.compare( "box.frame" ) == 0 )
									{
										// Set the frame
										Animation * animation = animations[animationsNames[currentAnimation]];
										
										if( animation != NULL )
										{
											animation->getFrameByIndex( currentFrame ).getBox().getOrigin().move( toolBox.getOrigin().getX(), toolBox.getOrigin().getY() );
											animation->getFrameByIndex( currentFrame ).getBox().resize( toolBox.getWidth(), toolBox.getHeight() );
										}
									}
									else if( currentTool.compare( "box.bounding" ) == 0 )
									{
										// Set the bounding box
										Animation * animation = animations[animationsNames[currentAnimation]];
										
										if( animation != NULL )
										{
											if( currentBoundingBox < animation->getFrameByIndex( currentFrame ).getBoundingBoxesCount() )
											{
												animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).getOrigin().move( toolBox.getOrigin().getX(), toolBox.getOrigin().getY() );
												animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).resize( toolBox.getWidth(), toolBox.getHeight() );
											}
											else
											{
												animation->getFrameByIndex( currentFrame ).addBoundingBox( toolBox );
												currentBoundingBox = animation->getFrameByIndex( currentFrame ).getBoundingBoxesCount() - 1;
												synchronizeLabels();
											}
										}
									}
									else if( currentTool.compare( "box.attack" ) == 0 )
									{
										// Set the attack area
										Animation * animation = animations[animationsNames[currentAnimation]];
										
										if( animation != NULL )
										{
											if( currentAttackArea < animation->getFrameByIndex( currentFrame ).getAttackAreasCount() )
											{
												animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).getOrigin().move( toolBox.getOrigin().getX(), toolBox.getOrigin().getY() );
												animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).resize( toolBox.getWidth(), toolBox.getHeight() );
											}
											else
											{
												animation->getFrameByIndex( currentFrame ).addAttackArea( toolBox );
												currentAttackArea = animation->getFrameByIndex( currentFrame ).getAttackAreasCount() - 1;
												synchronizeLabels();
											}
										}
									}
									else if( currentTool.compare( "box.defence" ) == 0 )
									{
										// Set the defence area
										Animation * animation = animations[animationsNames[currentAnimation]];
										
										if( animation != NULL )
										{
											if( currentDefenceArea < animation->getFrameByIndex( currentFrame ).getDefenceAreasCount() )
											{
												animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).getOrigin().move( toolBox.getOrigin().getX(), toolBox.getOrigin().getY() );
												animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).resize( toolBox.getWidth(), toolBox.getHeight() );
											}
											else
											{
												animation->getFrameByIndex( currentFrame ).addDefenceArea( toolBox );
												currentDefenceArea = animation->getFrameByIndex( currentFrame ).getDefenceAreasCount() - 1;
												synchronizeLabels();
											}
										}
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
								Animation * animation = animations[animationsNames[currentAnimation]];
								
								if( animation != NULL )
								{
									animation->getFrameByIndex( currentFrame ).getAnchor().moveBy( -1 * ( shiftKeyState ? 10 : 1 ), 0 );
									restrictPointToBoxArea( animation->getFrameByIndex( currentFrame ).getAnchor(), animation->getFrameByIndex( currentFrame ).getBox() );
								}
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
										
										Box spriteBox( 0, 0, sprite->getWidth(), sprite->getHeight() );
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getBox(), spriteBox );
										
									}
									else if( currentTool.compare( "box.bounding" ) == 0 )
									{
										// Move action
										if( !altKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).getOrigin().moveBy( -1 * ( shiftKeyState ? 10 : 1 ), 0 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).resizeBy( shiftKeyState ? 10 : 1, 0 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).resizeBy( shiftKeyState ? -10 : -1, 0 );
										
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ), animation->getFrameByIndex( currentFrame ).getBox() );
									}
									else if( currentTool.compare( "box.attack" ) == 0 )
									{
										// Move action
										if( !altKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).getOrigin().moveBy( -1 * ( shiftKeyState ? 10 : 1 ), 0 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).resizeBy( shiftKeyState ? 10 : 1, 0 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).resizeBy( shiftKeyState ? -10 : -1, 0 );
										
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ), animation->getFrameByIndex( currentFrame ).getBox() );
									}
									else if( currentTool.compare( "box.defence" ) == 0 )
									{
										// Move action
										if( !altKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).getOrigin().moveBy( -1 * ( shiftKeyState ? 10 : 1 ), 0 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).resizeBy( shiftKeyState ? 10 : 1, 0 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).resizeBy( shiftKeyState ? -10 : -1, 0 );
										
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ), animation->getFrameByIndex( currentFrame ).getBox() );
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
								Animation * animation = animations[animationsNames[currentAnimation]];
								
								if( animation != NULL )
								{
									animation->getFrameByIndex( currentFrame ).getAnchor().moveBy( shiftKeyState ? 10 : 1, 0 );
									restrictPointToBoxArea( animation->getFrameByIndex( currentFrame ).getAnchor(), animation->getFrameByIndex( currentFrame ).getBox() );
								}
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
										
										Box spriteBox( 0, 0, sprite->getWidth(), sprite->getHeight() );
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getBox(), spriteBox );
									}
									else if( currentTool.compare( "box.bounding" ) == 0 )
									{
										// Move action
										if( !ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).getOrigin().moveBy( shiftKeyState ? 10 : 1, 0 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).resizeBy( shiftKeyState ? 10 : 1, 0 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).resizeBy( shiftKeyState ? -10 : -1, 0 );
										
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ), animation->getFrameByIndex( currentFrame ).getBox() );
									}
									else if( currentTool.compare( "box.attack" ) == 0 )
									{
										// Move action
										if( !ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).getOrigin().moveBy( shiftKeyState ? 10 : 1, 0 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).resizeBy( shiftKeyState ? 10 : 1, 0 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).resizeBy( shiftKeyState ? -10 : -1, 0 );
										
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ), animation->getFrameByIndex( currentFrame ).getBox() );
									}
									else if( currentTool.compare( "box.defence" ) == 0 )
									{
										// Move action
										if( !ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).getOrigin().moveBy( shiftKeyState ? 10 : 1, 0 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).resizeBy( shiftKeyState ? 10 : 1, 0 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).resizeBy( shiftKeyState ? -10 : -1, 0 );
										
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ), animation->getFrameByIndex( currentFrame ).getBox() );
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
								Animation * animation = animations[animationsNames[currentAnimation]];
								
								if( animation != NULL )
								{
									animation->getFrameByIndex( currentFrame ).getAnchor().moveBy( 0, -1 * ( shiftKeyState ? 10 : 1 ) );
									restrictPointToBoxArea( animation->getFrameByIndex( currentFrame ).getAnchor(), animation->getFrameByIndex( currentFrame ).getBox() );
								}
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
										
										Box spriteBox( 0, 0, sprite->getWidth(), sprite->getHeight() );
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getBox(), spriteBox );
									}
									else if( currentTool.compare( "box.bounding" ) == 0 )
									{
										// Move action
										if( !altKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).getOrigin().moveBy( 0, -1 * ( shiftKeyState ? 10 : 1 ) );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).resizeBy( 0, shiftKeyState ? 10 : 1 );
											
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).resizeBy( 0, shiftKeyState ? -10 : -1 );
										
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ), animation->getFrameByIndex( currentFrame ).getBox() );
									}
									else if( currentTool.compare( "box.attack" ) == 0 )
									{
										// Move action
										if( !altKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).getOrigin().moveBy( 0, -1 * ( shiftKeyState ? 10 : 1 ) );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).resizeBy( 0, shiftKeyState ? 10 : 1 );
											
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).resizeBy( 0, shiftKeyState ? -10 : -1 );
										
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ), animation->getFrameByIndex( currentFrame ).getBox() );
									}
									else if( currentTool.compare( "box.defence" ) == 0 )
									{
										// Move action
										if( !altKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).getOrigin().moveBy( 0, -1 * ( shiftKeyState ? 10 : 1 ) );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).resizeBy( 0, shiftKeyState ? 10 : 1 );
											
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).resizeBy( 0, shiftKeyState ? -10 : -1 );
										
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ), animation->getFrameByIndex( currentFrame ).getBox() );
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
								Animation * animation = animations[animationsNames[currentAnimation]];
								
								if( animation != NULL )
								{
									animation->getFrameByIndex( currentFrame ).getAnchor().moveBy( 0, shiftKeyState ? 10 : 1 );
									restrictPointToBoxArea( animation->getFrameByIndex( currentFrame ).getAnchor(), animation->getFrameByIndex( currentFrame ).getBox() );
								}
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

										Box spriteBox( 0, 0, sprite->getWidth(), sprite->getHeight() );
										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getBox(), spriteBox );
									}
									else if( currentTool.compare( "box.bounding" ) == 0 )
									{
										// Move action
										if( !ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).getOrigin().moveBy( 0, shiftKeyState ? 10 : 1 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).resizeBy( 0, shiftKeyState ? 10 : 1 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ).resizeBy( 0, shiftKeyState ? -10 : -1 );

										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getBoundingBox( currentBoundingBox ), animation->getFrameByIndex( currentFrame ).getBox() );
									}
									else if( currentTool.compare( "box.attack" ) == 0 )
									{
										// Move action
										if( !ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).getOrigin().moveBy( 0, shiftKeyState ? 10 : 1 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).resizeBy( 0, shiftKeyState ? 10 : 1 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ).resizeBy( 0, shiftKeyState ? -10 : -1 );

										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getAttackArea( currentAttackArea ), animation->getFrameByIndex( currentFrame ).getBox() );
									}
									else if( currentTool.compare( "box.defence" ) == 0 )
									{
										// Move action
										if( !ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).getOrigin().moveBy( 0, shiftKeyState ? 10 : 1 );
									
										// Resize + action
										if( ctrlKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).resizeBy( 0, shiftKeyState ? 10 : 1 );
										
										// Resize - action
										else if( altKeyState )
											animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ).resizeBy( 0, shiftKeyState ? -10 : -1 );

										restrictBoxToBoxArea( animation->getFrameByIndex( currentFrame ).getDefenceArea( currentDefenceArea ), animation->getFrameByIndex( currentFrame ).getBox() );
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
						//int prevZoom = currentZoom;
						int mouseX = 0;
						int mouseY = 0;
						SDL_GetMouseState( &mouseX, &mouseY );
						
						if( lastEvent.wheel.y > 0 )
						{
							currentZoom += 5;
							
							//origin.moveBy( static_cast<int>( static_cast<double>( mouseX - origin.getX() ) * -5.0 / 100.0 ), static_cast<int>( static_cast<double>( mouseY - origin.getY() ) * -1.0 * static_cast<double>( currentZoom - prevZoom ) / 100.0 ) );
							
							synchronizeLabels();
						}
						else if( lastEvent.wheel.y < 0 )
						{
							currentZoom -= 5;
							if( currentZoom < 5 )
								currentZoom = 5;
								
							//origin.moveBy( static_cast<int>( static_cast<double>( mouseX - origin.getX() ) * -5.0 / 100.0 ), static_cast<int>( static_cast<double>( mouseY - origin.getY() ) * -1.0 * static_cast<double>( prevZoom - currentZoom ) / 100.0 ) );
							
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
							renderBoxInformation( cAnimation->getFrameByIndex( i ).getBox(), origin );
						
						// Render anchor
						Point aPt( cAnimation->getFrameByIndex( i ).getAnchor() );
						aPt.move( origin.getX() + applyZoom( cAnimation->getFrameByIndex( i ).getBox().getOrigin().getX() + aPt.getX() ), origin.getY() + applyZoom( cAnimation->getFrameByIndex( i ).getBox().getOrigin().getY() + aPt.getY() ) );
						aPt.render( Color( 0x00, 0xFF, 0x00 ), 5 * currentZoom / 100 );
						
						if( !toolActive && currentTool.compare( "anchor" ) == 0 )
							renderPointInformation( cAnimation->getFrameByIndex( i ).getAnchor(), fBox.getOrigin() );
						
						
						// Render bounding boxes
						for( unsigned int iBox = 0 ; iBox < cAnimation->getFrameByIndex( i ).getBoundingBoxesCount() ; iBox++ )
						{
							Box bBox( cAnimation->getFrameByIndex( i ).getBoundingBox( iBox ) );
							bBox.getOrigin().move( origin.getX() + applyZoom( cAnimation->getFrameByIndex( i ).getBox().getOrigin().getX() + bBox.getOrigin().getX() ), origin.getY() + applyZoom( cAnimation->getFrameByIndex( i ).getBox().getOrigin().getY() + bBox.getOrigin().getY() ) );
							bBox.resize( applyZoom( bBox.getWidth() ), applyZoom( bBox.getHeight() ) );
							bBox.render( Color( 0x00, 0xFF, 0x00 ) );
							
							if( iBox == currentBoundingBox )
							{
								if( !toolActive && currentTool.compare( "box.bounding" ) == 0 )
									renderBoxInformation( cAnimation->getFrameByIndex( i ).getBoundingBox( iBox ), fBox.getOrigin() );
							}
						}
						
						// Render attack areas
						for( unsigned int iAttack = 0 ; iAttack < cAnimation->getFrameByIndex( i ).getAttackAreasCount() ; iAttack++ )
						{
							Box aBox( cAnimation->getFrameByIndex( i ).getAttackArea( iAttack ) );
							aBox.getOrigin().move( origin.getX() + applyZoom( cAnimation->getFrameByIndex( i ).getBox().getOrigin().getX() + aBox.getOrigin().getX() ), origin.getY() + applyZoom( cAnimation->getFrameByIndex( i ).getBox().getOrigin().getY() + aBox.getOrigin().getY() ) );
							aBox.resize( applyZoom( aBox.getWidth() ), applyZoom( aBox.getHeight() ) );
							aBox.render( Color( 0xFF, 0x00, 0x00 ) );
							
							if( iAttack == currentAttackArea )
							{
								if( !toolActive && currentTool.compare( "box.attack" ) == 0 )
									renderBoxInformation( cAnimation->getFrameByIndex( i ).getAttackArea( iAttack ), fBox.getOrigin() );
							}
						}
						
						// Render defence areas
						for( unsigned int iDefence = 0 ; iDefence < cAnimation->getFrameByIndex( i ).getDefenceAreasCount() ; iDefence++ )
						{
							Box dBox( cAnimation->getFrameByIndex( i ).getDefenceArea( iDefence ) );
							dBox.getOrigin().move( origin.getX() + applyZoom( cAnimation->getFrameByIndex( i ).getBox().getOrigin().getX() + dBox.getOrigin().getX() ), origin.getY() + applyZoom( cAnimation->getFrameByIndex( i ).getBox().getOrigin().getY() + dBox.getOrigin().getY() ) );
							dBox.resize( applyZoom( dBox.getWidth() ), applyZoom( dBox.getHeight() ) );
							dBox.render( Color( 0x00, 0x00, 0xFF ) );
							
							if( iDefence == currentDefenceArea )
							{
								if( !toolActive && currentTool.compare( "box.defence" ) == 0 )
									renderBoxInformation( cAnimation->getFrameByIndex( i ).getDefenceArea( iDefence ), fBox.getOrigin() );
							}
						}
					}
				}
				
				// Render current tool
				if( toolActive )
				{
					if( currentTool.compare( "anchor" ) == 0 )
					{
						Point frameOrigin( origin );
						frameOrigin.moveBy( applyZoom( cAnimation->getFrameByIndex( currentFrame ).getBox().getOrigin().getX() ), applyZoom( cAnimation->getFrameByIndex( currentFrame ).getBox().getOrigin().getY() ) );
						
						renderPointInformation( toolBox.getOrigin(), frameOrigin );
					}
					else if( currentTool.substr( 0, 3 ).compare( "box" ) == 0 )
					{
						Color color;
						Box scaledBox;
						Point relative( origin );
			
						if( currentTool.compare( "box.frame" ) == 0 )
						{
							scaledBox.getOrigin().move( origin.getX() + applyZoom( toolBox.getOrigin().getX() ), origin.getY() + applyZoom( toolBox.getOrigin().getY() ) );
							color.setColor( "FFFFFF" );
						}
						else
						{
							if( currentTool.compare( "box.bounding" ) == 0 ) color.setColor( "00FF00" );
							else if( currentTool.compare( "box.attack" ) == 0 ) color.setColor( "FF0000" );
							else if( currentTool.compare( "box.defence" ) == 0 ) color.setColor( "0000FF" );
							
							scaledBox.getOrigin().move( origin.getX() + applyZoom( cAnimation->getFrameByIndex( currentFrame ).getBox().getOrigin().getX() + toolBox.getOrigin().getX() ), origin.getY() + applyZoom( cAnimation->getFrameByIndex( currentFrame ).getBox().getOrigin().getY() + toolBox.getOrigin().getY() ) );
							relative.moveBy( applyZoom( cAnimation->getFrameByIndex( currentFrame ).getBox().getOrigin().getX() ), applyZoom( cAnimation->getFrameByIndex( currentFrame ).getBox().getOrigin().getY() ) );
						}
						
						scaledBox.resize( applyZoom( toolBox.getWidth() ), applyZoom( toolBox.getHeight() ) );
						scaledBox.render( color );
						renderBoxInformation( toolBox, relative );
					}
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
				Animation * animation = animations[animationsNames[currentAnimation]];
				
				if( animation != NULL )
				{
					int animationZoom = 100;
					Box negativeArea;
					Box positiveArea;
					
					if( animation->getFrameCount() > 0 )
					{
						// Find animation area
						for( unsigned int i = 0 ; i < animation->getFrameCount() ; i++ )
						{
							int width = animation->getFrameByIndex( i ).getBox().getWidth() - animation->getFrameByIndex( i ).getAnchor().getX();
							int height = animation->getFrameByIndex( i ).getAnchor().getY();
						
							// Positive area
							if( width > positiveArea.getWidth() )
								positiveArea.setWidth( width );
								
							if( height > positiveArea.getHeight() )
								positiveArea.setHeight( height );
							
							// Negative area
							width = animation->getFrameByIndex( i ).getAnchor().getX();
							height = animation->getFrameByIndex( i ).getBox().getHeight() - animation->getFrameByIndex( i ).getAnchor().getY();
							
							if( width > negativeArea.getWidth() )
								 negativeArea.setWidth( width );
								 
							if( height > negativeArea.getHeight() )
								negativeArea.setHeight( height );
						}
						
						// Determine the zoom level
						if( positiveArea.getWidth() + negativeArea.getWidth() > positiveArea.getHeight() + negativeArea.getHeight() )
							animationZoom = static_cast<int>( 300.0 / static_cast<double>( positiveArea.getWidth() + negativeArea.getWidth() ) * 100.0 );
						else
							animationZoom = static_cast<int>( 300.0 / static_cast<double>( positiveArea.getHeight() + negativeArea.getHeight() ) * 100.0 );
					}
					
					Frame * frame = animation->getFrame( ticks );
				
					if( frame != NULL )
					{
						SDL_Rect dstRect;
					
						dstRect.x = currentScreenWidth - 300 + applyZoom( negativeArea.getWidth() - frame->getAnchor().getX(), animationZoom );
						dstRect.y = applyZoom( positiveArea.getHeight() - frame->getAnchor().getY(), animationZoom );
						dstRect.w = applyZoom( frame->getBox().getWidth(), animationZoom );
						dstRect.h = applyZoom( frame->getBox().getHeight(), animationZoom );
	
						SDL_Rect srcRect;
						frame->getBox().fillSDLRect( &srcRect );
		
						SDL_RenderCopy( Screen::get()->getRenderer(), sprite->getTexture(), &srcRect, &dstRect );
					
						// Render anchor
						Point fAnchor( frame->getAnchor() );
						fAnchor.move( currentScreenWidth - 300 + applyZoom( negativeArea.getWidth(), animationZoom ), applyZoom( positiveArea.getHeight(), animationZoom ) );
						fAnchor.render( Color( 0x00, 0xFF, 0x00 ), applyZoom( 5, animationZoom ) );
					
						// Render bounding boxes
						for( unsigned int iBox = 0 ; iBox < frame->getBoundingBoxesCount() ; iBox++ )
						{
							Box bBox( frame->getBoundingBox( iBox ) );
							bBox.getOrigin().move( currentScreenWidth - 300 + applyZoom( negativeArea.getWidth() - frame->getAnchor().getX() + bBox.getOrigin().getX(), animationZoom ), applyZoom( positiveArea.getHeight() - frame->getAnchor().getY() + bBox.getOrigin().getY(), animationZoom ) );
							bBox.resize( applyZoom( bBox.getWidth(), animationZoom ), applyZoom( bBox.getHeight(), animationZoom ) );
							bBox.render( Color( 0x00, 0xFF, 0x00 ) );
						}
					
						// Render attack areas
						for( unsigned int iAttack = 0 ; iAttack < frame->getAttackAreasCount() ; iAttack++ )
						{
							Box aBox( frame->getAttackArea( iAttack ) );
							aBox.getOrigin().move( currentScreenWidth - 300 + applyZoom( negativeArea.getWidth() - frame->getAnchor().getX() + aBox.getOrigin().getX(), animationZoom ), applyZoom( positiveArea.getHeight() - frame->getAnchor().getY() + aBox.getOrigin().getY(), animationZoom ) );
							aBox.resize( applyZoom( aBox.getWidth(), animationZoom ), applyZoom( aBox.getHeight(), animationZoom ) );
							aBox.render( Color( 0xFF, 0x00, 0x00 ) );
						}
					
						// Render defence areas
						for( unsigned int iDefence = 0 ; iDefence < frame->getDefenceAreasCount() ; iDefence++ )
						{
							Box dBox( frame->getDefenceArea( iDefence ) );
							dBox.getOrigin().move( currentScreenWidth - 300 + applyZoom( negativeArea.getWidth() - frame->getAnchor().getX() + dBox.getOrigin().getX(), animationZoom ), applyZoom( positiveArea.getHeight() - frame->getAnchor().getY() + dBox.getOrigin().getY(), animationZoom ) );
							dBox.resize( applyZoom( dBox.getWidth(), animationZoom ), applyZoom( dBox.getHeight(), animationZoom ) );
							dBox.render( Color( 0x00, 0x00, 0xFF ) );
						}
					}
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

void initUserInterface()
{
	int currentScreenWidth = Screen::get()->getWidth();
	
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
	editorUi.getElement( "btn_prev_boundingbox" )->addEventHandler( "mouseup", prevBoundingBox );
	panelButtons.push_back( "btn_prev_boundingbox" );
	
	editorUi.addElement( "btn_del_boundingbox", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_del_boundingbox" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_del_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 245, 470 );
	editorUi.getElement( "btn_del_boundingbox" )->addEventHandler( "mouseup", deleteBoundingBox );
	panelButtons.push_back( "btn_del_boundingbox" );
	
	editorUi.addElement( "btn_add_boundingbox", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_add_boundingbox" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_add_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 95, 470 );
	editorUi.getElement( "btn_add_boundingbox" )->addEventHandler( "mouseup", addBoundingBox );
	panelButtons.push_back( "btn_add_boundingbox" );
	
	editorUi.addElement( "btn_next_boundingbox", new Button( "font0", ">" ) );
	editorUi.getElement( "btn_next_boundingbox" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_next_boundingbox" )->getBox().getOrigin().move( currentScreenWidth - 50, 470 );
	editorUi.getElement( "btn_next_boundingbox" )->addEventHandler( "mouseup", nextBoundingBox );
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
	editorUi.getElement( "btn_prev_attackarea" )->addEventHandler( "mouseup", prevAttackArea );
	panelButtons.push_back( "btn_prev_attackarea" );
	
	editorUi.addElement( "btn_del_attackarea", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_del_attackarea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_del_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 245, 500 );
	editorUi.getElement( "btn_del_attackarea" )->addEventHandler( "mouseup", deleteAttackArea );
	panelButtons.push_back( "btn_del_attackarea" );
	
	editorUi.addElement( "btn_add_attackarea", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_add_attackarea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_add_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 95, 500 );
	editorUi.getElement( "btn_add_attackarea" )->addEventHandler( "mouseup", addAttackArea );
	panelButtons.push_back( "btn_add_attackarea" );
	
	editorUi.addElement( "btn_next_attackarea", new Button( "font0", ">" ) );
	editorUi.getElement( "btn_next_attackarea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_next_attackarea" )->getBox().getOrigin().move( currentScreenWidth - 50, 500 );
	editorUi.getElement( "btn_next_attackarea" )->addEventHandler( "mouseup", nextAttackArea );
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
	editorUi.getElement( "btn_prev_defencearea" )->addEventHandler( "mouseup", prevDefenceArea );
	panelButtons.push_back( "btn_prev_defencearea" );
	
	editorUi.addElement( "btn_del_defencearea", new Button( "font0", "~" ) );
	editorUi.getElement( "btn_del_defencearea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_del_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 245, 530 );
	editorUi.getElement( "btn_del_defencearea" )->addEventHandler( "mouseup", deleteDefenceArea );
	panelButtons.push_back( "btn_del_defencearea" );
	
	editorUi.addElement( "btn_add_defencearea", new Button( "font0", "+" ) );
	editorUi.getElement( "btn_add_defencearea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_add_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 95, 530 );
	editorUi.getElement( "btn_add_defencearea" )->addEventHandler( "mouseup", addDefenceArea );
	panelButtons.push_back( "btn_add_defencearea" );
	
	editorUi.addElement( "btn_next_defencearea", new Button( "font0", ">" ) );
	editorUi.getElement( "btn_next_defencearea" )->getBox().setWidth( 40 );
	editorUi.getElement( "btn_next_defencearea" )->getBox().getOrigin().move( currentScreenWidth - 50, 530 );
	editorUi.getElement( "btn_next_defencearea" )->addEventHandler( "mouseup", nextDefenceArea );
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
	pointInfo << "x: " << point.getX() << " y: " << point.getY();
	Font::get( "font0" )->renderSize( &infoWidth, &infoHeight, pointInfo.str() );
	Font::get( "font0" )->render( relative.getX() + applyZoom( point.getX() ) + (infoWidth / -2), relative.getY() + applyZoom( point.getY() ) - infoHeight, pointInfo.str() );
}

void renderBoxInformation( Box& box, Point& relative )
{
	int infoWidth = 0;
	int infoHeight = 0;
	
	stringstream boxInfo;
	boxInfo << "x: " << box.getOrigin().getX() << " y: " << box.getOrigin().getY();
	Font::get( "font0" )->renderSize( &infoWidth, &infoHeight, boxInfo.str() );
	Font::get( "font0" )->render( relative.getX() + applyZoom( box.getOrigin().getX() ) + ((applyZoom( box.getWidth() ) - infoWidth) / 2), relative.getY() + applyZoom( box.getOrigin().getY() ) + (applyZoom( box.getHeight() ) / 2) - infoHeight, boxInfo.str() );

	infoWidth = 0;
	infoHeight = 0;
	boxInfo.str( "" );
	boxInfo << box.getWidth() << " x " << box.getHeight();
	Font::get( "font0" )->renderSize( &infoWidth, &infoHeight, boxInfo.str() );
	Font::get( "font0" )->render( relative.getX() + applyZoom( box.getOrigin().getX() ) + ((applyZoom(box.getWidth()) - infoWidth) / 2), relative.getY() + applyZoom( box.getOrigin().getY() ) + (applyZoom( box.getHeight() ) / 2), boxInfo.str() );
}

void restrictPointToBoxArea( Point& point, Box& area )
{
	if( point.getX() < 0 )
		point.setX( 0 );
	else if( point.getX() > area.getWidth() )
		point.setX( area.getWidth() );
	
	if( point.getY() < 0 )
		point.setY( 0 );
	else if( point.getY() > area.getHeight() )
		point.setY( area.getHeight() );
}

void restrictBoxToBoxArea( Box& box, Box& area )
{
	if( box.getWidth() >= 0 )
	{
		if( box.getOrigin().getX() < 0 )
			box.getOrigin().setX( 0 );
		else if( box.getWidth() > area.getWidth() - box.getOrigin().getX() )
			box.setWidth( area.getWidth() - box.getOrigin().getX() );
	}
	else
	{
		if( box.getOrigin().getX() + box.getWidth() < 0 )
			box.setWidth( -1 * box.getOrigin().getX() + 1 );
		else if( box.getOrigin().getX() > area.getWidth() )
			box.getOrigin().setX( area.getWidth() );
	}
	
	if( box.getHeight() >= 0 )
	{
		if( box.getOrigin().getY() < 0 )
			box.getOrigin().setY( 0 );
		else if( box.getHeight() > area.getHeight() - box.getOrigin().getY() )
			box.setHeight( area.getHeight() - box.getOrigin().getY() );
	}
	else
	{
		if( box.getOrigin().getY() + box.getHeight() < 0 )
			box.setHeight( -1 * box.getOrigin().getY() + 1 );
		else if( box.getOrigin().getY() > area.getHeight() )
			box.getOrigin().setY( area.getHeight() );
	}
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
	stringstream ss;
	
	for( vector<string>::iterator it = panelButtons.begin() ; it != panelButtons.end() ; it++ )
		editorUi.hideElement( *it );
	
	savingState = true;
	editorUi.showElement( "zlbl_saving" );
	
	node::Node * object = new node::Node( node::Node::Tag, "object" );
	object->attr( "sprite", spriteFilename );
	
	// Browsing animations
	for( map<string, Animation *>::const_iterator it = animations.begin() ; it != animations.end() ; it++ )
	{
		node::Node * animation = new node::Node( node::Node::Tag, "animation" );
		animation->attr( "name", it->first );
		
		ss.str( "" );
		ss << it->second->getSpeed();
		animation->attr( "speed", ss.str() );
		
		// Browsing frames
		for( unsigned int iFrame = 0 ; iFrame < it->second->getFrameCount() ; iFrame++ )
		{
			node::Node * frame = new node::Node( node::Node::Tag, "frame" );
			
			// Frame box
			ss.str( "" );
			ss << it->second->getFrameByIndex( iFrame ).getBox().getOrigin().getX();
			frame->attr( "x", ss.str() );
			
			ss.str( "" );
			ss << it->second->getFrameByIndex( iFrame ).getBox().getOrigin().getY();
			frame->attr( "y", ss.str() );
			
			ss.str( "" );
			ss << it->second->getFrameByIndex( iFrame ).getBox().getWidth();
			frame->attr( "width", ss.str() );
			
			ss.str( "" );
			ss << it->second->getFrameByIndex( iFrame ).getBox().getHeight();
			frame->attr( "height", ss.str() );
			
			// Anchor
			node::Node * anchor = new node::Node( node::Node::Tag, "anchor" );
			
			ss.str( "" );
			ss << it->second->getFrameByIndex( iFrame ).getAnchor().getX();
			anchor->attr( "x", ss.str() );
			
			ss.str( "" );
			ss << it->second->getFrameByIndex( iFrame ).getAnchor().getY();
			anchor->attr( "y", ss.str() );
			
			frame->append( anchor );
			
			// Bounding boxes
			for( unsigned int iBounding = 0 ; iBounding < it->second->getFrameByIndex( iFrame ).getBoundingBoxesCount() ; iBounding++ )
			{
				node::Node * bounding = new node::Node( node::Node::Tag, "bounding-box" );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getBoundingBox( iBounding ).getOrigin().getX();
				bounding->attr( "x", ss.str() );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getBoundingBox( iBounding ).getOrigin().getY();
				bounding->attr( "y", ss.str() );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getBoundingBox( iBounding ).getWidth();
				bounding->attr( "width", ss.str() );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getBoundingBox( iBounding ).getHeight();
				bounding->attr( "height", ss.str() );
				
				frame->append( bounding );
			}
			
			// Attack areas
			for( unsigned int iAttack = 0 ; iAttack < it->second->getFrameByIndex( iFrame ).getAttackAreasCount() ; iAttack++ )
			{
				node::Node * attack = new node::Node( node::Node::Tag, "attack-area" );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getAttackArea( iAttack ).getOrigin().getX();
				attack->attr( "x", ss.str() );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getAttackArea( iAttack ).getOrigin().getY();
				attack->attr( "y", ss.str() );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getAttackArea( iAttack ).getWidth();
				attack->attr( "width", ss.str() );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getAttackArea( iAttack ).getHeight();
				attack->attr( "height", ss.str() );
				
				frame->append( attack );
			}
			
			// Defence areas
			for( unsigned int iDefence = 0 ; iDefence < it->second->getFrameByIndex( iFrame ).getDefenceAreasCount() ; iDefence++ )
			{
				node::Node * defence = new node::Node( node::Node::Tag, "defence-area" );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getDefenceArea( iDefence ).getOrigin().getX();
				defence->attr( "x", ss.str() );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getDefenceArea( iDefence ).getOrigin().getY();
				defence->attr( "y", ss.str() );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getDefenceArea( iDefence ).getWidth();
				defence->attr( "width", ss.str() );
				
				ss.str( "" );
				ss << it->second->getFrameByIndex( iFrame ).getDefenceArea( iDefence ).getHeight();
				defence->attr( "height", ss.str() );
				
				frame->append( defence );
			}
			
			animation->append( frame );
		}
		
		object->append( animation );
	}
	
	ofstream file;
	file.open( objectFilename.c_str() );
	
	if( file.is_open() )
	{
		file << object->text( true );
		file.close();
	}
	else
		cout << "[ObjectEditor] Can not open file \"" << objectFilename << "\" to save object." << endl;

	delete object;
	
	for( vector<string>::iterator it = panelButtons.begin() ; it != panelButtons.end() ; it++ )
		editorUi.showElement( *it );
						
	editorUi.hideElement( "zlbl_saving" );
	savingState = false;

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
	bool success = true;
	ifstream file( filename.c_str() );

	if( file.is_open() )
	{
		stringstream ss;
		ss << file.rdbuf();

		parser::NodeParser nParser( ss.str() );
		node::Node * root = nParser.parse();

		if( root != NULL )
		{
			node::Node * object = root->find( "object" );

			if( object != NULL )
			{
				if( object->hasAttr( "sprite" ) )
				{
					Sprite * nSprite = new Sprite( object->attr( "sprite" ) );
				
					if( nSprite->isLoaded() )
					{
						cleanObjectVariables();
						sprite = nSprite;
						
						objectFilename = filename;
						spriteFilename = object->attr( "sprite" );
						
						adjustSpriteToScreen();
						
						// Initialize each animation with one frame
						Frame defaultFrame( Box( 0, 0, sprite->getWidth(), sprite->getHeight() ) );
						
						for( vector<string>::iterator it = animationsNames.begin() ; it != animationsNames.end() ; it++ )
						{
							animations[*it] = new Animation();
							animations[*it]->setSpeed( 100 );
							animations[*it]->addFrame( defaultFrame );
						}
						
						// Browse animations
						node::Node * animation = object->childAt( 0 );
						
						while( animation != NULL )
						{
							if( animation->getType() == node::Node::Tag && animation->getName() == "animation" )
							{
								cout << "[Object Editor] Found animation \"" <<  animation->attr( "name" ) << "\"." << endl;
								Animation * anim = NULL;
								
								map<string, Animation *>::iterator itAnimation = animations.find( animation->attr( "name" ) );
								
								if( itAnimation != animations.end() )
									anim = itAnimation->second;
								else
								{
									animationsNames.push_back( animation->attr( "name" ) );
									animations[animation->attr( "name" )] = new Animation();
									animations[animation->attr( "name" )]->setSpeed( 100 );
									animations[animation->attr( "name" )]->addFrame( defaultFrame );
									
									anim = animations[animation->attr( "name" )];
								}
								
								if( animation->isIntegerAttr( "speed" ) )
									anim->setSpeed( static_cast<unsigned int>( animation->integerAttr( "speed" ) ) );
								
								// Browse frames
								node::Node * frame = animation->childAt( 0 );
								bool firstFrame = true;
								
								while( frame != NULL )
								{
									if( frame->getType() == node::Node::Tag && frame->getName() == "frame" )
									{
										Frame oFrame;
										oFrame.getAnchor().move( 0, frame->isIntegerAttr( "height" ) ? frame->integerAttr( "height" ) : 0 );
										oFrame.getBox().getOrigin().move( frame->isIntegerAttr( "x" ) ? frame->integerAttr( "x" ) : 0, frame->isIntegerAttr( "y" ) ? frame->integerAttr( "y" ) : 0 );
										oFrame.getBox().resize( frame->isIntegerAttr( "width" ) ? frame->integerAttr( "width" ) : 0, frame->isIntegerAttr( "height" ) ? frame->integerAttr( "height" ) : 0 );
										
										node::Node * frameChild = frame->childAt( 0 );

										while( frameChild != NULL )
										{
											if( frameChild->getType() == node::Node::Tag )
											{
												if( frameChild->getName() == "anchor" )
												{
													if( frameChild->isIntegerAttr( "x" ) )
														oFrame.getAnchor().setX( frameChild->integerAttr( "x" ) );
										
													if( frameChild->isIntegerAttr( "y" ) )
														oFrame.getAnchor().setY( frameChild->integerAttr( "y" ) );
												}
												else if( frameChild->getName() == "bounding-box" )
												{
													Box boundingBox;
													
													if( frameChild->isIntegerAttr( "x" ) )
														boundingBox.getOrigin().setX( frameChild->integerAttr( "x" ) );
													
													if( frameChild->isIntegerAttr( "y" ) )
														boundingBox.getOrigin().setY( frameChild->integerAttr( "y" ) );
													
													if( frameChild->isIntegerAttr( "width" ) )
														boundingBox.setWidth( frameChild->integerAttr( "width" ) );
													
													if( frameChild->isIntegerAttr( "height" ) )
														boundingBox.setHeight( frameChild->integerAttr( "height" ) );
													
													oFrame.addBoundingBox( boundingBox );
												}
												else if( frameChild->getName() == "attack-area" )
												{
													Box attackArea;
													
													if( frameChild->isIntegerAttr( "x" ) )
														attackArea.getOrigin().setX( frameChild->integerAttr( "x" ) );
													
													if( frameChild->isIntegerAttr( "y" ) )
														attackArea.getOrigin().setY( frameChild->integerAttr( "y" ) );
													
													if( frameChild->isIntegerAttr( "width" ) )
														attackArea.setWidth( frameChild->integerAttr( "width" ) );
													
													if( frameChild->isIntegerAttr( "height" ) )
														attackArea.setHeight( frameChild->integerAttr( "height" ) );
													
													oFrame.addAttackArea( attackArea );
												}
												else if( frameChild->getName() == "defence-area" )
												{
													Box defenceArea;
													
													if( frameChild->isIntegerAttr( "x" ) )
														defenceArea.getOrigin().setX( frameChild->integerAttr( "x" ) );
													
													if( frameChild->isIntegerAttr( "y" ) )
														defenceArea.getOrigin().setY( frameChild->integerAttr( "y" ) );
													
													if( frameChild->isIntegerAttr( "width" ) )
														defenceArea.setWidth( frameChild->integerAttr( "width" ) );
													
													if( frameChild->isIntegerAttr( "height" ) )
														defenceArea.setHeight( frameChild->integerAttr( "height" ) );
													
													oFrame.addDefenceArea( defenceArea );
												}
											}
											
											frameChild = frameChild->next();
										}

										
										if( firstFrame )
										{
											firstFrame = false;
											anim->removeFrameByIndex( 0 );
										}
										
										anim->addFrame( oFrame );
									}
									
									frame = frame->next();
								}
							}
							
							animation = animation->next();
						}
						
						synchronizeLabels();
					}
					else
					{
						cout << "[Object Editor] Can not load sprite \"" << object->attr( "sprite" ) << "\"." << endl;
						success = false;
						delete nSprite;
					}
				}
				else
				{
					cout << "[Object Editor] Can not find \"sprite\" attribute." << endl;
					success = false;
				}
			}
			else
			{
				cout << "[Object Editor] Can not find <object> tag." << endl;
				success = false;
			}
		}
		else
		{
			cout << "[Object Editor] Can not parse file as XML." << endl;
			success = false;
		}
	}
	else
	{
		cout << "[Object Editor] Can not open file \"" << filename << "\"." << endl;
		success = false;
	}
	
	return success;
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
		
		if( frame.getBox().getWidth() + frame.getBox().getOrigin().getX() < sprite->getWidth() )
			frame.getBox().getOrigin().moveBy( frame.getBox().getWidth(), 0 );
			
		else if( frame.getBox().getHeight() + frame.getBox().getOrigin().getY() < sprite->getHeight() )
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
		if( currentFrame > 0 ) currentFrame = currentFrame - 1;
		synchronizeLabels();
	}
	
	return true;
}

bool nextFrame( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
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
	
	if( animation != NULL )
	{
		if( currentFrame == 0 )
			currentFrame = animation->getFrameCount() - 1;
		else
			currentFrame--;
			
		synchronizeLabels();
	}
	
	return true;
}

// Bounding Box Events
bool addBoundingBox( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		if( currentBoundingBox < animation->getFrameByIndex( currentFrame ).getBoundingBoxesCount() )
		{
			currentBoundingBox++;
			synchronizeLabels();
		}
	}
	
	return true;
}

bool deleteBoundingBox( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		if( currentBoundingBox < animation->getFrameByIndex( currentFrame ).getBoundingBoxesCount() )
		{
			animation->getFrameByIndex( currentFrame ).removeBoundingBox( currentBoundingBox );
			if( currentBoundingBox > 0 ) currentBoundingBox = currentBoundingBox - 1;
		}
		else if( animation->getFrameByIndex( currentFrame ).getBoundingBoxesCount() > 0 )
			currentBoundingBox = animation->getFrameByIndex( currentFrame ).getBoundingBoxesCount() - 1;
		else
			currentBoundingBox = 0;
		
		synchronizeLabels();
	}
	
	return true;
}

bool nextBoundingBox( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		currentBoundingBox++;
		
		if( currentBoundingBox >= animation->getFrameByIndex( currentFrame ).getBoundingBoxesCount() )
			currentBoundingBox = 0;
			
		synchronizeLabels();
	}
	
	return true;
}

bool prevBoundingBox( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		if( currentBoundingBox == 0 )
			currentBoundingBox = animation->getFrameByIndex( currentFrame ).getBoundingBoxesCount() - 1;
		else
			currentBoundingBox--;
			
		synchronizeLabels();
	}
	
	return true;
}

// Attack Area Events
bool addAttackArea( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		if( currentAttackArea < animation->getFrameByIndex( currentFrame ).getAttackAreasCount() )
		{
			currentAttackArea++;
			synchronizeLabels();
		}
	}
	
	return true;
}

bool deleteAttackArea( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		if( currentAttackArea < animation->getFrameByIndex( currentFrame ).getAttackAreasCount() )
		{
			animation->getFrameByIndex( currentFrame ).removeAttackArea( currentAttackArea );
			if( currentAttackArea > 0 ) currentAttackArea = currentAttackArea - 1;
		}
		else if( animation->getFrameByIndex( currentFrame ).getAttackAreasCount() > 0 )
			currentAttackArea = animation->getFrameByIndex( currentFrame ).getAttackAreasCount() - 1;
		else
			currentAttackArea = 0;
		
		synchronizeLabels();
	}
	
	return true;
}

bool nextAttackArea( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		currentAttackArea++;
		
		if( currentAttackArea >= animation->getFrameByIndex( currentFrame ).getAttackAreasCount() )
			currentAttackArea = 0;
			
		synchronizeLabels();
	}
	
	return true;
}

bool prevAttackArea( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		if( currentAttackArea == 0 )
			currentAttackArea = animation->getFrameByIndex( currentFrame ).getAttackAreasCount() - 1;
		else
			currentAttackArea--;
			
		synchronizeLabels();
	}
	
	return true;
}

// Defence Area Events
bool addDefenceArea( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		if( currentDefenceArea < animation->getFrameByIndex( currentFrame ).getDefenceAreasCount() )
		{
			currentDefenceArea++;
			synchronizeLabels();
		}
	}
	
	return true;
}

bool deleteDefenceArea( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		if( currentDefenceArea < animation->getFrameByIndex( currentFrame ).getDefenceAreasCount() )
		{
			animation->getFrameByIndex( currentFrame ).removeDefenceArea( currentDefenceArea );
			if( currentDefenceArea > 0 ) currentDefenceArea = currentDefenceArea - 1;
		}
		else if( animation->getFrameByIndex( currentFrame ).getDefenceAreasCount() > 0 )
			currentDefenceArea = animation->getFrameByIndex( currentFrame ).getDefenceAreasCount() - 1;
		else
			currentDefenceArea = 0;
		
		synchronizeLabels();
	}
	
	return true;
}

bool nextDefenceArea( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		currentDefenceArea++;
		
		if( currentDefenceArea >= animation->getFrameByIndex( currentFrame ).getDefenceAreasCount() )
			currentDefenceArea = 0;
			
		synchronizeLabels();
	}
	
	return true;
}

bool prevDefenceArea( Element * element )
{
	Animation * animation = animations[animationsNames[currentAnimation]];
	
	if( animation != NULL )
	{
		if( currentDefenceArea == 0 )
			currentDefenceArea = animation->getFrameByIndex( currentFrame ).getAttackAreasCount() - 1;
		else
			currentDefenceArea--;
			
		synchronizeLabels();
	}
	
	return true;
}
