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
		#ifdef DEBUG0
		cout << "[Object#" << this << "] Destroyed object (" << this << ")." << endl;
		#endif
	}
	
	bool Object::load( const char * filename )
	{
		bool success = true;
		
		#ifdef DEBUG0
		cout << "[Object#" << this << "] Loading file \"" << filename << "\"" << endl;
		#endif
		
		ifstream file( filename );
		
		if( file.is_open() )
		{
			stringstream ss;
			ss << file.rdbuf();
			
			parser::NodeParser nParser( ss.str() );
			node::Node * root = nParser.parse();
			
			root->show();
			
			delete root;
			
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

