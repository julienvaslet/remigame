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
	Object::Object() : sprite(NULL)
	{
	}
	
	Object::Object( const char * filename ) : sprite(NULL)
	{
		this->load( filename );
	}
	
	Object::Object( const string& filename ) : sprite(NULL)
	{
		this->load( filename.c_str() );
	}
	
	Object::~Object()
	{
		if( this->sprite != NULL )
			delete sprite;

		#ifdef DEBUG0
		cout << "[Object#" << this << "] Destroyed object (" << this << ")." << endl;
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
				if( root->getName() == "object" )
				{
					if( root->hasAttr( "sprite" ) )
					{
						this->sprite = new Sprite( root->attr( "sprite" ) );
						
						if( this->sprite->isLoaded() )
						{
							// Browse animations
							node::Node * animation = root->childAt( 0 );
							
							while( animation != NULL )
							{
								if( animation->getName() == "animation" )
								{
									// Attribute name
									cout << "name = " << animation->attr( "name" ) << endl;
									
									// Attribute speed
									cout << "speed = " << animation->attr( "speed" ) << endl;
									
									// Browse frames
									node::Node * frame = animation->childAt( 0 );
									
									while( frame != NULL )
									{
										if( frame->getName() == "frame" )
										{
											// Attribute x
											cout << "x = " << frame->attr( "x" ) << endl;
											
											// Attribute y
											cout << "y = " << frame->attr( "y" ) << endl;
											
											// Attribute width
											cout << "width = " << frame->attr( "width" ) << endl;
											
											// Attribute height
											cout << "height = " << frame->attr( "height" ) << endl;
										}
									}
								}
								
								animation = animation->next();
							}
						}
						else
						{
							#ifdef DEBUG0
							cout << "[Object#" << this << "] Unable to load sprite \"" << root->attr( "sprite" ) << "\"." << endl;
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
		
		return success;
	}
}

