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
	Object::Object() : x(0), y(0), width(0), height(0), speedModulation(0), zoom(100), sprite(NULL), currentAnimation(NULL)
	{
	}
	
	Object::Object( const char * filename ) : x(0), y(0), width(0), height(0), speedModulation(0), zoom(100), sprite(NULL), currentAnimation(NULL)
	{
		this->load( filename );
	}
	
	Object::Object( const string& filename ) : x(0), y(0), width(0), height(0), speedModulation(0), zoom(100), sprite(NULL), currentAnimation(NULL)
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
											int anchorX = 0;
											int anchorY = frame->isIntegerAttr( "height" ) ? frame->integerAttr( "height" ) : 0;
											
											node::Node * frameChild = frame->childAt( 0 );
											
											while( frameChild != NULL )
											{
												if( frameChild->getType() == node::Node::Tag && frameChild->getName() == "anchor" )
												{
													if( frameChild->isIntegerAttr( "x" ) )
														anchorX = frameChild->integerAttr( "x" );
													
													if( frameChild->isIntegerAttr( "y" ) )
														anchorY = frameChild->integerAttr( "y" );
												}
												
												frameChild = frameChild->next();
											}
											
											anim->addFrame(
												frame->isIntegerAttr( "x" ) ? frame->integerAttr( "x" ) : 0,
												frame->isIntegerAttr( "y" ) ? frame->integerAttr( "y" ) : 0,
												frame->isIntegerAttr( "width" ) ? frame->integerAttr( "width" ) : 0,
												frame->isIntegerAttr( "height" ) ? frame->integerAttr( "height" ) : 0,
												anchorX,
												anchorY
											);
											
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
			const Sprite::Frame * frame = this->currentAnimation->getFrame( time );
			
			if( frame != NULL )
			{
				SDL_Rect dstRect;
				int anchorX = this->x;
				int anchorY = this->y;
				
				dstRect.x = anchorX - (frame->anchorX * this->zoom / 100);
				dstRect.y = anchorY - (frame->anchorY * this->zoom / 100);
				dstRect.w = (this->zoom * frame->width) / 100;
				dstRect.h = (this->zoom * frame->height) / 100;
	
				SDL_Rect srcRect;
				srcRect.x = frame->x;
				srcRect.y = frame->y;
				srcRect.w = frame->width;
				srcRect.h = frame->height;
	
				SDL_RenderCopy( Screen::get()->getRenderer(), this->sprite->getTexture(), &srcRect, &dstRect );
				
				// Render the anchor point
				if( true )
				{
					SDL_SetRenderDrawColor( Screen::get()->getRenderer(), 0, 0, 255, 128 );
					SDL_RenderDrawLine( Screen::get()->getRenderer(), anchorX + (5 * this->zoom / 100), anchorY, anchorX - (5 * this->zoom / 100), anchorY );
					SDL_RenderDrawLine( Screen::get()->getRenderer(), anchorX, anchorY + (5 * this->zoom / 100), anchorX, anchorY - (5 * this->zoom / 100) );
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
}

