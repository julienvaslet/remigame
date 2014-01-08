#ifndef _DATA_PARSER_NODEPARSER_H
#define _DATA_PARSER_NODEPARSER_H	1

#include <data/parser/Parser.h>
#include <data/node/Node.h>

using namespace std;

namespace data
{
	namespace parser
	{
		class NodeParser : public Parser
		{
			protected:		
				vector<string> selfClosedTags;
		
				node::Node * root;
				node::Node * currentNode;
		
				node::Node * findTagToClose( node::Node * node, const string& name );
		
				bool doctype();
				bool comment();
				bool tag();
				bool text();
				bool attribute();
		
	
			public:
				NodeParser( const string& content );
				~NodeParser();
		
				void addSelfClosedTag( const string& tag );
				void removeSelfClosedTag( const string& tag );
		
				node::Node * parse();
		};
	}
}

#endif

