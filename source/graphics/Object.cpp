#include <graphics/Object.h>

#ifdef DEBUG0
#include <iostream>
#endif

#include <data/parser/NodeParser.h>

#include <fstream>
#include <sstream>
#include <string>

using namespace data;
using namespace std;

namespace graphics
{
	Object::Object() : x(0), y(0), width(0), height(0), speedModulation(0), zoom(100), sprite(NULL), currentAnimation(NULL), anchorPointRenderingState(false), boundingBoxesRenderingState(false), attackAreasRenderingState(false), defenceAreasRenderingState(false)
	{
	}
	
	Object::Object( const char * filename ) : x(0), y(0), width(0), height(0), speedModulation(0), zoom(100), sprite(NULL), currentAnimation(NULL), anchorPointRenderingState(false), boundingBoxesRenderingState(false), attackAreasRenderingState(false), defenceAreasRenderingState(false)
	{
		this->load( filename );
	}
	
	Object::Object( const string& filename ) : x(0), y(0), width(0), height(0), speedModulation(0), zoom(100), sprite(NULL), currentAnimation(NULL), anchorPointRenderingState(false), boundingBoxesRenderingState(false), attackAreasRenderingState(false), defenceAreasRenderingState(false)
	{
		this->load( filename.c_str() );
	}
	
	Object::~Object()
	{
		for( map<string, Animation *>::iterator it = this->animations.begin() ; it != this->animations.end() ; it++ )
			delete it->second;
			
		if( this->sprite != NULL )
			delete sprite;

		#ifdef DEBUG0
		cout << "[Object#" << this << "] Destroyed object." << endl;
		#endif
	}
	
	bool Object::load( const char * filename )
	{
		bool success = true;
		
		#ifdef DEBUG0
		cout << "[Object#" << this << "] Loading file \"" << filename << "\"..." << endl;
		#endif
		
		ifstream file( filename );
		
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
						this->sprite = new Sprite( object->attr( "sprite" ) );
						
						if( this->sprite->isLoaded() )
						{
							// Browse animations
							node::Node * animation = object->childAt( 0 );
							
							while( animation != NULL )
							{
								if( animation->getType() == node::Node::Tag && animation->getName() == "animation" )
								{
									Animation * anim = new Animation();
									anim->setSpeed( animation->isIntegerAttr( "speed" ) ? static_cast<unsigned int>( animation->integerAttr( "speed" ) ): 100 );
									
									// Browse frames
									node::Node * frame = animation->childAt( 0 );
									
									int iFrame = 0;
									
									while( frame != NULL )
									{
										if( frame->getType() == node::Node::Tag && frame->getName() == "frame" )
										{
											Frame oFrame;
											oFrame.getAnchor().move( 0, frame->isIntegerAttr( "height" ) ? frame->integerAttr( "height" ) : 0 );
											
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
													else if( frameChild->getName() == "bouding-box" )
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
												}
												
												frameChild = frameChild->next();
											}
											
											oFrame.getBox().getOrigin().move( frame->isIntegerAttr( "x" ) ? frame->integerAttr( "x" ) : 0, frame->isIntegerAttr( "y" ) ? frame->integerAttr( "y" ) : 0 );
											oFrame.getBox().resize( frame->isIntegerAttr( "width" ) ? frame->integerAttr( "width" ) : 0, frame->isIntegerAttr( "height" ) ? frame->integerAttr( "height" ) : 0 );
											
											anim->addFrame( oFrame );

											iFrame++;
										}
										
										frame = frame->next();
									}
									
									if( iFrame > 0 )
									{
										this->animations[ animation->attr( "name" ) ] = anim;
										
										#ifdef DEBUG0
										cout << "[Object#" << this << "] Animation \"" << animation->attr( "name" ) << "\" loaded." << endl;
										#endif
									}
									else
									{
										delete anim;
									}
								}
								
								animation = animation->next();
							}
						}
						else
						{
							#ifdef DEBUG0
							cout << "[Object#" << this << "] Unable to load sprite \"" << object->attr( "sprite" ) << "\"." << endl;
							#endif
							
							delete this->sprite;
							this->sprite = NULL;
							success = false;
						}
					}
					else
					{
						#ifdef DEBUG0
						cout << "[Object#" << this << "] Unable to find \"sprite\" attribute in <object> tag in \"" << filename << "\"." << endl;
						#endif
						
						success = false;
					}
				}
				else
				{
					#ifdef DEBUG0
					cout << "[Object#" << this << "] Unable to find <object> tag in \"" << filename << "\"." << endl;
					#endif
					
					success = false;
				}
				
				
				delete root;
			}
			else
			{
				#ifdef DEBUG0
				cout << "[Object#" << this << "] Unable to parse file \"" << filename << "\"." << endl;
				#endif
				success = false;
			}
			
			file.close();
		}
		else
		{
			#ifdef DEBUG0
			cout << "[Object#" << this << "] Unable to load file \"" << filename << "\"." << endl;
			#endif
			
			success = false;
		}
		
		// If the object has been successfully loaded
		// "idle" animation is selected or, if absent, the first one.
		if( success )
		{
			if( !this->setAnimation( "idle" ) )
			{
				if( this->animations.empty() )
				{
					map<string, Animation *>::iterator it = this->animations.begin();
					this->currentAnimation = it->second;
					this->currentAnimation->reset();
					
					#ifdef DEBUG0
					cout << "[Object#" << this << "] First animation selected: \"" << it->first << "\"." << endl;
					#endif
				}
				else
				{
					#ifdef DEBUG0
					cout << "[Object#" << this << "] There is no animation for this object." << endl;
					#endif
				}
			}
		}
		
		return success;
	}
	
	bool Object::isLoaded()
	{
		return this->sprite->isLoaded();
	}
		
	bool Object::setAnimation( const string& name )
	{
		bool changed = true;
		
		map<string, Animation *>::const_iterator it = this->animations.find( name );
		
		if( it != this->animations.end() )
		{
			this->currentAnimation = it->second;
			this->currentAnimation->reset();
			this->currentAnimation->setSpeedModulation( this->speedModulation );
			
			#ifdef DEBUG0
			cout << "[Object#" << this << "] Animation \"" << it->first << "\" selected." << endl;
			#endif
		}
		else
		{
			changed = false;
			
			#ifdef DEBUG0
			cout << "[Object#" << this << "] Animation \"" << name << "\" does not exist." << endl;
			#endif
		}
		
		return changed;
	}
	
	void Object::render( unsigned int time )
	{
		if( currentAnimation != NULL )
		{
			Frame * frame = this->currentAnimation->getFrame( time );
			
			if( frame != NULL )
			{
				SDL_Rect dstRect;
				int anchorX = this->x;
				int anchorY = this->y;
				
				dstRect.x = anchorX - (frame->getAnchor().getX() * this->zoom / 100);
				dstRect.y = anchorY - (frame->getAnchor().getY() * this->zoom / 100);
				dstRect.w = (this->zoom * frame->getBox().getWidth()) / 100;
				dstRect.h = (this->zoom * frame->getBox().getHeight()) / 100;
	
				SDL_Rect srcRect;
				srcRect.x = frame->getBox().getOrigin().getX();
				srcRect.y = frame->getBox().getOrigin().getY();
				srcRect.w = frame->getBox().getWidth();
				srcRect.h = frame->getBox().getHeight();
	
				SDL_RenderCopy( Screen::get()->getRenderer(), this->sprite->getTexture(), &srcRect, &dstRect );
				
				// Render the anchor point (green)
				if( this->anchorPointRenderingState )
				{
					SDL_SetRenderDrawColor( Screen::get()->getRenderer(), 0, 255, 0, 128 );
					SDL_RenderDrawLine( Screen::get()->getRenderer(), anchorX + (5 * this->zoom / 100), anchorY, anchorX - (5 * this->zoom / 100), anchorY );
					SDL_RenderDrawLine( Screen::get()->getRenderer(), anchorX, anchorY + (5 * this->zoom / 100), anchorX, anchorY - (5 * this->zoom / 100) );
				}
				
				// Render each bounding boxes (green)
				if( this->boundingBoxesRenderingState )
				{
					SDL_SetRenderDrawColor( Screen::get()->getRenderer(), 0, 255, 0, 128 );
					
					for( int i = 0 ; i < frame->getBoundingBoxesCount() ; i++ )
					{
						Box boundingBox = frame->getBoundingBox( i );
						SDL_Rect bRect;
						bRect.x = this->x + ((boundingBox.getOrigin().getX() - frame->getAnchor().getX()) * this->zoom / 100);
						bRect.y = this->y + ((boundingBox.getOrigin().getY() - frame->getAnchor().getY()) * this->zoom / 100);
						bRect.w = (boundingBox.getWidth() * this->zoom / 100 );
						bRect.h = (boundingBox.getHeight() * this->zoom / 100 );
						
						SDL_RenderDrawRect( Screen::get()->getRenderer(), &bRect );
					}
				}
				
				// Render each attack areas (red)
				if( this->attackAreasRenderingState )
				{
					SDL_SetRenderDrawColor( Screen::get()->getRenderer(), 0, 255, 0, 128 );
					
					for( int i = 0 ; i < frame->getAttackAreasCount() ; i++ )
					{
						Box attackArea = frame->getAttackArea( i );
						SDL_Rect bRect;
						bRect.x = this->x + ((attackArea.getOrigin().getX() - frame->getAnchor().getX()) * this->zoom / 100);
						bRect.y = this->y + ((attackArea.getOrigin().getY() - frame->getAnchor().getY()) * this->zoom / 100);
						bRect.w = (attackArea.getWidth() * this->zoom / 100 );
						bRect.h = (attackArea.getHeight() * this->zoom / 100 );
						
						SDL_RenderDrawRect( Screen::get()->getRenderer(), &bRect );
					}	
				}
				
				// Render each defence areas (blue)
				if( this->defenceAreasRenderingState )
				{
					SDL_SetRenderDrawColor( Screen::get()->getRenderer(), 0, 0, 255, 128 );
					
					for( int i = 0 ; i < frame->getDefenceAreasCount() ; i++ )
					{
						Box defenceArea = frame->getDefenceArea( i );
						SDL_Rect bRect;
						bRect.x = this->x + ((defenceArea.getOrigin().getX() - frame->getAnchor().getX()) * this->zoom / 100);
						bRect.y = this->y + ((defenceArea.getOrigin().getY() - frame->getAnchor().getY()) * this->zoom / 100);
						bRect.w = (defenceArea.getWidth() * this->zoom / 100 );
						bRect.h = (defenceArea.getHeight() * this->zoom / 100 );
						
						SDL_RenderDrawRect( Screen::get()->getRenderer(), &bRect );
					}
				}
			}
		}
	}
	
	int Object::getX()
	{
		return this->x;
	}

	int Object::getY()
	{
		return this->y;
	}

	int Object::getWidth()
	{
		return this->width;
	}

	int Object::getHeight()
	{
		return this->height;
	}

	void Object::move( int x, int y )
	{
		this->x = x;
		this->y = y;
	}

	void Object::resize( int width, int height )
	{
		this->width = width;
		this->height = height;
	}
	
	void Object::setSpeedModulation( int modulation )
	{
		this->speedModulation = modulation;
		this->currentAnimation->setSpeedModulation( this->speedModulation );
	}
	
	int Object::getSpeedModulation()
	{
		return this->speedModulation;
	}
	
	int Object::getAnimationSpeed()
	{
		return this->currentAnimation->getSpeed();
	}
	
	int Object::getZoom()
	{
		return this->zoom;
	}
	
	void Object::setZoom( int zoom )
	{
		this->zoom = zoom;
	}
	
	void Object::setAnchorPointRenderingState( bool state )
	{
		this->anchorPointRenderingState = state;
	}
	
	void Object::setBoundingBoxesRenderingState( bool state )
	{
		this->boundingBoxesRenderingState = state;
	}
	
	void Object::setAttackAreasRenderingState( bool state )
	{
		this->attackAreasRenderingState = state;
	}
	
	void Object::setDefenceAreasRenderingState( bool state )
	{
		this->defenceAreasRenderingState = state;
	}
}

