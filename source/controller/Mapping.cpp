#include <controller/Mapping.h>
#include <data/parser/NodeParser.h>

#ifdef DEBUG0
#include <iostream>
#endif

#include <string>
#include <fstream>
#include <sstream>

using namespace std;
using namespace data;

namespace controller
{
	Mapping::Mapping( const string& filename )
	{
		this->load( filename.c_str() );
	}

	Mapping::Mapping( const char * filename )
	{
		this->load( filename );
	}

	Mapping::~Mapping()
	{
	}
	
	bool Mapping::load( const char * filename )
	{
		map<string, Mapping::Button> internalValues;
		internalValues["BTNUP"] = Mapping::BTNUP;
		internalValues["BTNRIGHT"] = Mapping::BTNRIGHT;
		internalValues["BTNDOWN"] = Mapping::BTNDOWN;
		internalValues["BTNLEFT"] = Mapping::BTNLEFT;
		internalValues["LT1"] = Mapping::LT1;
		internalValues["LT2"] = Mapping::LT2;
		internalValues["LT3"] = Mapping::LT3;
		internalValues["RT1"] = Mapping::RT1;
		internalValues["RT2"] = Mapping::RT2;
		internalValues["RT3"] = Mapping::RT3;
		internalValues["SELECT"] = Mapping::SELECT;
		internalValues["START"] = Mapping::START;
		internalValues["AXH"] = Mapping::AXH;
		internalValues["AHV"] = Mapping::AXV;
		
		#ifdef DEBUG0
		cout << "[Mapping#" << this << "] Loading file \"" << filename << "\"..." << endl;
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
				node::Node * nMapping = root->find( "mapping" );
				
				if( nMapping != NULL )
				{
					node::Node * node = nMapping->childAt( 0 );
					
					while( node != NULL )
					{
						if( node->getType() == node::Node::Tag )
						{
							if( node->getName() == "button" )
							{
								if( node->isIntegerAttr( "id" ) && node->hasAttr( "action" ) )
								{
									map<string, Mapping::Button>::iterator itBtn = internalValues.find( node->attr( "action" ) );
									
									if( itBtn != internalValues.end() )
									{
										this->buttons[node->integerAttr( "id" )] = internalValues[node->attr( "action" )];
										
										#ifdef DEBUG0
										cout << "[Mapping#" << this << "] Button #" << node->integerAttr( "id" ) << " triggers action \"" << node->attr( "action" ) << "\"." << endl;
										#endif
									}
									else
									{
										#ifdef DEBUG0
										cout << "[Mapping#" << this << "] Ignoring action \"" << node->attr( "action" ) << "\"." << endl;
										#endif
									}
								}
							}
							else if( node->getName() == "axis" )
							{
								if( node->isIntegerAttr( "id" ) && node->hasAttr( "action" ) )
								{
									map<string, Mapping::Button>::iterator itBtn = internalValues.find( node->attr( "action" ) );
										
									if( itBtn != internalValues.end() )
									{
										this->axes[node->integerAttr( "id" )] = internalValues[node->attr( "action" )];
										
										#ifdef DEBUG0
										cout << "[Mapping#" << this << "] Axis #" << node->integerAttr( "id" ) << " triggers action \"" << node->attr( "action" ) << "\"." << endl;
										#endif
									}
									else
									{
										#ifdef DEBUG0
										cout << "[Mapping#" << this << "] Ignoring action \"" << node->attr( "action" ) << "\"." << endl;
										#endif
									}
								}
							}
						}
						
						node = node->next();
					}
				}
				else
				{
					#ifdef DEBUG0
					cout << "[Mapping#" << this << "] Unable to find \"mapping\" tag." << endl;
					#endif
				}
			}
			else
			{
				#ifdef DEBUG0
				cout << "[Mapping#" << this << "] Unable to parse file." << endl;
				#endif
			}
		}
		else
		{
			#ifdef DEBUG0
			cout << "[Mapping#" << this << "] Unable to open file \"" << filename << "\"." << endl;
			#endif
		}
		
		return this->isLoaded();
	}
	
	bool Mapping::isLoaded()
	{
		return this->buttons.size() + this->axes.size() > 0;
	}

	Mapping::Button Mapping::getButtonFromButton( int value )
	{
		Mapping::Button btn = Mapping::NOBTN;
		
		map<int, Mapping::Button>::iterator it = this->buttons.find( value );
		
		if( it != this->buttons.end() )
			btn = it->second;
		
		return btn;
	}
	
	Mapping::Button Mapping::getButtonFromAxis( int value )
	{
		Mapping::Button btn = Mapping::NOBTN;
		
		map<int, Mapping::Button>::iterator it = this->axes.find( value );
		
		if( it != this->axes.end() )
			btn = it->second;
		
		return btn;
	}
}
