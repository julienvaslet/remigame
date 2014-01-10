#ifndef __DATA_NODE_NODE_H
#define __DATA_NODE_NODE_H	1

#include <string>
#include <map>
#include <vector>
using namespace std;

namespace data
{
	namespace node
	{
		class Node
		{
			public:
				const static int Comment;
				const static int Text;
				const static int Tag;

			protected:
				static unsigned int CurrentUId;
		
				unsigned int uid;
				int type;
				string name;
		
				Node * parentNode;
				map<string, string> attributes;
				vector<Node *> children;
	
			public:
				Node( int type = Tag, string name = "" );
				~Node();
		
				Node * clone();
		
				void append( Node * node );
				void remove( Node * node );
		
				const string& getName() const;
				int getType() const;
		
				void attr( const string& name, const string& value );
				const bool hasAttr( const string& name ) const;
				const string& attr( const string& name );
				int attr attrAsInteger( const string& name ) const;
		
				Node * parent();
		
				bool test( const string& pattern );
				bool test( const string& tag, const map<string, string>& attributes );
		
				Node * find( const string& pattern );
		
				Node * next( const string& pattern = "" );
				Node * prev( const string& pattern = "" );
				Node * first( const string& pattern = "" );
				Node * last( const string& pattern = "" );
		
				Node * childAt( int index );
				int indexOf( const Node * node );
		
				string text( bool outer = false, int depth = 0 ) const;
				void show() const;
		};
	}
}

#endif

